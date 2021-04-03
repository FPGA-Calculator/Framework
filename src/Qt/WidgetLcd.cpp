#include "WidgetLcd.h"

WidgetLcd::WidgetLcd(QWidget *parent) : QLabel(parent)
{
    m_lcd = new QImage(":/vrEmuLcd.gif");

    setFixedSize(m_lcd->width(), m_lcd->height());
}

WidgetLcd::~WidgetLcd()
{
    delete m_lcd;
}

void WidgetLcd::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QImage lcd_image = m_lcd->copy();

    QPainter qp(&lcd_image);
    qp.setRenderHint(QPainter::Antialiasing, true);

    painter.drawImage(QRect(0, 0, width(), height()), lcd_image);
}
