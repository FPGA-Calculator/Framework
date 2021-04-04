#include "ClassSim.h"

ClassSim::ClassSim(int argc, char *argv[]) : QObject(nullptr)
{
    // Prevent unused variable warnings
    if (false && argc && argv) {}

    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");

    // Construct a VerilatedContext to hold simulation time, etc.
    // Multiple modules (made later below with Vtop) may share the same
    // context to share time, or modules may have different contexts if
    // they should be independent from each other.

    // Set debug level, 0 is off, 9 is highest presently used
    // May be overridden by commandArgs argument parsing
    contextp->debug(0);

    // Randomization reset policy
    // May be overridden by commandArgs argument parsing
    contextp->randReset(2);

    // Verilator must compute traced signals
    contextp->traceEverOn(true);

    // Pass arguments so Verilated code can see them, e.g. $value$plusargs
    // This needs to be called before you create any model
    contextp->commandArgs(argc, argv);

    // Set Vtop's input signals
    top->BUTTON = !0;
    top->CLOCK_50 = 0;
}

ClassSim::~ClassSim()
{
    // Final model cleanup
    top->final();
}

int ClassSim::run(ulong ticks)
{
    Q_UNUSED(ticks);

    // Simulate until $finish
    while (!contextp->gotFinish())
    {
        // Historical note, before Verilator 4.200 Verilated::gotFinish()
        // was used above in place of contextp->gotFinish().
        // Most of the contextp-> calls can use Verilated:: calls instead;
        // the Verilated:: versions simply assume there's a single context
        // being used (per thread).  It's faster and clearer to use the
        // newer contextp-> versions.

        contextp->timeInc(1);  // 1 timeprecision period passes...
        // Historical note, before Verilator 4.200 a sc_time_stamp()
        // function was required instead of using timeInc.  Once timeInc()
        // is called (with non-zero), the Verilated libraries assume the
        // new API, and sc_time_stamp() will no longer work.

        // Toggle a fast (time/2 period) clock
        top->CLOCK_50 = !top->CLOCK_50;

        // Toggle control signals on an edge that doesn't correspond
        // to where the controls are sampled; in this example we do
        // this only on a negedge of CLOCK_50, because we know
        // reset is not sampled there.
        if (!top->CLOCK_50) {
            if (contextp->time() > 1 && contextp->time() < 10) {
                top->BUTTON = !1;  // Assert reset
            } else {
                top->BUTTON = !0;  // Deassert reset
            }
        }

        // Evaluate model
        // (If you have multiple models being simulated in the same
        // timestep then instead of eval(), call eval_step() on each, then
        // eval_end_step() on each. See the manual.)
        top->eval();

        if (bool(top->E) != lastE)
        {
            if (!top->E)
                emit lcd_write(top->RS, top->D);

            lastE = top->E;
        }
    }

    if (contextp->gotFinish()) // Return 1 for finished simulation
        return 1;

    return 0; // Return 0 for number of ticks completed
}

// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }
