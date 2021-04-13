//============================================================================
// This is the top module
//============================================================================
`default_nettype none

module top
(
    input wire CLOCK_50,
    input wire BUTTON,
    output logic RS,
    output logic E,
    output logic [7:0] D,
    output logic [2:0] LED
);
// Define the output trace file, which, as a parameter, can be overriden
parameter TRACE_FILE="logs/vlt_dump.vcd";

reg internal_reset = 0;
reg last_signal = 0;
logic clean_signal;
logic data_ready;
logic lcd_busy;
logic [8:0] d_in;
logic [3:0] rom_in;

`ifndef QUARTUS
initial begin
    if ($test$plusargs("trace") != 0) begin
        $display("[%0t] Tracing file: %s\n", $time, TRACE_FILE);
        $dumpfile(TRACE_FILE);
        $dumpvars();
    end
        $display("[%0t] Model running...\n", $time);
end
`endif

lcd lcd(
    .clock(CLOCK_50),
    .internal_reset(internal_reset),
    .d_in(d_in),
    .data_ready(data_ready),
    .rs(RS),
    .e(E),
    .d(D),
    .busy_flag(lcd_busy)
);

async_rom #(.ROM_FILE("lcd_cmd.txt")) rom(
    .rom_in(rom_in),
    .rom_out(d_in)
);

controller controller (
    .clock(CLOCK_50),
    .lcd_busy(lcd_busy),
    .internal_reset(internal_reset),
    .rom_address(rom_in),
    .data_ready(data_ready)
);

debounce debounce(
    .clk (CLOCK_50),
    .in  (BUTTON),
    .out (clean_signal)
);

always_ff @ (posedge CLOCK_50) begin
    if (last_signal != clean_signal) begin
        last_signal <= clean_signal;
        if (clean_signal == 0) begin
            internal_reset <= 1;
        end
    end
    else begin
        internal_reset <= 0;
    end
end

//----------------------------------------------------------------------------
// Drives 3 LEDs on the test board:
// LED2: synchronous button read, without debounce
// LED1: shows the reset + LCD initialization duration
// LED0: blinks with a period of 1s (based on the 50MHz input clock)

logic [31:0] counter = 0;
logic button = 1;
logic blink = 0;

always_ff @(posedge CLOCK_50) begin
    if (counter == (50000000 / 2)) begin
        counter <= 0;
        blink <= ~blink;
    end
    else
        counter <= counter + 1;
end

always_ff @(posedge CLOCK_50) begin
    button <= BUTTON;
end

assign {LED[2], LED[1], LED[0]} = {button, clean_signal, blink};

endmodule : top
