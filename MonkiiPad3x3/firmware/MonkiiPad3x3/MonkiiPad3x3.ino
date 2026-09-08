#include <Keyboard.h>

const int colPins[3] = {5, 6, 7};
const int rowPins[3] = {2, 3, 4};

// I set off = numpad, on = arrows and shortcuts. Ts can be Toggled by holding key 1.
bool macroMode = false;

unsigned long pressStart = 0;
const unsigned long holdTime = 1500;

void setup() {

  for (int c = 0; c < 3; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }

  for (int r = 0; r < 3; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH);
  }

  Keyboard.begin();
}

void loop() {

  for (int r = 0; r < 3; r++) {

    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(30);

    for (int c = 0; c < 3; c++) {

      if (digitalRead(colPins[c]) == LOW) {

        // key 1 does 2 jobs so i wait and see how long its held
        if (r == 0 && c == 0) {

          pressStart = millis();

          while (digitalRead(colPins[c]) == LOW) {

            if (millis() - pressStart > holdTime) {

              macroMode = !macroMode;

              while (digitalRead(colPins[c]) == LOW);
              delay(200);
              goto skipKey;
            }
          }

          // let go early so it was just a '1'
          Keyboard.write('1');
          delay(20);
          goto skipKey;
        }

        if (!macroMode) {

          char numbers[3][3] = {
            {'1','2','3'},
            {'4','5','6'},
            {'7','8','9'}
          };

          Keyboard.write(numbers[r][c]);
        }

        // macro mode, arrows in the middle and edit keys on the bottom row
        else {

          if (r == 0 && c == 1) Keyboard.write(KEY_UP_ARROW);

          else if (r == 1 && c == 0) Keyboard.write(KEY_LEFT_ARROW);

          else if (r == 1 && c == 1) Keyboard.write(KEY_DOWN_ARROW);

          else if (r == 1 && c == 2) Keyboard.write(KEY_RIGHT_ARROW);

          else if (r == 2 && c == 0) {  // undo
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('z');
            delay(10);
            Keyboard.releaseAll();
          }

          else if (r == 2 && c == 1) {  // redo
            Keyboard.press(KEY_LEFT_CTRL);
            Keyboard.press('y');
            delay(10);
            Keyboard.releaseAll();
          }

          else if (r == 2 && c == 2) {
            Keyboard.write(KEY_DELETE);
          }

          else {
            Keyboard.write('1');
          }
        }

        // sit here til its released, crude but it stops repeats
        while (digitalRead(colPins[c]) == LOW);
        delay(20);

        skipKey:;
      }
    }

    digitalWrite(rowPins[r], HIGH);
  }
}
