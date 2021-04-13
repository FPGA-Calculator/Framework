//============================================================================
// Debounce module
//============================================================================
`default_nettype none

module debounce (
    input clk,
    input in,
    output reg out
);

wire delta;
reg [19:0] timer;

initial timer = 0;
initial out = 0;

assign delta = in ^ out;

always_ff @(posedge clk) begin
    if (timer[19]) begin
        out <= in;
        timer <= 0;
    end
    else if (delta) begin
        timer <= timer + 20'b1;
    end
    else begin
        timer <= 0;
    end
end

endmodule : debounce
