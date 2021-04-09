#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBasicTimer>
#include "ClassSim.h"
#include "WidgetLcd.h"
#include "WidgetLed.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(ClassSim *sim);
    ~MainWindow();

private slots:
    void led_update(quint8 led);
    void timerEvent(QTimerEvent *event) override;

private:
    Ui::MainWindow *ui;
    ClassSim *m_sim;
    QBasicTimer m_timer;

    WidgetLcd *m_lcd;
    WidgetLed *m_led0;
    WidgetLed *m_led1;
    WidgetLed *m_led2;
};
#endif // MAINWINDOW_H
