#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_lcd = vrEmuLcdNew(16, 2, EmuLcdRomA02);
}

MainWindow::~MainWindow()
{
    vrEmuLcdDestroy(m_lcd);

    delete ui;
}
