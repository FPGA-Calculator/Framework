//============================================================================
// ROM module
//============================================================================
`default_nettype none

module rom (
    input [3:0] rom_in, // Address input
    output reg [8:0] rom_out // Data output
);

always_comb begin
    case (rom_in)
        4'h0: rom_out = 9'b010000000; // Set address 0 (first char / first line)
        4'h1: rom_out = 9'b101001000;
        4'h2: rom_out = 9'b101100101;
        4'h3: rom_out = 9'b101101100;
        4'h4: rom_out = 9'b101101100;
        4'h5: rom_out = 9'b101101111;
        4'h6: rom_out = 9'b011000000; // Set address 40h (first char / second line)
        4'h7: rom_out = 9'b101010111;
        4'h8: rom_out = 9'b101101111;
        4'h9: rom_out = 9'b101110010;
        4'ha: rom_out = 9'b101101100;
        4'hb: rom_out = 9'b101100100;
        4'hc: rom_out = 9'b100100000;
        4'hd: rom_out = 9'b100100000;
        4'he: rom_out = 9'b010001111; // Leave cursor at the last char / first line
        4'hf: rom_out = 9'b100100000; // Need to write a char to make cursor visible
        default: rom_out = 9'hXXX;
    endcase
end

endmodule
