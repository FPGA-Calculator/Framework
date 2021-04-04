QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../verilator/include
INCLUDEPATH += $$PWD/../verilog/obj_dir

SOURCES += \
    ../verilator/include/verilated.cpp \
    ../verilog/obj_dir/Vtop.cpp \
    ../verilog/obj_dir/Vtop__ALL.cpp \
    ClassSim.cpp \
    WidgetLcd.cpp \
    main.cpp \
    MainWindow.cpp \
    vrEmuLcd.cpp

HEADERS += \
    ../verilator/include/verilated.h \
    ../verilog/obj_dir/Vtop.h \
    ClassSim.h \
    MainWindow.h \
    WidgetLcd.h \
    font.h \
    vrEmuLcd.h

FORMS += \
    MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
