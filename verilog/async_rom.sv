//============================================================================
// (Asynchronous) ROM module
//============================================================================
// Warning 10030 can be ignored:
// https://www.intel.com/content/www/us/en/programmable/support/support-resources/knowledge-base/solutions/rd11032011_661.html
`default_nettype none

module async_rom
    #(parameter DATA_WIDTH = 9,
                ADDR_WIDTH = 4,
                ROM_FILE = "")  // ROM content read by $readmemb
(
    input  logic [ADDR_WIDTH-1:0] rom_in,  // Address input
    output logic [DATA_WIDTH-1:0] rom_out  // Data output
);

logic [DATA_WIDTH-1:0] rom_data [0:2**ADDR_WIDTH-1];

initial
    $readmemb(ROM_FILE, rom_data);

assign rom_out = rom_data[rom_in];

endmodule
