#ifndef WIDGETLED_H
#define WIDGETLED_H

#include <QWidget>
#include <QPaintEngine>

class WidgetLed : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetLed(QWidget *parent);
    ~WidgetLed();
    void lit(bool lit); // Turn this LED on (true) or off (false)

private:
    void paintEvent(QPaintEvent *) override;

    QImage *m_led0, *m_led1; // LED off and LED on images
    bool is_lit;

signals:

};

#endif // WIDGETLED_H
