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
    B00000,
    B00000
  },

  {
    B00000,
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
    B00000,
    B11111
  },

  {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111
  }
};

LiquidCrystal_I2C lcd1(0x27, 16, 2);  
LiquidCrystal_I2C lcd2(0x3F, 16, 2);  
LiquidCrystal_I2C *curr;

bool first = true;

int minutes[2] = {0, 0};
int seconds[2] = {0, 0};


int digits[2][5] = { // digits of first and second timer
                      {0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0}
                   };
                   
int pos[5] = {0, 3, 6, 10, 13};

void configureTimers() {
  int index = 0;

  curr = &lcd1;

  bool digitOn = true;
  unsigned long long prevTime = millis();
  int currTime;

  bool first = false;
  bool cyclePressed = false;
  
  while (true) {
    while (!cyclePressed) {
      if (digitalRead(4) == HIGH) {
        return;
      }
      
      if (cycleButtonPressed()) {
        cyclePressed = true;  
      }
      
      currTime = millis();
      if (digitOn) {
        if (currTime - prevTime >= 600) {
          prevTime = currTime;
          clearSquare(*curr, pos[index]);      
          digitOn = !digitOn;
        } 
      } else {
        if (currTime - prevTime >= 300) {
          prevTime = currTime;
          drawFactory(*curr, digits[first][index], pos[index]);      
          digitOn = !digitOn;
        }
      }

      if (incrementButtonPressed()) {
        ++digits[first][index] %= (index == 3 ? 6 : 10);
      }
    }

    while (cycleButtonPressed()) {} // waiting for players to release the button 
    
    cyclePressed = false;
    
    drawFactory(*curr, digits[first][index], pos[index]);      
      
    if (++index == 4) {
      curr = first ? &lcd1 : &lcd2;
      first = !first;
      index = 0;  
    } 
  }
}

bool prevIncrementState = 0;
bool incrementButtonPressed() {
  delay(30);
  if (prevIncrementState == digitalRead(2)) {
    return false;
  }
  
  prevIncrementState = digitalRead(2);
  return prevIncrementState;
}

bool cycleButtonPressed() {
  return digitalRead(3) == HIGH;  
}

bool stopButtonPressed() {
  return digitalRead(4) == HIGH;  
}

unsigned long long prevTime;
void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  pinMode(12, OUTPUT);
  
  lcd1.init();     
  lcd2.init();                    
  
  lcd1.backlight();                     
  lcd2.backlight();
  
  for (int i = 0; i < 8; i++) {
    lcd1.createChar(i, segments[i]);
    lcd2.createChar(i, segments[i]);
  }
  
  lcd1.home(); 
  lcd2.home(); 

  curr = &lcd1;
  for (int i = 0; i < 5; i++) {
    drawFactory(*curr, digits[0][i], pos[i]);     
  }
  drawTwoDots(*curr);
  
  curr = &lcd2;
  for (int i = 0; i < 5; i++) {
    drawFactory(*curr, digits[0][i], pos[i]);     
  }
  drawTwoDots(*curr);
    
  configureTimers();
  printTime(lcd1, digits[0]);
  printTime(lcd2, digits[1]);
  
  while (digitalRead(3) != HIGH) {}
  
  prevTime = millis();
}

bool whiteTurn = true;

void buzz() {
  tone(12, 1000);
  delay(500);
  noTone(12);
}

void loop() {
  printTime(lcd1, digits[0]);
  printTime(lcd2, digits[1]);
  
  lcd1.print(millis());
  lcd1.setCursor(0, 0);
  if (millis() - prevTime >= 1000) {
    prevTime = millis();
    if (!decrementTime(digits[whiteTurn])) {
//      buzz(); 
      char *msg;
      if (whiteTurn) {
          msg = "White wins\n on time!";
      } else {
          msg = "Black wins\n on time!";  
      }
      
      lcd1.setCursor(0, 0);
      lcd1.print(msg);
      lcd2.setCursor(0, 0);
      lcd2.print(msg);
      delay(5000);
      exit(0);
    }
  }

  if (digitalRead(2)) {
    whiteTurn = false;  
  }
  if (digitalRead(4)) {
    whiteTurn = true;  
  }
}

void printTime(LiquidCrystal_I2C lcd, int* digits) {
  for (int i = 0; i < 5; i++) {
    if (i == 0 && digits[0] == 0) {
      clearSquare(lcd, pos[0]);  
      continue;
    }

    if (i == 1 && digits[0] == 0 && digits[1] == 0) {
      clearSquare(lcd, pos[1]);
      continue;  
    }
    drawFactory(lcd, digits[i], pos[i]); 
  }  
}

int decrementTime(int *digits) {
  int minutes = 0;
  int seconds = 0;
  
  seconds = digits[3] * 10 + digits[4];
  minutes = digits[0] * 100 + digits[1] * 10 + digits[2];

  if (minutes == 0 && seconds == 0) {
    return 0;  
  }

  seconds--;
  if (seconds < 0) {
    seconds = 59;
    minutes--;
  }

  digits[0] = (minutes % 1000) / 100;
  digits[1] = (minutes % 100) / 10;
  digits[2] = (minutes % 10) / 1;

  digits[3] = seconds / 10;
  digits[4] = seconds % 10;
  
  return 1;
}

void drawFactory(LiquidCrystal_I2C lcd, int num, int x) {

  int zero[6]  = {0, 4, 1, 3, 5, 2};
  int one[6]   = {4, 1, -1, 5, 7, 5};
  int two[6]   = {4, 6, 1, 0, 6, 5};
  int three[6] = {4, 6, 1, 5, 6, 2};
  int four[6]  = {3, 5, 7, -1, -1, 7};
  int five[6]  = {0, 6, 4, 5, 6, 2};
  int six[6]   = {0, 6, 4, 3, 6, 2};
  int seven[6] = {4, 4, 2, -1, 0, -1};
  int eight[6] = {0, 6, 1, 3, 6, 2};
  int nine[6]  = {0, 6, 1, 5, 6, 2};
  
  switch(num){
    case 0: drawNum(lcd, zero, x); break;
    case 1: drawNum(lcd, one, x); break;
    case 2: drawNum(lcd, two, x); break;
    case 3: drawNum(lcd, three, x); break;
    case 4: drawNum(lcd, four, x); break;
    case 5: drawNum(lcd, five, x); break;
    case 6: drawNum(lcd, six, x); break;
    case 7: drawNum(lcd, seven, x); break;
    case 8: drawNum(lcd, eight, x); break;
    case 9: drawNum(lcd, nine, x); break;
  }
}

void drawTwoDots(LiquidCrystal_I2C lcd) {
  lcd.setCursor(9, 0);
  lcd.write(5);
  lcd.setCursor(9, 1);
  lcd.write(5);  
}

void clearSquare(LiquidCrystal_I2C lcd, int x) {
  lcd.setCursor(x, 0);
  for (int i = 0; i < 6; i++) {
    lcd.print(" ");
    if (i == 2) {
      lcd.setCursor(x, 1);
    } 
  }  
}
void drawNum(LiquidCrystal_I2C lcd, int segs[], int x){
  lcd.setCursor(x, 0);
  for (int i = 0; i < 6; i++) {
    if (segs[i] == -1) {
      lcd.print(" ");
    } else {
      lcd.write(segs[i]);
    }
    if (i == 2) {
      lcd.setCursor(x, 1);
    }
  }
}
