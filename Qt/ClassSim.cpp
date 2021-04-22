#include "ClassSim.h"

ClassSim::ClassSim(int argc, char *argv[]) : QObject(nullptr)
{
    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);

    // Set Vtop's input signals
    top->BT_RESET = button_state;
    top->COL = 0b11111;
    top->ROW = 0b1111111;
    top->CLOCK_50 = 0;
}

ClassSim::~ClassSim()
{
    // Final model cleanup
    top->final();
}

// Run the simulation for the specified number of "ticks" which we define as
// half-clock periods. Returns either when this perid completed (ret 0) or
// when the simulation $finished (ret 1)
int ClassSim::run(ulong ticks)
{
    // Simulate until $finish or the specified number of clock ticks has elapsed
    while (!contextp->gotFinish() && ticks--)
    {
        contextp->timeInc(1);  // 1 timeprecision period passes...

        // Toggle a fast (time/2 period) clock
        top->CLOCK_50 = !top->CLOCK_50;

        // Set button state
        top->BT_RESET = button_state;

        // Evaluate model
        top->eval();

        // If the LCD Enable pin changed, emit the change
        if (bool(top->E) != lastE)
        {
            // Emit only on the negative edge of this signal
            if (!top->E)
                emit lcd_write(top->RS, top->D);
            lastE = top->E;
        }

        // If the LED state changed, emit the change
        quint8 led = top->LED & 0b111;
        if (led != lastLED)
        {
            emit led_changed(led);
            lastLED = led;
        }
    }

    if (contextp->gotFinish()) // Return 1 for finished simulation
        return 1;

    return 0; // Return 0 for number of ticks completed
}

// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }
