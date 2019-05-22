//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif

uint8_t segments[8][8]  = {
  {
    B00111,
    B01111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  },

  {
    B11100,
    B11110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  },

  {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11110,
    B11100
  },

  {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B01111,
    B00111
  },

  {
    B11111,
    B11111,
    B11111,
    B00000,
    B00000,
    B00000,
    B00000
  },

  {
    B00000,
    B00000,
    B00000,
    B00000,
    B11111,
    B11111,
    B11111
  },

  {
    B11111,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111
  },

  {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B11111
  }
  
  
};

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  for (int i = 0; i< 8; i++) {
    lcd.createChar(i, segments[i]);
  }
  lcd.home(); 

  lcd.write(7);
 
  drawFactory(0, 0); 
}

void loop()
{
}

int zero[6] = {0, 4, 1, 3, 5, 2};

void drawFactory(int num, int x){
  switch(num){
    case 0:
      drawNum(zero, x); break;

    /*case 2:
      draw2(x); break;*/

  }
}

void drawNum(int segs[], int x){
  lcd.setCursor(x, 0);
  for (int i = 0; i < 6; i++) {
    lcd.write(segs[i]);
    if (i == 2) {
      lcd.setCursor(x, 1);
    }
  }
}
