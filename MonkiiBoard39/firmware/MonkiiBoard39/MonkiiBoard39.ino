#include <Keyboard.h>

const int colPins[10] = {9, 8, 7, 6, 5, 4, 10, 16, 14, 15};
const int rowPins[4]  = {A3, A2, A1, A0};

bool keyState[4][10] = {false};
unsigned long lastChangeTime[4][10] = {0};
const int debounceDelay = 30;

// i made the mods sticky, tap one then hit a key and it picks it up.
// chording on a board this small is painful
bool ctrlActive = false;
bool shiftActive = false;
bool altActive = false;
bool guiActive = false;

// double tapping win sends it for real instead of just arming it
unsigned long lastGuiPressTime = 0;
const int doubleTapDelay = 300;

uint8_t keymap[4][10] = {

  {'q','w','e','r','t','y','u','i','o','p'},
  {'a','s','d','f','g','h','j','k','l', KEY_BACKSPACE},
  {'z','x','c','v','b','n','m',',','.', KEY_RETURN},

  // bottom row is all mods + a 2 key spacebar so i do it by hand in loop()
  {0,0,0,0,0,' ',' ',0,0,0}
};

void setup() {

  for (int c = 0; c < 10; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  for (int r = 0; r < 4; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  Keyboard.begin();
}

void applyModifiers() {
  if (ctrlActive)  Keyboard.press(KEY_LEFT_CTRL);
  if (shiftActive) Keyboard.press(KEY_LEFT_SHIFT);
  if (altActive)   Keyboard.press(KEY_LEFT_ALT);
  if (guiActive)   Keyboard.press(KEY_LEFT_GUI);
}

void releaseModifiers() {
  if (ctrlActive)  Keyboard.release(KEY_LEFT_CTRL);
  if (shiftActive) Keyboard.release(KEY_LEFT_SHIFT);
  if (altActive)   Keyboard.release(KEY_LEFT_ALT);
  if (guiActive)   Keyboard.release(KEY_LEFT_GUI);
}

void clearModifiers() {
  ctrlActive = false;
  shiftActive = false;
  altActive = false;
  guiActive = false;
}

void loop() {

  for (int r = 0; r < 4; r++) {

    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(30);

    for (int c = 0; c < 10; c++) {

      bool reading = (digitalRead(colPins[c]) == LOW);

      // ignore the change unless this key has been quiet long enough
      if (reading != keyState[r][c] &&
          millis() - lastChangeTime[r][c] > debounceDelay) {

        lastChangeTime[r][c] = millis();
        keyState[r][c] = reading;

        if (reading) {

          if (r == 3) {

            if (c == 0) ctrlActive = true;
            else if (c == 1) shiftActive = true;
            else if (c == 2) altActive = true;

            else if (c == 3) {

              unsigned long now = millis();

              if (now - lastGuiPressTime < doubleTapDelay) {
                Keyboard.press(KEY_LEFT_GUI);
                delay(50);
                Keyboard.release(KEY_LEFT_GUI);
                guiActive = false;
              } else {
                guiActive = true;
              }

              lastGuiPressTime = now;
            }

            // both halves of the wide spacebar
            else if (c == 5 || c == 6) {
              Keyboard.press(' ');
            }

            continue;
          }

          uint8_t key = keymap[r][c];

          if (key != 0) {
            applyModifiers();
            Keyboard.press(key);
          }
        }

        // on release, letting go of a real key also clears my sticky mods
        else {

          if (r == 3) {

            if (c == 5 || c == 6) {
              Keyboard.release(' ');
            }

            continue;
          }

          uint8_t key = keymap[r][c];

          if (key != 0) {
            Keyboard.release(key);
            releaseModifiers();
            clearModifiers();
          }
        }
      }
    }

    digitalWrite(rowPins[r], HIGH);
  }
}
