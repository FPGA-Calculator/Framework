#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QLabel *l = ui->centralwidget->findChild<QLabel *>("lcd");
    QPixmap pic(":/vrEmuLcd.gif");
    l->setPixmap(pic);

    m_lcd = vrEmuLcdNew(16, 2, EmuLcdRomA02);
}

MainWindow::~MainWindow()
{
    vrEmuLcdDestroy(m_lcd);
    delete ui;
}
