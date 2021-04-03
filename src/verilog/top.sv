//============================================================================
// This is the top module
//============================================================================
`default_nettype none

module top
(
    input CLOCK_50,
    input BUTTON,
    output RS,
    output E,
    output [7:0] D
);

reg internal_reset = 0;
reg last_signal = 0;
wire clean_signal;
wire data_ready;
wire lcd_busy;
wire [8:0] d_in;
wire [3:0] rom_in;

initial begin
    if ($test$plusargs("trace") != 0) begin
        $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
        $dumpfile("logs/vlt_dump.vcd");
        $dumpvars();
    end
        $display("[%0t] Model running...\n", $time);
end

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

rom rom(
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

endmodule
