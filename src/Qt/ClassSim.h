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

signals:
    void lcd_write(bool rs, uint8_t d);

private:
    // Using unique_ptr is similar to
    // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
    const std::unique_ptr<VerilatedContext> contextp {new VerilatedContext};

    // Construct the Verilated model, from Vtop.h generated from Verilating "top.v".
    // Using unique_ptr is similar to "Vtop* top = new Vtop" then deleting at end.
    // "TOP" will be the hierarchical name of the module.
    const std::unique_ptr<Vtop> top {new Vtop{contextp.get(), "TOP"}};

    bool lastE {};
};

#endif // CLASSSIM_H
