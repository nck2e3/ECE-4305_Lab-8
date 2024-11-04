transcript off
onbreak {quit -force}
onerror {quit -force}
transcript on

asim +access +r +m+xadc_fpro  -L xil_defaultlib -L unisims_ver -L unimacro_ver -L secureip -O2 xil_defaultlib.xadc_fpro xil_defaultlib.glbl

do {xadc_fpro.udo}

run

endsim

quit -force
