//============================================================================
// Debounce module
//============================================================================
`default_nettype none

module debounce (
    input wire clk,
    input wire in,
    output logic out
);

logic delta;
logic [19:0] timer = 0;

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
