/*
 * Troy's HD44780U Lcd Display Emulator
 * Copyright (c) 2020 Troy Schrapel
 * This code is licensed under the MIT license
 * https://github.com/visrealm/VrEmuLcd
 *
 * Modified by GD.
 */

#ifndef _VR_EMU_LCD_H_
#define _VR_EMU_LCD_H_

#undef byte
typedef unsigned char byte;

/* PRIVATE DATA STRUCTURE
 * ---------------------------------------- */
struct vrEmuLcd_s;
typedef struct vrEmuLcd_s VrEmuLcd;

/* PUBLIC CONSTANTS
 * ---------------------------------------- */
extern const byte LCD_CMD_CLEAR;
extern const byte LCD_CMD_HOME;

extern const byte LCD_CMD_ENTRY_MODE;
extern const byte LCD_CMD_ENTRY_MODE_INCREMENT;
extern const byte LCD_CMD_ENTRY_MODE_DECREMENT;
extern const byte LCD_CMD_ENTRY_MODE_SHIFT;

extern const byte LCD_CMD_DISPLAY;
extern const byte LCD_CMD_DISPLAY_ON;
extern const byte LCD_CMD_DISPLAY_CURSOR;
extern const byte LCD_CMD_DISPLAY_CURSOR_BLINK;

extern const byte LCD_CMD_SHIFT;
extern const byte LCD_CMD_SHIFT_CURSOR;
extern const byte LCD_CMD_SHIFT_DISPLAY;
extern const byte LCD_CMD_SHIFT_LEFT;
extern const byte LCD_CMD_SHIFT_RIGHT;

extern const byte LCD_CMD_FUNCTION;
extern const byte LCD_CMD_FUNCTION_LCD_1LINE;
extern const byte LCD_CMD_FUNCTION_LCD_2LINE;

extern const byte LCD_CMD_SET_CGRAM_ADDR;
extern const byte LCD_CMD_SET_DRAM_ADDR;

typedef enum
{
    EmuLcdRomA00,  // Japanese
    EmuLcdRomA02   // European
} vrEmuLcdCharacterRom;

/* PUBLIC INTERFACE
 * ---------------------------------------- */

/* Function:  vrEmuLcdNew
 * --------------------
 * create a new LCD
 *
 * cols: number of display columns  (8 to 40)
 * rows: number of display rows (1, 2 or 4)
 * rom:  character rom to load
 */
VrEmuLcd *vrEmuLcdNew(int width, int height, vrEmuLcdCharacterRom rom);

/* Function:  vrEmuLcdDestroy
 * --------------------
 * destroy an LCD
 *
 * lcd: lcd object to destroy / clean up
 */
void vrEmuLcdDestroy(VrEmuLcd *lcd);

/* Function:  vrEmuLcdSendCommand
 * --------------------
 * send a command to the lcd (RS is low, R/W is low)
 *
 * byte: the data (DB0 -> DB7) to send
 */
void vrEmuLcdSendCommand(VrEmuLcd *lcd, byte data);


/* Function:  vrEmuLcdWriteByte
 * --------------------
 * write a byte to the lcd (RS is high, R/W is low)
 *
 * byte: the data (DB0 -> DB7) to send
 */
void vrEmuLcdWriteByte(VrEmuLcd *lcd, byte data);

/* Function:  vrEmuLcdWriteString
 * ----------------------------------------
 * write a string to the lcd
 * iterates over the characters and sends them individually
 *
 * str: the string to write.
 */
void vrEmuLcdWriteString(VrEmuLcd *lcd, const char *str);


/* Function:  vrEmuLcdGetDataOffset
 * ----------------------------------------
 * return the character offset in ddram for a given
 * row and column on the display.
 *
 * can be used to set the current cursor address
 */
int vrEmuLcdGetDataOffset(VrEmuLcd *lcd, int row, int col);

/* Function:  vrEmuLcdReadByte
 * --------------------
 * read a byte from the lcd (RS is high, R/W is high)
 *
 * returns: the data (DB0 -> DB7) at the current address
 */
byte vrEmuLcdReadByte(VrEmuLcd *lcd);

/* Function:  vrEmuLcdReadAddress
 * --------------------
 * read the current address offset (RS is high, R/W is high)
 *
 * returns: the current address offset (either CGRAM or DDRAM)
 */
byte vrEmuLcdReadAddress(VrEmuLcd *lcd);


/* Function:  vrEmuLcdCharBits
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
const byte *vrEmuLcdCharBits(VrEmuLcd *lcd, byte c);

/* Function:  vrEmuLcdUpdatePixels
 * ----------------------------------------
 * updates the display's pixel data
 * changes are only reflected in the pixel data when this function is called
 */
void vrEmuLcdUpdatePixels(VrEmuLcd *lcd);

/* Function:  vrEmuLcdNumPixels
 * ----------------------------------------
 * get the size of the entire display in pixels (including unused border pixels)
 */
void vrEmuLcdNumPixels(VrEmuLcd *lcd, int *width, int *height);

/* Function:  vrEmuLcdNumPixelsX
 * ----------------------------------------
 * returns: number of horizontal pixels in the display
 */
int vrEmuLcdNumPixelsX(VrEmuLcd *lcd);

/* Function:  vrEmuLcdNumPixelsY
 * ----------------------------------------
 * returns: number of vertical pixels in the display
 */
int vrEmuLcdNumPixelsY(VrEmuLcd *lcd);

/* Function:  charvrEmuLcdPixelState
 * ----------------------------------------
 * returns: pixel state at the given location
 *
 * -1 = no pixel (character borders)
 *  0 = pixel off
 *  1 = pixel on
 *
 */
char vrEmuLcdPixelState(VrEmuLcd *lcd, int x, int y);

#endif // _VR_EMU_LCD_H_
