#include "WidgetLed.h"

WidgetLed::WidgetLed(QWidget *parent) : QWidget(parent), is_lit(false)
{
    m_led0 = new QImage(":/led0");
    m_led1 = new QImage(":/led1");
}

WidgetLed::~WidgetLed()
{
    delete m_led0;
    delete m_led1;
}

void WidgetLed::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QImage led_image = is_lit ? m_led1->copy() : m_led0->copy();

    QPainter qp(&led_image);
    qp.setRenderHint(QPainter::Antialiasing, true);

    painter.drawImage(QRect(0, 0, width(), height()), led_image);
}

void WidgetLed::lit(bool lit)
{
    if (lit != is_lit) // Repaint only if the LED state is different
    {
        is_lit = lit;
        repaint();
    }
}
