#include <Keyboard.h>
#include <Mouse.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 128x64 oled on Wire1, no reset pin wired
Adafruit_SSD1306 display(128, 64, &Wire1, -1);

// 4x4 matrix, rows drive and cols read
const int ROW_NUM = 4;
const int COL_NUM = 4;
const int rowPins[ROW_NUM] = {4, 5, 6, 7};
const int colPins[COL_NUM] = {0, 1, 2, 3};

// rotary encoder, A and B for turning plus the push switch
#define ENC_A   10
#define ENC_B   11
#define ENC_SW   8

// made up codes for the shortcut keys that arent normal ascii
#define K_EMPTY   0x00
#define K_ALL     0xEE
#define K_UNDO    0xEF
#define K_REDO    0xF0
#define K_COPY    0xF1
#define K_PASTE   0xF2
#define K_SAVE    0xF3
#define K_FIND    0xF4
#define K_BOLD    0xF5
#define K_ITAL    0xF6
#define K_UNDR    0xF7
#define K_CMNT    0xF8
#define K_DUPL    0xF9
#define K_INDL    0xFA
#define K_INDR    0xFB
#define K_ENT     0xFC

#define LAYER_NUMPAD    0
#define LAYER_SHORTCUTS 1
#define LAYER_COUNT     2

const uint8_t keymap[LAYER_COUNT][ROW_NUM][COL_NUM] = {
  // numpad
  {
    { '1',   '2',   '3',   '/'   },
    { '4',   '5',   '6',   '*'   },
    { '7',   '8',   '9',   '-'   },
    { '0',   '.',   K_ENT, '+'   },
  },
  // shortcuts
  {
    { K_UNDO,  K_REDO,  K_COPY,  K_PASTE },
    { K_SAVE,  K_FIND,  K_ALL,   K_EMPTY },
    { K_BOLD,  K_ITAL,  K_UNDR,  K_EMPTY },
    { K_CMNT,  K_DUPL,  K_INDL,  K_INDR  },
  },
};

const char* layerName[LAYER_COUNT] = { "NUMPAD", "SHORTCUTS" };

int  currentLayer = LAYER_NUMPAD;

bool          keyState[ROW_NUM][COL_NUM];
bool          lastRaw[ROW_NUM][COL_NUM];
unsigned long debounceTimers[ROW_NUM][COL_NUM];
uint8_t       pressedKey[ROW_NUM][COL_NUM];
const unsigned long DEBOUNCE_MS = 20;

int           encLastA      = HIGH;
bool          encSwState    = false;
bool          encSwLast     = HIGH;
unsigned long encSwDebounce = 0;

bool          menuActive   = false;
int           menuSelected = 0;
unsigned long menuTimeout  = 0;
const unsigned long MENU_HIDE_MS = 3000;

char lastKeyStr[16] = "---";

// the normal screen: current layer up top, last key big underneath
void drawMainScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("LAYER: ");
  display.print(layerName[currentLayer]);
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print("LAST KEY:");

  // center it, roughly 18px per char at size 3
  display.setTextSize(3);
  int len  = strlen(lastKeyStr);
  int xPos = max(0, (128 - len * 18) / 2);
  display.setCursor(xPos, 30);
  display.print(lastKeyStr);

  display.display();
}

