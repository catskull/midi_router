#include <LiquidCrystal.h>

#define SPACER "  "
#define BUTTON_PIN 7
#define LEFT_HALF_CURSOR (uint8_t)0
#define RIGHT_HALF_CURSOR (uint8_t)1
#define DISPLAY_WIDTH_IN_CHARS 16
#define DISPLAY_HEIGHT_IN_CHARS 2
#define TOP_ROW 0
#define BOTTOM_ROW 1
#define PORT_1_COLUMN 1
#define PORT_2_COLUMN (PORT_1_COLUMN + COLUMN_SPACE)
#define PORT_3_COLUMN (PORT_2_COLUMN + COLUMN_SPACE)
#define PORT_4_COLUMN (PORT_3_COLUMN + COLUMN_SPACE)
#define COLUMN_SPACE 4
#define MIN_CHANNEL 1
#define MAX_CHANNEL 16

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// custom chars generated from: https://omerk.github.io/lcdchargen/
byte leftDown[8] = {
  0b00001,
	0b00001,
	0b00001,
	0b00001,
	0b01001,
	0b00101,
	0b00011,
	0b00001
};

byte rightDown[8] = {
  0b10000,
	0b10000,
	0b10000,
	0b10000,
	0b10010,
	0b10100,
	0b11000,
	0b10000
};

int column = 1;
bool button_flag = true;
bool button_value = true;
int counter = 0;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.createChar(0, leftDown);
  lcd.createChar(1, rightDown);
  lcd.begin(DISPLAY_WIDTH_IN_CHARS, DISPLAY_HEIGHT_IN_CHARS);
  lcd.setCursor(column, TOP_ROW);
  lcd.noAutoscroll();
  lcd.write(LEFT_HALF_CURSOR);
  lcd.write(RIGHT_HALF_CURSOR);
  Serial.begin(9600);
}

bool digitalReadDebounced(int pin) {
  if (digitalRead(pin)) {
    delay(5);
    if (digitalRead(pin)) {
      return true;
    }
  }
  return false;
}

void drawCursor() {
  lcd.write(LEFT_HALF_CURSOR);    // display the cursor
  lcd.write(RIGHT_HALF_CURSOR);
}

void eraseCursor() {
  lcd.print(SPACER);              // erase the cursor
}

void incrementPort(int port_number) {
  int col = 0;
  switch (port_number) {
    case 1:
      col = PORT_1_COLUMN;
      break;
    case 2:
      col = PORT_2_COLUMN;
      break;
    case 3:
      col = PORT_3_COLUMN;
      break;
    default:
      col = PORT_4_COLUMN;
      break;
    }
    
    lcd.setCursor(col, BOTTOM_ROW);
    if (counter > MAX_CHANNEL) {
      eraseCursor();
      lcd.setCursor(col, BOTTOM_ROW);
      counter = MIN_CHANNEL;
    }
    
    lcd.print(counter);
      
}

void loop() {
  button_value = digitalReadDebounced(BUTTON_PIN);
  
  if (!button_value && !button_flag) {
    counter++;
    button_flag = true;             // set the button flag to only change the cursor once per press
    lcd.setCursor(column, TOP_ROW);
    eraseCursor();
    column += COLUMN_SPACE;                    // jump to the next space for the cursor
    if (column >= DISPLAY_WIDTH_IN_CHARS) {             // if the cursor is as far right as it can be
      eraseCursor();
      column = 1;                   // reset the column to the first position
    }
    lcd.setCursor(column, TOP_ROW);
    drawCursor();
  }
  else if (button_value) {
    button_flag = false;
  }
  
  // incrementPort(1);
  // incrementPort(2);
  incrementPort(3);
  // incrementPort(4);

}



