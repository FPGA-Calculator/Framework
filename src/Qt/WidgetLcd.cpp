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
}
