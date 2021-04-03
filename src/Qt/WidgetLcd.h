#ifndef LCD_H
#define LCD_H

#include <QPaintEngine>
#include <QLabel>

class WidgetLcd : public QLabel
{
    Q_OBJECT
public:
    explicit WidgetLcd(QWidget *parent);
    ~WidgetLcd();

private:
    void paintEvent(QPaintEvent *) override;

private:
    QImage *m_lcd;
};

#endif // LCD_H
