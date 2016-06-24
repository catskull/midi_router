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
#define MODE_CHANNEL 0
#define MODE_VOLUME 1
#define MODE_PITCH 2
#define MODE_VOID -1

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
int mapped = 0;
int oldVal = 0;
int mode = MODE_VOID;
int tempMode = MODE_VOID;
unsigned long time;

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.createChar(0, leftDown);
  lcd.createChar(1, rightDown);
  lcd.begin(DISPLAY_WIDTH_IN_CHARS, DISPLAY_HEIGHT_IN_CHARS);
  lcd.setCursor(column, TOP_ROW);
  lcd.noAutoscroll();
  lcd.write(LEFT_HALF_CURSOR);
  lcd.write(RIGHT_HALF_CURSOR);
  mapped = map(analogRead(0), 0, 1015, 1, 16);
  oldVal = mapped;
  
  // for (int i = 1; i < 5; i++) {
  //   incrementPort(i, 1);
  // }
  
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

void incrementPort(int port_number, int value) {
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
    eraseCursor();
    lcd.setCursor(col, BOTTOM_ROW);

    if (value) {
      lcd.print(value);
    }
}

void cycleCursor() {
  button_value = digitalReadDebounced(BUTTON_PIN);
  
  if (!button_value && !button_flag) {
    updateTime();
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
}

void changeChannel() {
  mapped = map(analogRead(0), 0, 1015, 0, 16);
  if (mapped != oldVal) {
    updateTime();
    if (column == PORT_1_COLUMN){
      incrementPort(1, mapped);
    }
    else if (column == PORT_2_COLUMN) {
      incrementPort(2, mapped);
    }
    else if (column == PORT_3_COLUMN) {
      incrementPort(3, mapped);
    }
    else if (column == PORT_4_COLUMN) {
      incrementPort(4, mapped);
    }
  }
  oldVal = mapped;
}

void selectMode() {
  mapped = map(analogRead(0), 0, 1015, 0, 2);
  if (mapped != oldVal) {
    lcd.setCursor(0,0);
    lcd.print("               ");
    switch (mapped) {
      case 0:
        tempMode = MODE_CHANNEL;
        lcd.setCursor(0,0);
        lcd.print("Channel");
        break;
      case 1:
        tempMode = MODE_VOLUME;
        lcd.setCursor(0,0);
        lcd.print("Volume");
        break;
      case 2:
        tempMode = MODE_PITCH;
        lcd.setCursor(0,0);
        lcd.print("Pitch Offset");
        break;
    }
  }
  oldVal = mapped;
}

void updateTime() {
  time = millis();
}

void checkTimeout() {
  if ((millis() - time) > 2000) {
    clearScreen();
    mode = MODE_VOID;
  }
}

void setMode() {
  button_value = digitalReadDebounced(BUTTON_PIN);
  
  if (!button_value && !button_flag) {
    mapped = oldVal;
    clearScreen();
    mode = tempMode;
  }
  else if (button_value) {
    button_flag = false;
  }
}

void clearScreen() {
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void loop() {
  if (mode == MODE_VOID) {
    selectMode();
    setMode();
  }
  else if (mode == MODE_CHANNEL) {
    cycleCursor();
    changeChannel();
    checkTimeout();
  }
}