// the layer picker: highlights the selection, stars the active layer
void drawMenu() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(35, 0);
  display.print("SELECT LAYER");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  for (int i = 0; i < LAYER_COUNT; i++) {
    int y = 16 + i * 20;

    // draw the highlighted row as white with black text
    if (i == menuSelected) {
      display.fillRoundRect(2, y - 2, 124, 18, 3, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setTextSize(1);
    display.setCursor(10, y + 4);
    if (i == currentLayer) display.print("* ");
    else display.print("  ");
    display.print(layerName[i]);
  }

  display.drawLine(0, 54, 127, 54, SSD1306_WHITE);
  display.display();
}

// fire the ctrl combo behind each shortcut code
void pressSpecial(uint8_t key) {
  switch (key) {
    case K_UNDO:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('z'); Keyboard.releaseAll(); break;
    case K_REDO:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('y'); Keyboard.releaseAll(); break;
    case K_COPY:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('c'); Keyboard.releaseAll(); break;
    case K_PASTE: Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('v'); Keyboard.releaseAll(); break;
    case K_SAVE:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('s'); Keyboard.releaseAll(); break;
    case K_FIND:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('f'); Keyboard.releaseAll(); break;
    case K_ALL:   Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('a'); Keyboard.releaseAll(); break;
    case K_BOLD:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('b'); Keyboard.releaseAll(); break;
    case K_ITAL:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('i'); Keyboard.releaseAll(); break;
    case K_UNDR:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('u'); Keyboard.releaseAll(); break;
    case K_CMNT:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('/'); Keyboard.releaseAll(); break;
    case K_DUPL:  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press('d'); Keyboard.releaseAll(); break;
    case K_INDL:  Keyboard.press(KEY_LEFT_SHIFT); Keyboard.press(KEY_TAB); Keyboard.releaseAll(); break;
    case K_INDR:  Keyboard.press(KEY_TAB); Keyboard.releaseAll(); break;
    case K_ENT:   Keyboard.press(KEY_RETURN); Keyboard.releaseAll(); break;
  }
}

const char* getKeyLabel(uint8_t key) {
  switch (key) {
    case K_UNDO:  return "UNDO";
    case K_REDO:  return "REDO";
    case K_COPY:  return "COPY";
    case K_PASTE: return "PASTE";
    case K_SAVE:  return "SAVE";
    case K_FIND:  return "FIND";
    case K_ALL:   return "ALL";
    case K_BOLD:  return "BOLD";
    case K_ITAL:  return "ITAL";
    case K_UNDR:  return "UNDR";
    case K_CMNT:  return "CMNT";
    case K_DUPL:  return "DUPL";
    case K_INDL:  return "IND<";
    case K_INDR:  return "IND>";
    case K_ENT:   return "ENT";
    default:      return "KEY";
  }
}

void onKeyPress(int r, int c) {
  uint8_t key      = keymap[currentLayer][r][c];
  pressedKey[r][c] = key;
  if (key == K_EMPTY) return;

  if (currentLayer == LAYER_NUMPAD) {
    if (key == K_ENT) {
      Keyboard.press(KEY_RETURN);
      strncpy(lastKeyStr, "ENT", sizeof(lastKeyStr) - 1);
    } else {
      Keyboard.press((char)key);
      char buf[4] = {(char)key, 0};
      strncpy(lastKeyStr, buf, sizeof(lastKeyStr) - 1);
    }
  } else {
    pressSpecial(key);
    strncpy(lastKeyStr, getKeyLabel(key), sizeof(lastKeyStr) - 1);
  }

  if (!menuActive) drawMainScreen();
}

void onKeyRelease(int r, int c) {
  uint8_t key      = pressedKey[r][c];
  pressedKey[r][c] = K_EMPTY;
  if (key == K_EMPTY) return;

  // only the numpad holds keys down; shortcuts already let go in pressSpecial
  if (currentLayer == LAYER_NUMPAD) {
    if (key == K_ENT) Keyboard.release(KEY_RETURN);
    else Keyboard.release((char)key);
  }
}

// turning the knob opens the layer menu and moves the highlight
void handleEncoder() {
  int a = digitalRead(ENC_A);

  if (a != encLastA && a == LOW) {
    int b = digitalRead(ENC_B);

    if (!menuActive) {
      menuActive   = true;
      menuSelected = currentLayer;
    }
    menuTimeout = millis();

    if (b == HIGH) {
      menuSelected = (menuSelected + 1) % LAYER_COUNT;
    } else {
      menuSelected = (menuSelected - 1 + LAYER_COUNT) % LAYER_COUNT;
    }

    drawMenu();
  }
  encLastA = a;
}

// pressing the knob opens the menu, or confirms the pick if its already open
void handleEncoderSwitch() {
  bool sw = (digitalRead(ENC_SW) == LOW);

  if (sw != encSwLast) {
    encSwDebounce = millis();
    encSwLast     = sw;
  }

  if ((millis() - encSwDebounce) > DEBOUNCE_MS && sw != encSwState) {
    encSwState = sw;
    if (sw) {
      if (menuActive) {
        currentLayer = menuSelected;
        menuActive   = false;
        strncpy(lastKeyStr, "---", sizeof(lastKeyStr) - 1);
        drawMainScreen();
      } else {
        menuActive   = true;
        menuSelected = currentLayer;
        menuTimeout  = millis();
        drawMenu();
      }
    }
  }
}

void setup() {
  for (int r = 0; r < ROW_NUM; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }
  for (int c = 0; c < COL_NUM; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  pinMode(ENC_A,  INPUT_PULLUP);
  pinMode(ENC_B,  INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  encLastA = digitalRead(ENC_A);

  memset(keyState,       false, sizeof(keyState));
  memset(lastRaw,        false, sizeof(lastRaw));
  memset(debounceTimers, 0,     sizeof(debounceTimers));
  memset(pressedKey,     0,     sizeof(pressedKey));

  Keyboard.begin();
  Mouse.begin();

  // oled on Wire1 so it stays off the matrix pins
  Wire1.setSDA(14);
  Wire1.setSCL(15);
  Wire1.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  drawMainScreen();
}

void loop() {
  unsigned long now = millis();

  // scan one row at a time
  for (int r = 0; r < ROW_NUM; r++) {
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(10);          // give the line a sec to settle

    for (int c = 0; c < COL_NUM; c++) {
      bool raw = (digitalRead(colPins[c]) == LOW);

      // reset the timer on every wobble, only act once its held steady
      if (raw != lastRaw[r][c]) {
        debounceTimers[r][c] = now;
        lastRaw[r][c]        = raw;
      }

      if ((now - debounceTimers[r][c]) >= DEBOUNCE_MS) {
        if (raw != keyState[r][c]) {
          keyState[r][c] = raw;
          if (raw) onKeyPress(r, c);
          else     onKeyRelease(r, c);
        }
      }
    }

    digitalWrite(rowPins[r], HIGH);
  }

  // drop the menu on its own if you walk away from it
  if (menuActive && (now - menuTimeout >= MENU_HIDE_MS)) {
    menuActive = false;
    drawMainScreen();
  }

  handleEncoder();
  handleEncoderSwitch();
}