#!/usr/bin/ruby
require 'tempfile'
require 'hex_file'

Dir["dumps/*.hex"].each do |hex_file_path|
  hf = HexFile.new(File.open(hex_file_path), File.basename(hex_file_path,'.hex'))
  hf.file_away
  hf.report
end

def gnu_list(list, opts="")
  list.collect { |path| "'#{path}'" }.join(" #{opts}, ") + opts
end

list = []
Dir["datas/*.data"].each do |hex_file_path|
  list << hex_file_path
end

instructions = "
set terminal png
set output 'plots/voltage-vs-time.png'

set title 'Voltage vs Time'
set xlabel 'Time (minutes)'
set ylabel 'Voltage (milivolts)'

plot #{gnu_list(list, "using 1:2 with lines")}

set output 'plots/current-vs-time.png'

set title 'Current vs Time'
set xlabel 'Time (minutes)'
set ylabel 'Current (amps)'

plot #{gnu_list(list, "using 1:3 with lines")}

set output 'plots/amphours-vs-time.png'

set title 'Amp Hours vs Time'
set xlabel 'Time (minutes)'
set ylabel 'milliAmp-hours'

plot #{gnu_list(list, "using 1:4 with lines")}
"

t = Tempfile.new("gnuplot")
t << instructions
t.flush

`cat #{t.path} | gnuplot`
