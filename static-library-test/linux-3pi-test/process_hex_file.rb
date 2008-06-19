#!/usr/bin/ruby
require 'hex_file'
hf = HexFile.new(File.open($*[0]))
print "What would you like to name this test? "
hf.name = STDIN.gets.chomp

hf.file_away
hf.report
`gnuplot plot_instructions`
