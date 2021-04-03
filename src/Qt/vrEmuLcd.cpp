/*
 * Troy's HD44780U Lcd Display Emulator
 * Copyright (c) 2020 Troy Schrapel
 * This code is licensed under the MIT license
 * https://github.com/visrealm/VrEmuLcd
 *
 * Modified by GD.
 */

#include "vrEmuLcd.h"
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <time.h>

/* PUBLIC CONSTANTS
 *---------------------------------------- */
const byte LCD_CMD_CLEAR                = 0b00000001;
const byte LCD_CMD_HOME                 = 0b00000010;

const byte LCD_CMD_ENTRY_MODE           = 0b00000100;
const byte LCD_CMD_ENTRY_MODE_INCREMENT = 0b00000010;
const byte LCD_CMD_ENTRY_MODE_DECREMENT = 0b00000000;
const byte LCD_CMD_ENTRY_MODE_SHIFT     = 0b00000001;

const byte LCD_CMD_DISPLAY              = 0b00001000;
const byte LCD_CMD_DISPLAY_ON           = 0b00000100;
const byte LCD_CMD_DISPLAY_CURSOR       = 0b00000010;
const byte LCD_CMD_DISPLAY_CURSOR_BLINK = 0b00000001;

const byte LCD_CMD_SHIFT                = 0b00010000;
const byte LCD_CMD_SHIFT_CURSOR         = 0b00000000;
const byte LCD_CMD_SHIFT_DISPLAY        = 0b00001000;
const byte LCD_CMD_SHIFT_LEFT           = 0b00000000;
const byte LCD_CMD_SHIFT_RIGHT          = 0b00000100;

const byte LCD_CMD_FUNCTION             = 0b00100000;
const byte LCD_CMD_FUNCTION_LCD_1LINE   = 0b00000000;
const byte LCD_CMD_FUNCTION_LCD_2LINE   = 0b00001000;

const byte LCD_CMD_SET_CGRAM_ADDR       = 0b01000000;
const byte LCD_CMD_SET_DRAM_ADDR        = 0b10000000;

/* PRIVATE CONSTANTS
 *---------------------------------------- */
#define CHAR_WIDTH_PX         5
#define CHAR_HEIGHT_PX        8

#define DATA_WIDTH_CHARS_1ROW 80
#define DATA_WIDTH_CHARS_2ROW 40
#define DATA_WIDTH_CHARS_4ROW 20
#define DDRAM_SIZE            128
#define DDRAM_VISIBLE_SIZE    80

#define DISPLAY_MIN_COLS      8
#define DISPLAY_MAX_COLS      40
#define DISPLAY_MIN_ROWS      1
#define DISPLAY_MAX_ROWS      4

#define CGRAM_STORAGE_CHARS   16
#define ROM_FONT_CHARS       (256 - CGRAM_STORAGE_CHARS)

#define CLOCK_TO_MS  (1.0 / (CLOCKS_PER_SEC / 1000.0))

#define CURSOR_MASK  (LCD_CMD_DISPLAY_CURSOR_BLINK | LCD_CMD_DISPLAY_CURSOR)
#define CURSOR_BLINK_PERIOD_MS  350
#define CURSOR_BLINK_CYCLE_MS  (2 * CURSOR_BLINK_PERIOD_MS)

#define DEFAULT_CGRAM_BYTE  0xaa

#include "font.h"

static int rowOffsets[] = { 0x00, 0x40, 0x14, 0x54 };

/* PRIVATE DATA STRUCTURE
 *---------------------------------------- */
struct vrEmuLcd_s
{
    // size in characters
    int cols;
    int rows;

    // current state
    byte entryModeFlags;
    byte displayFlags;
    int scrollOffset;

    // ddRam storage
    byte *ddRam;
    byte *ddPtr;
    int dataWidthCols;

    // cgRam storage
    byte cgRam[CGRAM_STORAGE_CHARS][CHAR_HEIGHT_PX];
    byte *cgPtr;

    // which character rom?
    vrEmuLcdCharacterRom characterRom;

    // display pixels
    char *pixels;
    int pixelsWidth;
    int pixelsHeight;
    int numPixels;
};

