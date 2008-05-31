#include <PololuQTRRC.h>
#include <Pololu3pi.h>
#include <OrangutanPushbuttons.h>
#include <OrangutanMotors.h>
#include <OrangutanLEDs.h>
#include <OrangutanLCD.h>
#include <OrangutanBuzzer.h>
#include <OrangutanAnalog.h>
#include <avr/pgmspace.h>

OrangutanLCD lcd;
Pololu3pi robot;
OrangutanPushbuttons buttons;
OrangutanMotors motors;
OrangutanLEDs leds;
OrangutanBuzzer buzzer;
OrangutanAnalog analog;

unsigned char __attribute__ ((progmem)) LcdCustomChar[] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0. 0/8 full progress block
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, // 1. 1/8 full progress block
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, // 2. 2/8 full progress block
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, // 3. 3/8 full progress block
  0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, // 4. 4/8 full progress block
  0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 5. 5/8 full progress block
  0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 6. 6/8 full progress block
  0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, // 7. 7/8 full progress block
};

#define LCD_CGRAM   6


void LCDLoadCustomChar(unsigned char* lcdCustomCharArray, unsigned char romCharNum, unsigned char lcdCharNum)
{
  register unsigned char i;

  // multiply the character index by 8
  lcdCharNum = (lcdCharNum<<3);	// each character occupies 8 bytes
  romCharNum = (romCharNum<<3);	// each character occupies 8 bytes

  // copy the 8 bytes into CG (character generator) RAM

  for(i=0; i<8; i++)
  {
    // set CG RAM address
    lcd.send((1<<LCD_CGRAM) | (lcdCharNum+i), 0);
    // write character data
    lcd.send( pgm_read_byte(lcdCustomCharArray+romCharNum+i), 1 );
  }
}

void display_readings(const unsigned int *calibrated_values)
{
  unsigned char i;

  for(i=0;i<5;i++) {
    unsigned char c = calibrated_values[i]/142;
    lcd.send(c, 1);
  }
}


void setup()
{
  lcd.clear();
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,0,0);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,1,1);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,2,2);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,3,3);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,4,4);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,5,5);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,6,6);
  LCDLoadCustomChar((unsigned char*)LcdCustomChar,7,7);
  lcd.clear();
}


void loop()
{
  unsigned int counter;

  robot.init(2000);

  // display temperature and wait for button press

  while(!buttons.isPressed(BUTTON_B))
  {
    int bat = robot.batteryMillivolts();

    lcd.clear();
    lcd.print(bat);
    lcd.print("mV");
    lcd.gotoXY(0,1);
    lcd.print("Press B");

    delay(50);
  }

  buttons.waitForRelease(BUTTON_B);
  delay(500);

  // auto-calibration

  for(counter=0;counter<80;counter++)
  {
    if(counter < 20 || counter >= 60)
      motors.setSpeeds(40,-40);
    else
      motors.setSpeeds(-40,40);

    lcd.clear();
    lcd.gotoXY(0,1);
    lcd.print("Press B");

    robot.calibrateLineSensors();

    delay(20);
  }
  motors.setSpeeds(0,0);

  // display calibrated values

  while(!buttons.isPressed(BUTTON_B))
  {
    unsigned int sensors[5] = {
      1,2,3,4,5    };
    unsigned int position = robot.readLine(sensors);

    lcd.clear();
    lcd.print(position);
    lcd.gotoXY(0,1);
    display_readings(sensors);

    delay(50);
  }
  buttons.waitForRelease(BUTTON_B);

  lcd.clear();

  buzzer.play("L16 cdegreg4");

  lcd.print("Go!");		

  while(buzzer.isPlaying());

  while(1)
  {
    unsigned int sensors[5] = {
      1,2,3,4,5    };
    unsigned int position = robot.readLine(sensors, 0);

    if(position < 2000)
      motors.setSpeeds(0,100);
    else
      motors.setSpeeds(100,0);
  }

}
