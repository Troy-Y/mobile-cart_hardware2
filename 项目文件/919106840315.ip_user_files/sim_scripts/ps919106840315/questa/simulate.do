onbreak {quit -f}
onerror {quit -f}

vsim -t 1ps -lib xil_defaultlib ps919106840315_opt

do {wave.do}

view wave
view structure
view signals

do {ps919106840315.udo}

run -all

quit -force
