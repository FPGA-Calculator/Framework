puts {
    ModelSimSE general compile script version 1.1
    Copyright (c) Doulos June 2004, SD
}

# Simply change the project settings in this section for each new project.
# There should be no need to modify the rest of the script.

set library_file_list {
    design_library {
        ../verilog/async_rom.sv
        ../verilog/controller.sv
        ../verilog/debounce.sv
        ../verilog/lcd.sv
        ../verilog/top.sv
    }
    test_library {
        ../verilog/async_rom.sv
        ../verilog/controller.sv
        ../verilog/debounce.sv
        ../verilog/lcd.sv
        ../verilog/top.sv
        ../verilog/tb_top.sv
    }
}

# Copy project static resource files to the working area
file copy -force ../verilog/lcd_cmd.txt .

set top_level test_library.test_bench_top
set wave_patterns { /top/* }
set wave_radices { hexadecimal {data q} }

# After sourcing the script from ModelSim for the first time use these commands to recompile.

proc c  {} { uplevel #0 source compile.tcl }
proc cc {} { global last_compile_time
            set last_compile_time 0
            r }
proc r {} { restart -f ; run -all }
proc q {} { quit -force }

# Does this installation support Tk?
set tk_ok 1
if [catch {package require Tk}] { set tk_ok 0 }

# Prefer a fixed point font for the transcript
set PrefMain(font) { Courier 10 roman normal }

# Compile out of date files
set time_now [clock seconds]
if [catch {set last_compile_time}] {
    set last_compile_time 0
}
foreach {library file_list} $library_file_list {
    vlib $library
    vmap work $library
    foreach file $file_list {
        if { $last_compile_time < [file mtime $file] } {
            if [regexp {.vhdl?$} $file] {
                vcom -93 $file
            } else {
                vlog +acc $file
            }
            set last_compile_time 0
        }
    }
}
set last_compile_time $time_now

# Load the simulation
eval vsim $top_level

# If waves are required
if [llength $wave_patterns] {
    noview wave
    foreach pattern $wave_patterns {
        add wave $pattern
    }
    configure wave -signalnamewidth 1
    foreach {radix signals} $wave_radices {
        foreach signal $signals {
            catch {property wave -radix $radix $signal}
        }
    }
}

# Run the simulation
run -all

# If waves are required
if [llength $wave_patterns] {
    if $tk_ok {wave zoomfull}
}

puts {
  Script commands are:

  c = Recompile changed and dependent files
 cc = Recompile everything
  r = restart the simulation
  q = Quit without confirmation
}
