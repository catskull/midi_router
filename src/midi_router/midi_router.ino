// Could possibly use classes/structures to encapsulate certain properties
// cursor, column, row, screen, etc.

#include <LiquidCrystal.h>
#include <EEPROM.h>

//Function prototypes
void setup();
void loop();
void selectMode();
void setMode(int);
void modeSetChannel();
void cycleCursor();
void changeChannel();
void checkTimeout();
bool digitalReadDebounced(int);
void drawCursor();
void eraseCursor();
void incrementPort(int, int);
void updateTime();
void clearScreen();


#define SPACER "  "
#define BUTTON_PIN 4
#define LEFT_HALF_CURSOR (uint8_t)0
#define RIGHT_HALF_CURSOR (uint8_t)1
#define CATSKULL_ICON (uint8_t)2
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

LiquidCrystal lcd(12, 11, 6, 5, 3, 2);

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

byte catskull[8] = {
	0b10001,
	0b11011,
	0b11111,
	0b10001,
	0b11011,
	0b10001,
	0b10101,
	0b01110
};

int column = 1;
//******************************//
// What is the difference between button flag and value?
bool button_flag = true;
bool button_value = true;
//******************************//
int counter = 0;
int mapped = 0;
int oldVal = 0;
int mode = MODE_VOID;
unsigned long time;
int incomingMIDI = 0;

// store the midi channel offsets in eprom
// byte channelOffset[] PROGMEM = {0, 0, 0, 0};

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lcd.createChar(0, leftDown);
  lcd.createChar(1, rightDown);
  lcd.createChar(2, catskull);
  lcd.begin(DISPLAY_WIDTH_IN_CHARS, DISPLAY_HEIGHT_IN_CHARS);
  lcd.setCursor(column, TOP_ROW);
  lcd.noAutoscroll();
  oldVal = map(analogRead(0), 0, 1015, 1, 16);

  Serial.begin(31250);
}

void loop() {
  incomingMIDI = Serial.read();
  lcd.setCursor(0,0);
  lcd.print("waiting...");
  if (incomingMIDI >= 0) {
    clearScreen();
    lcd.setCursor(0,0);
    lcd.print(incomingMIDI);
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(10);
  }
  // switch (mode) {
  //   case MODE_VOID:
  //     selectMode();
  //     break;
  //   case MODE_CHANNEL:
  //     modeSetChannel();
  //     break;
  //     
  //   default:
  //     selectMode();
  // }
}

void selectMode() {
  int tempMode = 0;
  mapped = map(analogRead(0), 0, 1000, 0, 2);
  if (mapped != oldVal) {
    lcd.setCursor(0,0);
    clearScreen();
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
    oldVal = mapped;
  }
  setMode(tempMode);
}

void setMode(int tempMode) {
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

void modeSetChannel() {
  modeSetChannelSetup();
  while (mode == MODE_CHANNEL) {
    cycleCursor();
    changeChannel();
    checkTimeout();
  }
}

void modeSetChannelSetup() {
  // read the value of the pot so we don't immediately overwrite the channel
  // I probably should use a rotary encoder instead of a pot.
  mapped = map(analogRead(0), 0, 1015, 0, 16);
  oldVal = mapped;
  column = -3;
  for (int i = 0; i < 4; i++) {
    // TODO: Clear the eeprom data on firt boot somehow
    if(EEPROM.read(i) > -1) {
      incrementPort(i+1, EEPROM.read(i));
    }
  }
}

void cycleCursor() {
  button_value = digitalReadDebounced(BUTTON_PIN);

  if (!button_value && !button_flag) {
    updateTime();
    counter++;
    button_flag = true;                       // set the button flag to only change the cursor once per press
    lcd.setCursor(column, TOP_ROW);
    eraseCursor();
    column += COLUMN_SPACE;                   // jump to the next space for the cursor
    if (column >= DISPLAY_WIDTH_IN_CHARS) {   // if the cursor is as far right as it can be
      eraseCursor();
      column = 1;                             // reset the column to the first position
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

void checkTimeout() {
  if ((millis() - time) > 2000) {
    clearScreen();
    mode = MODE_VOID;
    column = 0;
    lcd.setCursor(0,0);
    lcd.write("Channel");
  }
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
  lcd.write(LEFT_HALF_CURSOR);
  lcd.write(RIGHT_HALF_CURSOR);
}

void eraseCursor() {
  lcd.print(SPACER);
}

void incrementPort(int port_number, int value) {
  int col = 0;
  int index = 0;
  switch (port_number) {
    case 1:
      col = PORT_1_COLUMN;
      index = 0;
      break;
    case 2:
      col = PORT_2_COLUMN;
      index = 1;
      break;
    case 3:
      col = PORT_3_COLUMN;
      index = 2;
      break;
    default:
      col = PORT_4_COLUMN;
      index = 3;
      break;
    }

    lcd.setCursor(col, BOTTOM_ROW);
    eraseCursor();
    lcd.setCursor(col, BOTTOM_ROW);
    
    if (value) {
      lcd.print(value);
    }
    else {
      lcd.print("NC");
    }
    EEPROM.write(0 + index, value);
}

void updateTime() {
  time = millis();
}

void clearScreen() {
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}
