//============================================================================
// This module drives an 16x2 OLED LCD
//
// This particular OLED needs the following connections:
//
// 1 GND
// 2 VDD to +5V
// 3 (not connected)
// 4 RS
// 5 GND (was R/W)
// 6 E
// 7..14 D[7:0]
// 15 (not connected)
// 16 (not connected)
//
//============================================================================
`default_nettype none

// This "Hello World" project is intended to demonstrate integration of
// an HD44780-compatible LCD device with an FPGA.
//
// Please see robotics.hobbizine/fpgalcd.html for a full discussion.

module lcd(
    input clock,
    input internal_reset,
    input [8:0] d_in,
    input data_ready,
    output reg rs = 0,
    output reg e = 0,
    output reg [7:0] d = 0,
    output reg busy_flag = 0
);

parameter CLK_FREQ = 50000000;

localparam int D_50ns  = int'(0.000000050 * CLK_FREQ);
localparam int D_250ns = int'(0.000000250 * CLK_FREQ);

localparam int D_40us  = int'(0.000040000 * CLK_FREQ);
localparam int D_60us  = int'(0.000060000 * CLK_FREQ);
localparam int D_200us = int'(0.000200000 * CLK_FREQ);

localparam int D_2ms   = int'(0.002000000 * CLK_FREQ);
localparam int D_5ms   = int'(0.005000000 * CLK_FREQ);
localparam int D_100ms = int'(0.100000000 * CLK_FREQ);

typedef enum logic[4:0] {
    STATE00, STATE01, STATE02, STATE03, STATE04, STATE05, STATE06, STATE07, STATE08, STATE09, STATE10, STATE11, STATE12,
    STATE13, STATE14, STATE15, STATE16, STATE17, STATE18, STATE19, STATE20, STATE21, STATE22, STATE23, STATE24, STATE25
} state_t;

state_t state = STATE00;
reg [23:0] count = 0;
reg start = 0;

always_ff @(posedge clock) begin
    if (data_ready) begin
        start <= 1;
    end
    if (internal_reset) begin
        state <= STATE00;
        count <= 0;
    end

    case (state)

    // Step 1 - 100ms delay after power on
    STATE00: begin
        busy_flag <= 1;                     // tells other modules LCD is processing
        if (int'(count) == D_100ms) begin   // if 100ms have elapsed
            rs <= 0;                        // pull RS low to indicate instruction
            d  <= 8'b00110000;              // set data to Function Set instruction
            count <= 0;                     // clear the counter
            state <= STATE01;               // advance to the next state
        end
        else begin                          // if 100ms have not elapsed
            count <= count + 24'b1;         // increment the counter
        end
    end

    // Steps 2 thru 4 raise and lower the enable pin three times to enter the
    // Function Set instruction that was loaded to the databus in STATE00 above

    // Step 2 - first Function Set instruction
    STATE01: begin
        if (int'(count) == D_50ns) begin    // if 50ns have elapsed (lets RS and D settle)
            e <= 1;                         // bring E high to initiate data write
            count <= 0;                     // clear the counter
            state <= STATE02;               // advance to the next state
        end
        else begin                          // otherwise
            count <= count + 24'b1;         // increment the counter
        end
    end

    STATE02: begin
        if (int'(count) == D_250ns) begin   // if 250ns have elapsed
            e <= 0;                         // bring E low
            count <= 0;                     // clear the counter
            state <= STATE03;               // advance to the next state
        end
        else begin                          // otherwise
            count <= count + 24'b1;         // increment the counter
        end
    end

    STATE03: begin
        if (int'(count) == D_5ms) begin     // if 5ms have elapsed
            e <= 1;                         // bring E high to initiate data write
            count <= 0;                     // clear the counter
            state <= STATE04;               // advance to the next state
        end
        else begin                          // otherwise
            count <= count + 24'b1;         // increment the counter
        end
    end

    // Step 3 - second Function Set instruction
    STATE04: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE05;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE05: begin
        if (int'(count) == D_200us) begin
            e <= 1;
            count <= 0;
            state <= STATE06;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // Step 4 - third and final Function Set instruction
    STATE06: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE07;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE07: begin
        if (int'(count) == D_200us) begin
            d  <= 8'b00111000;              // Configuration: 8-bit, 2 lines, 5x7 font
            count <= 0;
            state <= STATE08;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // Step 5 - enter the Configuation command
    STATE08: begin
        if (int'(count) == D_50ns) begin
            e <= 1;
            count <= 0;
            state <= STATE09;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE09: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE10;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE10: begin
        if (int'(count) == D_60us) begin
            d  <= 8'b00001000;              // Display Off command
            count <= 0;
            state <= STATE11;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // Step 6 - enter the Display Off command
    STATE11: begin
        if (int'(count) == D_50ns) begin
            e <= 1;
            count <= 0;
            state <= STATE12;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE12: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE13;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE13: begin
        if (int'(count) == D_60us) begin
            d  <= 8'b00000001;              // Clear command
            count <= 0;
            state <= STATE14;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // Step 7 - enter the Clear command
    STATE14: begin
        if (int'(count) == D_50ns) begin
            e <= 1;
            count <= 0;
            state <= STATE15;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE15: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE16;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE16: begin
        if (int'(count) == D_5ms) begin
            d  <= 8'b00000110;              // Entry Mode:cursor moves, display stands still
            count <= 0;
            state <= STATE17;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // Step 8 - Set the Entry Mode
    STATE17: begin
        if (int'(count) == D_50ns) begin
            e <= 1;
            count <= 0;
            state <= STATE18;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE18: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE19;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE19: begin
        if (int'(count) == D_60us) begin
            d  <= 8'b00001110;              // Display On + cursor On
            count <= 0;
            state <= STATE20;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // Step 9 - enter the Display On command
    STATE20: begin
        if (int'(count) == D_50ns) begin
            e <= 1;
            count <= 0;
            state <= STATE21;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE21: begin
        if (int'(count) == D_250ns) begin
            e <= 0;
            count <= 0;
            state <= STATE22;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    STATE22: begin
        if (int'(count) == D_60us) begin    // 60us
            busy_flag <= 0;                 // clear the busy flag
            count <= 0;
            state <= STATE23;
        end
        else begin
            count <= count + 24'b1;
        end
    end

    // End Initialization - Start entering data.

    STATE23: begin
        if (start) begin                    // wait for data
            if (count == 24'b0) begin       // if this is the first iteration of STATE23
                busy_flag <= 1;             // set the busy flag
                rs <= d_in[8];              // read the RS value from input
                d  <= d_in[7:0];            // read the data value input
                count <= count + 24'b1;     // increment the counter
            end
            else if (int'(count) == D_50ns) begin // if 50ns have elapsed
                count <= 0;                 // clear the counter
                state <= STATE24;           // advance to the next state
            end
            else begin                      // if it's not the first or last
                count <= count + 24'b1;     // increment the counter
            end
        end
    end

    STATE24: begin
        if (count == 24'b0) begin           // if this is the first iteration of STATE24
            e <= 1;                         // bring E high to initiate data write
            count <= count + 24'b1;         // increment the counter
        end
        else if (int'(count) == D_250ns) begin // if 250ns have elapsed
            count <= 0;                     // clear the counter
            state <= STATE25;               // advance to the next state
        end
        else begin                          // if it's not the first or last
            count <= count + 24'b1;         // increment the counter
        end
    end

    STATE25: begin
        if (count == 24'b0) begin           // if this is the first iteration of STATE25
            e <= 0;                         // bring E low
            count <= count + 24'b1;         // increment the counter
        end
        else if (int'(count) == D_40us && rs == 1) begin // if data is a character and 40us has elapsed
            start <= 0;                     // clear the start flag
            busy_flag <= 0;                 // clear the busy flag
            count <= 0;                     // clear the counter
            state <= STATE23;               // go back to STATE23 and wait for next data
        end
        else if (int'(count) == D_2ms && rs == 0) begin // if data is a command and 2ms has elapsed
            start <= 0;                     // clear the start flag
            busy_flag <= 0;                 // clear the busy flag
            count <= 0;                     // clear the counter
            state <= STATE23;               // go back to STATE23 and wait for next data
        end
        else begin                          // if it's not the first or last
            count <= count + 24'b1;         // increment the counter
        end
    end
    default: ;
    endcase
end

endmodule : lcd
