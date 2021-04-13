onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate /test_bench_top/BUTTON
add wave -noupdate /test_bench_top/RS
add wave -noupdate /test_bench_top/E
add wave -noupdate -radix ascii -radixshowbase 0 /test_bench_top/D
add wave -noupdate /test_bench_top/LED
add wave -noupdate -divider Top
add wave -noupdate /test_bench_top/top/internal_reset
add wave -noupdate /test_bench_top/top/last_signal
add wave -noupdate /test_bench_top/top/clean_signal
add wave -noupdate /test_bench_top/top/data_ready
add wave -noupdate /test_bench_top/top/lcd_busy
add wave -noupdate -divider Controller
add wave -noupdate /test_bench_top/top/controller/lcd_busy
add wave -noupdate /test_bench_top/top/controller/internal_reset
add wave -noupdate /test_bench_top/top/controller/rom_address
add wave -noupdate /test_bench_top/top/controller/data_ready
add wave -noupdate /test_bench_top/top/controller/current_lcd_state
add wave -noupdate /test_bench_top/top/controller/halt
add wave -noupdate -divider LCD
add wave -noupdate /test_bench_top/top/lcd/internal_reset
add wave -noupdate /test_bench_top/top/lcd/d_in
add wave -noupdate /test_bench_top/top/lcd/data_ready
add wave -noupdate /test_bench_top/top/lcd/rs
add wave -noupdate /test_bench_top/top/lcd/e
add wave -noupdate /test_bench_top/top/lcd/d
add wave -noupdate /test_bench_top/top/lcd/busy_flag
add wave -noupdate /test_bench_top/top/lcd/state
add wave -noupdate /test_bench_top/top/lcd/count
add wave -noupdate /test_bench_top/top/lcd/start
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {110264212500 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 252
configure wave -valuecolwidth 74
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ms
update
WaveRestoreZoom {103505873300 ps} {115800204700 ps}
