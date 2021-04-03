//============================================================================
// Controller module
//============================================================================
`default_nettype none

module controller (
    input clock,
    input lcd_busy,
    input internal_reset,
    output reg [3:0] rom_address = 0,
    output reg data_ready = 0
);

reg current_lcd_state = 0;
reg halt = 0;

always_ff @(posedge clock) begin

    // Resets the demo on the push button
    if (internal_reset) begin
        rom_address <= 0;
        data_ready <= 0;
        current_lcd_state <= 0;
        halt <= 0;
    end

    // Stops the demo after one run through the ROM
    if (rom_address == 4'b1111) begin
        halt <= 1;
        data_ready <= 0;
        rom_address <= 0;
    end

    // Prevent the system from sending the first character during initialization
    if (rom_address == 0 && halt == 0) begin
        current_lcd_state <= 1;
    end

    // This logic monitors the LCD module busy flag when the LCD goes from busy to free,
    // the controller raises the data ready flag and the output of the ROM is presented
    // to the LCD module when the LCD goes from free to busy, the controller increments the
    // ROM address to be ready for the next cycle

    if (halt == 0) begin
        if (current_lcd_state != lcd_busy) begin
            current_lcd_state <= lcd_busy;
            if (lcd_busy == 0) begin
                data_ready <= 1;
            end
            else if (lcd_busy == 1) begin
                rom_address <= rom_address + 4'b1;
                data_ready <= 0;
            end
        end
    end
end

endmodule
