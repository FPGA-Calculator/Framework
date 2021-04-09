#ifndef CLASSSIM_H
#define CLASSSIM_H

#include <QObject>

#include <memory>       // For std::unique_ptr
#include <verilated.h>  // Include common routines
#include "Vtop.h"       // Include model header, generated from Verilating "top.v"

class ClassSim : public QObject
{
    Q_OBJECT
public:
    explicit ClassSim(int argc, char *argv[]);
    ~ClassSim();

    int run(ulong ticks);
    void set_button(bool state) { button_state = state; }

signals:
    void lcd_write(bool rs, quint8 d);
    void led_changed(quint8 led);

private:
    // Using unique_ptr is similar to
    // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
    const std::unique_ptr<VerilatedContext> contextp {new VerilatedContext};

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v".
    // Using unique_ptr is similar to "Vtop* top = new Vtop" then deleting at end.
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vtop> top {new Vtop{contextp.get(), "TOP"}};

    bool lastE {};
    quint8 lastLED;
    bool button_state {1};
};

#endif // CLASSSIM_H
