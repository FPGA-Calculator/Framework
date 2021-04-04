#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_lcd = ui->centralwidget->findChild<WidgetLcd *>("lcd");

    QTimer::singleShot(1000, m_lcd, &WidgetLcd::restart);
}

MainWindow::~MainWindow()
{
    delete ui;
}
