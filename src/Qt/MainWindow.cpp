#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(ClassSim *sim) : QMainWindow(nullptr), ui(new Ui::MainWindow), m_sim(sim)
{
    ui->setupUi(this);

    m_lcd = ui->centralwidget->findChild<WidgetLcd *>("lcd");

    connect(sim, &ClassSim::lcd_write, m_lcd, &WidgetLcd::write);

//    QTimer::singleShot(1000, m_lcd, &WidgetLcd::local_test);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(periodic()));
    m_timer->start(100);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::periodic()
{
    int ret = m_sim->run(1000);
    if (ret == 1)
    {
        qDebug() << "Simulation finished";
        m_timer->stop();
    }
}
