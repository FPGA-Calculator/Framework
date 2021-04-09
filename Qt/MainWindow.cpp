#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(ClassSim *sim) : QMainWindow(nullptr), ui(new Ui::MainWindow), m_sim(sim)
{
    ui->setupUi(this);

    m_lcd = ui->centralwidget->findChild<WidgetLcd *>("lcd");
    m_led0 = ui->centralwidget->findChild<WidgetLed *>("led0");
    m_led1 = ui->centralwidget->findChild<WidgetLed *>("led1");
    m_led2 = ui->centralwidget->findChild<WidgetLed *>("led2");

    connect(sim, &ClassSim::lcd_write, m_lcd, &WidgetLcd::write);
    connect(sim, &ClassSim::led_changed, this, &MainWindow::led_update);
    connect(ui->btButton, &QPushButton::pressed, this, [this](){ m_sim->set_button(0); });
    connect(ui->btButton, &QPushButton::released, this, [this](){ m_sim->set_button(1); });

#if LCD_TEST
    QTimer::singleShot(1000, m_lcd, &WidgetLcd::local_test);
#endif // LCD_TEST

    m_timer.start(100, this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// TODO: This is not the correct way to run a (simulation) process continuously
void MainWindow::timerEvent(QTimerEvent *event)
{
    int ret = m_sim->run(5000000);
    if (ret == 1)
    {
        qDebug() << "Simulation finished";
        m_timer.stop();
    }
}

void MainWindow::led_update(quint8 led)
{
    led = ~led; // An LED is lit when the corresponding bit is 0
    m_led0->lit(led & 1);
    m_led1->lit(led & 2);
    m_led2->lit(led & 4);
    update();
}
