#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ClassSim.h"
#include "WidgetLcd.h"

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
    void periodic();

private:
    Ui::MainWindow *ui;
    WidgetLcd *m_lcd;
    ClassSim *m_sim;
    QTimer *m_timer;
};
#endif // MAINWINDOW_H
