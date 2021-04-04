#ifndef LCD_H
#define LCD_H

#include <QPaintEngine>
#include <QLabel>
#include "vrEmuLcd.h"

class WidgetLcd : public QLabel
{
    Q_OBJECT
public:
    explicit WidgetLcd(QWidget *parent);
    ~WidgetLcd();

    void local_test();

public slots:
    void write(bool rs, uint8_t d);

private:
    void paintEvent(QPaintEvent *) override;

private:
    VrEmuLcd *m_lcd;
    QImage *m_image;
};

#endif // LCD_H
