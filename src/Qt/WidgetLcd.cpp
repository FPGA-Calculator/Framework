#include "WidgetLcd.h"

WidgetLcd::WidgetLcd(QWidget *parent) : QLabel(parent)
{
    m_lcd = vrEmuLcdNew(16, 2, EmuLcdRomA02);

    m_image = new QImage(":/vrEmuLcd.gif");

    setFixedSize(m_image->width(), m_image->height());
}

WidgetLcd::~WidgetLcd()
{
    vrEmuLcdDestroy(m_lcd);

    delete m_image;
}

void WidgetLcd::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QImage lcd_image = m_image->copy();

    QPainter qp(&lcd_image);
    qp.setRenderHint(QPainter::Antialiasing, true);

    painter.drawImage(QRect(0, 0, width(), height()), lcd_image);

    const int xstart = 78;
    const int ystart = 90;

    const int numPixelsX = vrEmuLcdNumPixelsX(m_lcd);
    const int numPixelsY = vrEmuLcdNumPixelsY(m_lcd);

    for (int y = 0; y < numPixelsY; y++)
    {
        for (int x = 0; x < numPixelsX; x++)
        {
            char c = vrEmuLcdPixelState(m_lcd, x, y);

            if (c == 1)
            {
                painter.fillRect(xstart + x * 4, ystart + y * 4, 3, 3, QBrush(Qt::yellow));
            }
        }
    }
}

void WidgetLcd::restart()
{
    vrEmuLcdSendCommand(m_lcd, 0b00110000); // set data to Function Set instruction
    vrEmuLcdSendCommand(m_lcd, 0b00111000); // Configuration: 8-bit, 2 lines, 5x7 font
    vrEmuLcdSendCommand(m_lcd, 0b00001000); // Display Off command
    vrEmuLcdSendCommand(m_lcd, 0b00000001); // Clear command
    vrEmuLcdSendCommand(m_lcd, 0b00000110); // Entry Mode:cursor moves, display stands still
    vrEmuLcdSendCommand(m_lcd, 0b00001110); // Display On + cursor On

    vrEmuLcdSendCommand(m_lcd, 0b10000000); // Set address 0 (first char / first line)
    vrEmuLcdWriteByte(m_lcd, 0b01001000);
    vrEmuLcdWriteByte(m_lcd, 0b01100101);
    vrEmuLcdWriteByte(m_lcd, 0b01101100);
    vrEmuLcdWriteByte(m_lcd, 0b01101100);
    vrEmuLcdWriteByte(m_lcd, 0b01101111);
    vrEmuLcdSendCommand(m_lcd, 0b11000000); // Set address 40h (first char / second line)
    vrEmuLcdWriteByte(m_lcd, 0b01010111);
    vrEmuLcdWriteByte(m_lcd, 0b01101111);
    vrEmuLcdWriteByte(m_lcd, 0b01110010);
    vrEmuLcdWriteByte(m_lcd, 0b01101100);
    vrEmuLcdWriteByte(m_lcd, 0b01100100);
    vrEmuLcdWriteByte(m_lcd, 0b00100000);
    vrEmuLcdWriteByte(m_lcd, 0b00100000);
    vrEmuLcdSendCommand(m_lcd, 0b10001000); // Leave cursor at the last char / first line
    vrEmuLcdWriteByte(m_lcd, 0b00100000); // Need to write a char to make cursor visible

    vrEmuLcdUpdatePixels(m_lcd);

    update();
}
