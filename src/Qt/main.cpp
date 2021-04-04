#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClassSim sim(argc, argv);
    MainWindow w(&sim);
    w.show();
    return a.exec();
}