/*
 * Function:  increment
 * --------------------
 * increments the ddRam pointer of a VrEmuLcd
 *
 * automatically skips to the correct line and
 * rolls back to the start
 */
static void increment(VrEmuLcd *lcd)
{
    ++lcd->ddPtr;

    // find pointer offset from start
    int offset = lcd->ddPtr - lcd->ddRam;

    // 4 row mode's funky addressing scheme
    if (lcd->rows > 2)
    {
        if (offset == 0x28)
            lcd->ddPtr = lcd->ddRam + 0x40;
        else if ((offset == 0x68) || (offset >= DDRAM_SIZE))
            lcd->ddPtr = lcd->ddRam;
    }
    else if (offset >= DDRAM_VISIBLE_SIZE)
    {
        lcd->ddPtr = lcd->ddRam;
    }
}

/*
 * Function:  decrement
 * --------------------
 * decrements the ddRam pointer of a VrEmuLcd
 *
 * automatically skips to the correct line and
 * rolls back to the end
 */
static void decrement(VrEmuLcd *lcd)
{
    --lcd->ddPtr;

    // find pointer offset from start
    int offset = lcd->ddPtr - lcd->ddRam;

    // 4 row mode's funky addressing scheme
    if (lcd->rows > 2)
    {
        if (offset == -1)
            lcd->ddPtr = lcd->ddRam + 0x67;
        else if (offset == 0x39)
            lcd->ddPtr = lcd->ddRam + 0x27;
    }

    if (offset == -1)
        lcd->ddPtr += DDRAM_VISIBLE_SIZE;
    else if (offset >= DDRAM_SIZE)
        lcd->ddPtr = lcd->ddRam;
}

/*
 * Function:  doShiftDdram
 * --------------------
 * shift the cursor or display as required
 * by the current entry mode flags
 */
static void doShift(VrEmuLcd *lcd)
{
    // if we're looking at cgram, shift the cg pointer
    if (lcd->cgPtr)
    {
        if (lcd->entryModeFlags & LCD_CMD_ENTRY_MODE_INCREMENT)
        {
            ++lcd->cgPtr;
            if (lcd->cgPtr >= (byte*) lcd->cgRam + sizeof(lcd->cgRam))
                lcd->cgPtr = (byte*) lcd->cgRam;
        }
        else
        {
            --lcd->cgPtr;
            if (lcd->cgPtr < (byte*) lcd->cgRam)
                lcd->cgPtr = (byte*) lcd->cgRam + sizeof(lcd->cgRam) - 1;
        }
    }

    // otherwise, shift the ddram pointer or scroll offset
    else
    {
        if (lcd->entryModeFlags & LCD_CMD_ENTRY_MODE_SHIFT)
        {
            if (lcd->entryModeFlags & LCD_CMD_ENTRY_MODE_INCREMENT)
                ++lcd->scrollOffset;
            else
                --lcd->scrollOffset;
        }

        if (lcd->entryModeFlags & LCD_CMD_ENTRY_MODE_INCREMENT)
            increment(lcd);
        else
            decrement(lcd);
    }
}

/* Function:  vrEmuLcdNew
 * --------------------
 * create a new LCD
 *
 * cols: number of display columns  (8 to 40)
 * rows: number of display rows (1, 2 or 4)
 * rom:  character rom to load
 */
