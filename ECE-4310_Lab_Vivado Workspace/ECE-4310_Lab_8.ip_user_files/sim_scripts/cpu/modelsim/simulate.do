onbreak {quit -f}
onerror {quit -f}

vsim -voptargs="+acc"  -L xpm -L microblaze_v11_0_13 -L xil_defaultlib -L lib_cdc_v1_0_3 -L proc_sys_reset_v5_0_15 -L lmb_v10_v3_0_14 -L lmb_bram_if_cntlr_v4_0_24 -L blk_mem_gen_v8_4_8 -L axi_lite_ipif_v3_0_4 -L mdm_v3_2_26 -L xlconcat_v2_1_6 -L iomodule_v3_1_10 -L unisims_ver -L unimacro_ver -L secureip -lib xil_defaultlib xil_defaultlib.cpu xil_defaultlib.glbl

set NumericStdNoWarnings 1
set StdArithNoWarnings 1

do {wave.do}

view wave
view structure
view signals

do {cpu.udo}

run 1000ns

quit -force