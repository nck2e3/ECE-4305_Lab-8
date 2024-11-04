vlib modelsim_lib/work
vlib modelsim_lib/msim

vlib modelsim_lib/msim/xil_defaultlib

vmap xil_defaultlib modelsim_lib/msim/xil_defaultlib

vlog -work xil_defaultlib -64 -incr -mfcu  \
"../../../../ECE-4310_Lab_8.gen/sources_1/ip/xadc_fpro/xadc_fpro.v" \


vlog -work xil_defaultlib \
"glbl.v"

