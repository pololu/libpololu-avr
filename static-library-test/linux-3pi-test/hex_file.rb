class HexFile
  # This power usage assumes that the power draw of the 3pi is relatively constant,
  # the reason for this is because of the step-up regulator inside of it.
  PowerUsage = 0.24*5000 #in milliWatts

  attr_accessor :hex, :milli_voltages, :amps, :amp_hours, :name

  def initialize(io, name="temp")
    puts name
    @name = name
    @milli_voltages = []
    @amps = []
    @amp_hours = []
    @hex = ""

    while (next_line = io.gets) do
      @hex << next_line
    end

    hex_array = hex.split(",")
    hex_array = hex_array[0,9999]

    i = 0
    while i < hex_array.size do
      milliVolts = eval("0x#{number_portion(hex_array[i+1]) + number_portion(hex_array[i])}")
      i += 2 # consume two hex_array numbers

      break if milliVolts == 0xFFFF # max value of EEProm probably not the voltage

      @milli_voltages << milliVolts

      amps = PowerUsage / milliVolts
      @amps << amps

      amp_hours = @amp_hours.last || 0

      milliAmpHours = (amps * 1000 / 60)
      @amp_hours << amp_hours + milliAmpHours
    end
  end

  def file_away
    File.open("dumps/#{@name}.hex", "w") { |f| f.write(@hex) }
    File.open("datas/#{@name}.data", "w") { |f| f.write(to_plot_data) }
  end

  def minutes_lasted
    @milli_voltages.size
  end

  def to_plot_data
    string = ""
    @milli_voltages.each_with_index do |v,minute|
      string << "#{minute} #{milli_voltages[minute]} #{amps[minute]} #{amp_hours[minute]}\n"
    end
    string
  end

  def report
    puts "Name: #{@name}"
    #puts "Minute   milliVolts   amps   amp-hours"
    #@milli_voltages.each_with_index do |v,minute|
    #  puts "#{minute} #{milli_voltages[minute]} #{amps[minute]} #{amp_hours[minute]}"
    #end
    puts "Total Minutes: #{minutes_lasted}"
    puts "milliAmpHours: #{amp_hours.last}"
  end

  private
  def number_portion(hex_string)
    number = hex_string.split("x")[1]
    if number.size == 1
      "0"+number
    else
      number
    end
  end

end