VrEmuLcd *vrEmuLcdNew(int cols, int rows, vrEmuLcdCharacterRom rom)
{
    // validate display size
    if (cols < DISPLAY_MIN_COLS)
        cols = DISPLAY_MIN_COLS;
    else if (cols > DISPLAY_MAX_COLS)
        cols = DISPLAY_MAX_COLS;

    if (rows < DISPLAY_MIN_ROWS)
        rows = DISPLAY_MIN_ROWS;
    else if (rows > DISPLAY_MAX_ROWS)
        rows = DISPLAY_MAX_ROWS;

    if (rows == 3)
        rows = 2;

    // build lcd data structure
    VrEmuLcd *lcd = (VrEmuLcd*) malloc(sizeof(VrEmuLcd));
    if (lcd != NULL)
    {
        lcd->cols = cols;
        lcd->rows = rows;
        lcd->characterRom = rom;

        lcd->ddRam = (byte*) malloc(DDRAM_SIZE);
        lcd->ddPtr = lcd->ddRam;
        lcd->entryModeFlags = LCD_CMD_ENTRY_MODE_INCREMENT;
        lcd->displayFlags = 0x00;
        lcd->scrollOffset = 0x00;
        lcd->cgPtr = NULL;
        lcd->pixelsWidth = lcd->cols *(CHAR_WIDTH_PX + 1) - 1;
        lcd->pixelsHeight = lcd->rows *(CHAR_HEIGHT_PX + 1) - 1;
        lcd->numPixels = lcd->pixelsWidth *lcd->pixelsHeight;
        lcd->pixels = (char*) malloc(lcd->numPixels);

        switch (lcd->rows)
        {
            case 1: lcd->dataWidthCols = DATA_WIDTH_CHARS_1ROW; break;
            case 2: lcd->dataWidthCols = DATA_WIDTH_CHARS_2ROW; break;
            case 4: lcd->dataWidthCols = DATA_WIDTH_CHARS_4ROW; break;
        }

        // fill arrays with default data
        if (lcd->ddRam != NULL)
            memset(lcd->ddRam, ' ', DDRAM_SIZE);

        memset(lcd->cgRam, DEFAULT_CGRAM_BYTE, sizeof(lcd->cgRam));
        memset(lcd->pixels, -1, lcd->numPixels);

        vrEmuLcdUpdatePixels(lcd);
    }

    return lcd;
}

/*
 * Function:  vrEmuLcdDestroy
 * --------------------
 * destroy an LCD
 *
 * lcd: lcd object to destroy / clean up
 */
void vrEmuLcdDestroy(VrEmuLcd *lcd)
{
    if (lcd)
    {
        free(lcd->ddRam);
        free(lcd->pixels);
        memset(lcd, 0, sizeof(VrEmuLcd));
        free(lcd);
    }
}

/*
 * Function:  vrEmuLcdSendCommand
 * --------------------
 * send a command to the lcd (RS is low)
 *
 * byte: the data (DB0 -> DB7) to send
 */
void vrEmuLcdSendCommand(VrEmuLcd *lcd, byte command)
{
    if (command & LCD_CMD_SET_DRAM_ADDR)
    {
        // ddram address in remaining 7 bits
        int offset = (command & 0x7f);
        lcd->ddPtr = lcd->ddRam + offset;
        lcd->cgPtr = NULL;
    }
    else if (command & LCD_CMD_SET_CGRAM_ADDR)
    {
        // cgram address in remaining 6 bits
        lcd->cgPtr = (byte*) lcd->cgRam + (command & 0x3f);
    }
    else if (command & LCD_CMD_FUNCTION)
    {
        // ignore
    }
    else if (command & LCD_CMD_SHIFT)
    {
        if (command & LCD_CMD_SHIFT_DISPLAY)
        {
            if (command & LCD_CMD_SHIFT_RIGHT)
                --lcd->scrollOffset;
            else
                ++lcd->scrollOffset;
        }
        else
        {
            if (command & LCD_CMD_SHIFT_RIGHT)
                increment(lcd);
            else
                decrement(lcd);
        }
    }
    else if (command & LCD_CMD_DISPLAY)
    {
        lcd->displayFlags = command;
    }
    else if (command & LCD_CMD_ENTRY_MODE)
    {
        lcd->entryModeFlags = command;
    }
    else if (command & LCD_CMD_HOME)
    {
        lcd->ddPtr = lcd->ddRam;
        lcd->scrollOffset = 0;
    }
    else if (command & LCD_CMD_CLEAR)
    {
        if (lcd->ddRam != NULL)
            memset(lcd->ddRam, ' ', DDRAM_SIZE);

        lcd->ddPtr = lcd->ddRam;
        lcd->scrollOffset = 0;
    }
}

/*
 * Function:  vrEmuLcdWriteByte
 * --------------------
 * write a byte to the lcd (RS is high)
 *
 * byte: the data (DB0 -> DB7) to send
 */
