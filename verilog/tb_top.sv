//============================================================================
// Testbench for the top level design
//============================================================================

module test_bench_top();

timeunit 1ns;
timeprecision 100ps;

reg CLOCK_50 = 0;
bit BUTTON = 1;
bit RS;
bit E;
bit [7:0] D;
logic [2:0] LED;

initial begin : init
    $display("Start of test at %d", $time);
    // Ignore debounce module by asserting clean_signal early
    #10 force top.clean_signal = 0;
end : init

// Although the clock is going forever, we will stop simulation at some point
initial forever #10 CLOCK_50 = ~CLOCK_50;

top top(.*);

endmodule : test_bench_top
