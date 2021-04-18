#include "MainWindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef QT_DEBUG
    // In debug mode, use the resources from their source location since the qrc file
    // is not being refreshed (unless fully rebuilding). This saves time, but will
    // not work in WebAssembly. We don't debug in that environment, anyways.
    QFile::remove("lcd_cmd.txt"); // copy will not force...
    QFile::copy("../verilog/lcd_cmd.txt", "lcd_cmd.txt");
#else
    // Copy resource file into the file system on the target platform
    // so it can be read by the async_rom module (using the $readmemb function)
    QFile::copy(":/lcd_cmd", "lcd_cmd.txt");
#endif // QT_DEBUG

    ClassSim sim(argc, argv);

    MainWindow w(&sim);
    w.show();
    return a.exec();
}