void vrEmuLcdWriteByte(VrEmuLcd *lcd, byte data)
{
    if (lcd->cgPtr)
    {
        // find row offset
        int row = (lcd->cgPtr - (byte*) lcd->cgRam) % CHAR_HEIGHT_PX;

        // find starting byte for the current character
        byte *startAddr = lcd->cgPtr - row;

        for (int i = 0; i < CHAR_WIDTH_PX; ++i)
        {
            byte bit = data &((0x01 << (CHAR_WIDTH_PX - 1)) >> i);
            if (bit)
                *(startAddr + i) |= (0x80 >> row);
            else
                *(startAddr + i) &= ~(0x80 >> row);
        }
    }
    else
    {
        *lcd->ddPtr = data;
    }

    doShift(lcd);
}

/*
 * Function:  vrEmuLcdReadByte
 * --------------------
 * read a byte from the lcd (RS is high)
 *
 * returns: the data (DB0 -> DB7) at the current address
 */
byte vrEmuLcdReadByte(VrEmuLcd *lcd)
{
    byte data = 0;

    if (lcd->cgPtr)
    {
        // find row offset
        int row = (lcd->cgPtr - (byte*) lcd->cgRam) % 8;

        // find starting byte for the current character
        byte *startAddr = lcd->cgPtr - row;

        for (int i = 0; i < CHAR_WIDTH_PX; ++i)
        {
            if (*(startAddr + i) &(0x80 >> row))
                data |= ((0x01 << (CHAR_WIDTH_PX - 1)) >> i);
        }
    }
    else
        data = *lcd->ddPtr;

    doShift(lcd);

    return data;
}

/* Function:  vrEmuLcdReadAddress
 * --------------------
 * read the current address offset (RS is high, R/W is high)
 *
 * returns: the current address
 */
byte vrEmuLcdReadAddress(VrEmuLcd *lcd)
{
    if (lcd->cgPtr)
        return (lcd->cgPtr - (byte*) lcd->cgRam) & 0x3f;
    return (lcd->ddPtr - lcd->ddRam) & 0x7f;
}

/*
 * Function:  vrEmuLcdWriteString
 * ----------------------------------------
 * write a string to the lcd
 * iterates over the characters and sends them individually
 *
 * str: the string to write.
 */
void vrEmuLcdWriteString(VrEmuLcd *lcd, const char *str)
{
    const char *ddPtr = str;
    while (*ddPtr != '\0')
    {
        vrEmuLcdWriteByte(lcd, *ddPtr);
        ++ddPtr;
    }
}

/*
 * Function:  vrEmuLcdCharBits
 * ----------------------------------------
 * return a character's pixel data
 *
 * pixel data consists of 5 bytes where each is
 * a vertical row of bits for the character
 *
 * c: character index
 *    0 - 15   cgram
 *    16 - 255 rom
 */
const byte *vrEmuLcdCharBits(VrEmuLcd *lcd, byte c)
{
    if (c < CGRAM_STORAGE_CHARS)
        return lcd->cgRam[c];

    const int characterRomIndex = c - CGRAM_STORAGE_CHARS;

    switch (lcd->characterRom)
    {
        case EmuLcdRomA00:
            return fontA00[characterRomIndex];
        case EmuLcdRomA02:
        default:
            return fontA02[characterRomIndex];
    }
}

/*
 * Function:  vrEmuLcdGetDataOffset
 * ----------------------------------------
 * return the character offset in ddram for a given
 * row and column.
 *
 * can be used to set the current cursor address
 */
int vrEmuLcdGetDataOffset(VrEmuLcd *lcd, int row, int col)
{
    // adjust for display scroll offset
    if (row >= lcd->rows) row = lcd->rows - 1;

    while (lcd->scrollOffset< 0)
        lcd->scrollOffset += lcd->dataWidthCols;

    int dataCol = (col + lcd->scrollOffset) % lcd->dataWidthCols;
    int rowOffset = row *lcd->dataWidthCols;

    if (lcd->rows > 2)
        rowOffset = rowOffsets[row];

    return rowOffset + dataCol;
}

