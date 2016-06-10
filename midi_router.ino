#include <LiquidCrystal.h>

#define CHANNEL_SELECT_INDICATOR "\\/"
#define SPACER "  "
#define BUTTON_PIN 7
#define LEFT_HALF_CURSOR (uint8_t)0
#define RIGHT_HALF_CURSOR (uint8_t)1

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
bool button_flag = 1;
bool button_value = 1;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.createChar(0, leftDown);
  lcd.createChar(1, rightDown);
  lcd.begin(16, 2);
  lcd.setCursor(column, 0);
  lcd.noAutoscroll();
  // lcd.print(CHANNEL_SELECT_INDICATOR);
  lcd.write(LEFT_HALF_CURSOR);
  lcd.write(RIGHT_HALF_CURSOR);
  Serial.begin(9600);
}

void loop() {
  button_value = digitalRead(BUTTON_PIN);
  if (!button_value && !button_flag) {
    button_flag = 1;
    lcd.setCursor(column,0);
    lcd.print(SPACER);
    column += 4;
    lcd.setCursor(column,0);
    // lcd.print(CHANNEL_SELECT_INDICATOR);
    lcd.write(LEFT_HALF_CURSOR);
    lcd.write(RIGHT_HALF_CURSOR);
    if (column >= 15) {
      lcd.print(SPACER);
      column = 1;
      lcd.setCursor(column, 0);
      // lcd.print(CHANNEL_SELECT_INDICATOR);
      lcd.write(LEFT_HALF_CURSOR);
      lcd.write(RIGHT_HALF_CURSOR);
    }
  }
  else if (button_value) {
    button_flag = 0;
  }
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
}

