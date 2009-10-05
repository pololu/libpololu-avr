#include <pololu/orangutan.h>

/*
 * svp1: for the Orangutan SVP.
 *
 * This example uses the OrangutanSVP functions to set the mode of the
 * auxiliary processor, take analog readings on line D/RX, and display
 * information about the Orangutan's current USB device state on the LCD.
 *
 * http://www.pololu.com/docs/0J20
 * http://www.pololu.com
 * http://forum.pololu.com
 */

int main()
{
  // Make SSbar be an output so it does not interfere with SPI communication.
  set_digital_output(IO_B4, LOW);

  // Set the mode to SVP_MODE_ANALOG so we can get analog readings on line D/RX.
  svp_set_mode(SVP_MODE_ANALOG);

  while(1)
  {
    clear(); // Erase the LCD.

    if (usb_configured())
    {
      // Connected to USB and the computer recognizes the device.
      print("USB");
    }
    else if (usb_power_present())
    {
      // Connected to USB.
      print("usb");
    }

    if (usb_suspend())
    {
      // Connected to USB, in the Suspend state.  
      lcd_goto_xy(4,0);
      print("SUS");
    }

    if (dtr_enabled())
    {
      // The DTR virtual handshaking line is 1.
      // This often means that a terminal program is conencted to the
      // Pololu Orangutan SVP USB Communication Port.
      lcd_goto_xy(8,0);
      print("DTR");
    }

    if (rts_enabled())
    {
      // The RTS virtual handshaking line is 1.
      lcd_goto_xy(12,0);
      print("RTS");
    }

    // Display an analog reading from channel D, in millivolts.
    lcd_goto_xy(0,1);
    print("Channel D: ");
    print_long(analog_read_millivolts(CHANNEL_D));

    // Wait for 100 ms, otherwise the LCD would flicker.
    delay_ms(100);
  }
}