/*
 * Function:  vrEmuLcdUpdatePixels
 * ----------------------------------------
 * updates the display's pixel data
 * changes are only reflected in the pixel data when this function is called
 */
void vrEmuLcdUpdatePixels(VrEmuLcd *lcd)
{
    // determine cursor blink state
    int cursorOn = lcd->displayFlags & CURSOR_MASK;
    if (lcd->displayFlags & LCD_CMD_DISPLAY_CURSOR_BLINK)
    {
        if (((int)(clock() * CLOCK_TO_MS) % CURSOR_BLINK_CYCLE_MS) < CURSOR_BLINK_PERIOD_MS)
        {
            cursorOn &= ~LCD_CMD_DISPLAY_CURSOR_BLINK;
        }
    }

    int displayOn = lcd->displayFlags & LCD_CMD_DISPLAY_ON;

    // /cycle through each row of the display
    for (int row = 0; row < lcd->rows; row++)
    {
        for (int col = 0; col < lcd->cols; col++)
        {
            // find top-left pixel for the current display character position
            char *charTopLeft = lcd->pixels + (row * (CHAR_HEIGHT_PX + 1) * lcd->pixelsWidth) + col * (CHAR_WIDTH_PX + 1);

            // find current character in ddram
            byte *ddPtr = lcd->ddRam + vrEmuLcdGetDataOffset(lcd, row, col);

            // only draw cursor if the data pointer is pointing at this character
            int drawCursor = cursorOn && (ddPtr == lcd->ddPtr);

            // get the character data (bits) for the current character
            const byte *bits = vrEmuLcdCharBits(lcd, *ddPtr);

            // apply its bits to the pixel data
            for (int y = 0; y < CHAR_HEIGHT_PX; y++)
            {
                // set pixel pointer
                char *pixel = charTopLeft + y * lcd->pixelsWidth;
                for (int x = 0; x < CHAR_WIDTH_PX; x++)
                {
                    // is the display on?
                    if (!displayOn)
                    {
                        *pixel = 0;
                        continue;
                    }

                    // set the pixel data from the character bits
                    *pixel = (bits[x] &(0x80 >> y)) ? 1 : 0;

                    // override with cursor data if appropriate
                    if (drawCursor)
                    {
                        if ((cursorOn & LCD_CMD_DISPLAY_CURSOR_BLINK) ||
                            ((cursorOn & LCD_CMD_DISPLAY_CURSOR) && (y == (CHAR_HEIGHT_PX - 1))))
                            *pixel = 1;
                    }

                    // next pixel
                    pixel++;
                }
            }
        }
    }
}

/*
 * Function:  vrEmuLcdNumPixels
 * ----------------------------------------
 * get the number of pixels for the entire display
 */
void vrEmuLcdNumPixels(VrEmuLcd *lcd, int *cols, int *rows)
{
    if (cols) *cols = vrEmuLcdNumPixelsX(lcd);
    if (rows) *rows = vrEmuLcdNumPixelsY(lcd);
}

/*
 * Function:  vrEmuLcdNumPixelsX
 * ----------------------------------------
 * returns: number of horizontal pixels in the display
 */
int vrEmuLcdNumPixelsX(VrEmuLcd *lcd)
{
    return lcd->pixelsWidth;
}

/*
 * Function:  vrEmuLcdNumPixelsY
 * ----------------------------------------
 * returns: number of vertical pixels in the display
 */
int vrEmuLcdNumPixelsY(VrEmuLcd *lcd)
{
    return lcd->pixelsHeight;
}

/*
 * Function:  charvrEmuLcdPixelState
 * ----------------------------------------
 * returns: pixel state at the given location
 *
 * -1 = no pixel (character borders)
 *  0 = pixel off
 *  1 = pixel on
 *
 */
char vrEmuLcdPixelState(VrEmuLcd *lcd, int x, int y)
{
    int offset = y * lcd->pixelsWidth + x;
    if ((offset >= 0) && (offset < lcd->numPixels))
        return lcd->pixels[offset];
    return -1;
}
