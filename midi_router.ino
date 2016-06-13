#include <LiquidCrystal.h>

#define SPACER "  "
#define BUTTON_PIN 7
#define LEFT_HALF_CURSOR (uint8_t)0
#define RIGHT_HALF_CURSOR (uint8_t)1
#define DISPLAY_WIDTH_IN_CHARS 16
#define DISPLAY_HEIGHT_IN_CHARS 2

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// custom chars generated from: https://omerk.github.io/lcdchargen/
byte leftDown[8] = {
  0b10000,
	0b10000,
	0b10000,
	0b11000,
	0b01100,
	0b00110,
	0b00011,
	0b00001
};

byte rightDown[8] = {
  0b00001,
	0b00001,
	0b00001,
	0b00011,
	0b00110,
	0b01100,
	0b11000,
	0b10000
};

int column = 1;
bool button_flag = TRUE;
bool button_value = TRUE;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.createChar(0, leftDown);
  lcd.createChar(1, rightDown);
  lcd.begin(DISPLAY_WIDTH_IN_CHARS, DISPLAY_HEIGHT_IN_CHARS);
  lcd.setCursor(column, 0);
  lcd.noAutoscroll();
  lcd.write(LEFT_HALF_CURSOR);
  lcd.write(RIGHT_HALF_CURSOR);
  Serial.begin(9600);
}

void loop() {
  button_value = digitalRead(BUTTON_PIN);
  if (!digitalRead(BUTTON_PIN) && !button_flag) {
    // delay to debounce the input
    delay(5);
    if (!digitalRead(BUTTON_PIN)){
      button_flag = TRUE;             // set the button flag to only change the cursor once per press
      lcd.setCursor(column,0);
      lcd.print(SPACER);              // erase the cursor
      column += 4;                    // jump to the next space for the cursor
      if (column >= 15) {             // if the cursor is as far right as it can be
        lcd.print(SPACER);
        column = 1;
      }
      lcd.setCursor(column,0);
      lcd.write(LEFT_HALF_CURSOR);    // display the cursor
      lcd.write(RIGHT_HALF_CURSOR);
    }
  }
  else if (digitalRead(BUTTON_PIN)) {
    button_flag = FALSE;
  }
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}

