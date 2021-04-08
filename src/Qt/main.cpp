#include "MainWindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Copy resource file into the file system on the target platform
    // so it can be read by the async_rom module (using the $readmemb construct)
    QFile::copy(":/lcd_cmd", "lcd_cmd.txt");

    ClassSim sim(argc, argv);

    MainWindow w(&sim);
    w.show();
    return a.exec();
}
