//////////////////////////////////////////////////
//              LICENSE                        //
//////////////////////////////////////////////////
#pragma region LICENSE
/*
  ************************************************************************************
  * MIT License
  *
  * Copyright (c) 2025 Crunchlabs LLC (LabelMaker Code)
  *
  ************************************************************************************
*/
#pragma endregion LICENSE

//////////////////////////////////////////////////
//              LIBRARIES                      //
//////////////////////////////////////////////////
#pragma region LIBRARIES
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <ezButton.h>
#include <Servo.h>
#pragma endregion LIBRARIES

//////////////////////////////////////////////////
//          PINS AND PARAMETERS                //
//////////////////////////////////////////////////
#pragma region PINS AND PARAMS

LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD 16x2 at 0x27

ezButton button1(14); // joystick button

#define INIT_MSG  "Initializing..."
#define MODE_NAME "   LABELMAKER   "
#define PRINT_CONF "  PRINT LABEL?  "
#define PRINTING  "    PRINTING    "
#define MENU_CLEAR ":                "

// text / scale
int x_scale = 230;
int y_scale = 230;
int scale   = x_scale;
int space   = x_scale * 5;

// Joystick
const int joystickXPin = A2;
const int joystickYPin = A1;
const int joystickButtonThreshold = 200;

// Menu / text
const char alphabet[] = "_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?,.#@";
int alphabetSize = sizeof(alphabet) - 1;
String text;

int currentCharacter = 0;
int cursorPosition   = 0;
int currentPage      = 0;
const int charactersPerPage = 16;

// Steppers
const int stepCount          = 200;
const int stepsPerRevolution = 2048;

Stepper xStepper(stepsPerRevolution, 6, 8, 7, 9);
Stepper yStepper(stepsPerRevolution, 2, 4, 3, 5);

int xPins[4] = {6, 8, 7, 9};
int yPins[4] = {2, 4, 3, 5};

// Servo
const int SERVO_PIN = 13;
Servo servo;
int angle = 30;

// States
enum State { MainMenu, Editing, PrintConfirmation, Printing };
State currentState = MainMenu;
State prevState    = Printing;

enum jState { LEFT, RIGHT, UP, DOWN, MIDDLE, UPRIGHT, UPLEFT, DOWNRIGHT, DOWNLEFT };
jState joyState    = MIDDLE;
jState prevJoyState = MIDDLE;

boolean pPenOnPaper = false;
int lineCount = 0;

int xpos = 0;
int ypos = 0;
const int posS = 2;
const int posM = 7;
const int posL = 12;

bool joyUp;
bool joyDown;
bool joyLeft;
bool joyRight;

int button1State;
int joystickX;
int joystickY;

#pragma endregion PINS AND PARAMS

//////////////////////////////////////////////////
//           CHARACTER VECTORS                 //
//////////////////////////////////////////////////
#pragma region CHARACTER VECTORS
const uint8_t vector[63][14] = {
  /*
    encoding:
    ones     = y
    tens     = x
    hundreds = draw/don't draw
    200      = end
    222      = plot point
  */
  {  0, 124, 140,  32, 112, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //A
  {  0, 104, 134, 132,   2, 142, 140, 100, 200, 200, 200, 200, 200, 200}, //B
  { 41, 130, 110, 101, 103, 114, 134, 143, 200, 200, 200, 200, 200, 200}, //C
  {  0, 104, 134, 143, 141, 130, 100, 200, 200, 200, 200, 200, 200, 200}, //D
  { 40, 100, 104, 144,  22, 102, 200, 200, 200, 200, 200, 200, 200, 200}, //E
  {  0, 104, 144,  22, 102, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //F
  { 44, 104, 100, 140, 142, 122, 200, 200, 200, 200, 200, 200, 200, 200}, //G
  {  0, 104,   2, 142,  44, 140, 200, 200, 200, 200, 200, 200, 200, 200}, //H
  {  0, 104, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //I
  {  1, 110, 130, 141, 144, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //J
  {  0, 104,   2, 142, 140,  22, 144, 200, 200, 200, 200, 200, 200, 200}, //K
  { 40, 100, 104, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //L
  {  0, 104, 122, 144, 140, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //M
  {  0, 104, 140, 144, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //N
  { 10, 101, 103, 114, 134, 143, 141, 130, 110, 200, 200, 200, 200, 200}, //O
  {  0, 104, 144, 142, 102, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //P
  {  0, 104, 144, 142, 120, 100,  22, 140, 200, 200, 200, 200, 200, 200}, //Q
  {  0, 104, 144, 142, 102,  22, 140, 200, 200, 200, 200, 200, 200, 200}, //R
  {  0, 140, 142, 102, 104, 144, 200, 200, 200, 200, 200, 200, 200, 200}, //S
  { 20, 124,   4, 144, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //T
  {  4, 101, 110, 130, 141, 144, 200, 200, 200, 200, 200, 200, 200, 200}, //U
  {  4, 120, 144, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //V
  {  4, 100, 122, 140, 144, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //W
  {  0, 144,   4, 140, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //X
  {  4, 122, 144,  22, 120, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //Y
  {  4, 144, 100, 140, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //Z
  {  0, 104, 144, 140, 100, 144, 200, 200, 200, 200, 200, 200, 200, 200}, //0
  {  0, 140,  20, 124, 104, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //1
  {  4, 144, 142, 102, 100, 140, 200, 200, 200, 200, 200, 200, 200, 200}, //2
  {  0, 140, 144, 104,  12, 142, 200, 200, 200, 200, 200, 200, 200, 200}, //3
  { 20, 123,  42, 102, 104, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //4
  {  0, 140, 142, 102, 104, 144, 200, 200, 200, 200, 200, 200, 200, 200}, //5
  {  2, 142, 140, 100, 104, 144, 200, 200, 200, 200, 200, 200, 200, 200}, //6
  {  0, 144, 104,  12, 132, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //7
  {  0, 140, 144, 104, 100,   2, 142, 200, 200, 200, 200, 200, 200, 200}, //8
  {  0, 140, 144, 104, 102, 142, 200, 200, 200, 200, 200, 200, 200, 200}, //9
  {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //
  {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, //
  {  0, 144, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // /
  {  0, 102, 124, 142, 140,  42, 102,   4, 103,  44, 143, 200, 200, 200}, // Ä
  {  0, 102, 142, 140, 100,   2,  14, 113,  34, 133, 200, 200, 200, 200}, // Ö
  {  4, 100, 140, 144,  14, 113,  34, 133, 200, 200, 200, 200, 200, 200}, // Ü
  {  0, 111, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // ,
  {  2, 142, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // -
  {  0, 222, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // .
  {  0, 222,   1, 104, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // !
  { 20, 222,  21, 122, 142, 144, 104, 200, 200, 200, 200, 200, 200, 200}, // ?
  {  0, 104, 134, 133, 122, 142, 140, 110, 200, 200, 200, 200, 200, 200}, // ß
  { 23, 124, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // '
  { 42, 120, 100, 101, 123, 124, 104, 103, 130, 140, 200, 200, 200, 200}, // &
  {  2, 142,  20, 124, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // +
  { 21, 222,  23, 222, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // :
  { 10, 121,  22, 222, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // ;
  { 14, 113,  33, 134, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // "
  { 10, 114,  34, 130,  41, 101,   3, 143, 200, 200, 200, 200, 200, 200}, // #
  { 34, 124, 120, 130, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // (
  { 10, 120, 124, 114, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // )
  {  1, 141,  43, 103, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200}, // =
  { 31, 133, 113, 111, 141, 144, 104, 100, 140, 200, 200, 200, 200, 200}, // @
  {  2, 142,  20, 124,   4, 140,   0, 144, 200, 200, 200, 200, 200, 200}, // *
  {  0, 140, 144, 104, 100,  12, 113,  33, 132,  31, 111, 200, 200, 200}, // } Smiley
  {  0, 140, 144, 104, 100,  13, 222,  33, 222,  32, 131, 111, 112, 132}, // ~ Open mouth
  { 20, 142, 143, 134, 123, 114, 103, 102, 120, 200, 200, 200, 200, 200}  // $ Heart
};
#pragma endregion CHARACTER VECTORS

//////////////////////////////////////////////////
//                  SETUP                      //
//////////////////////////////////////////////////
#pragma region SETUP
void setup() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print(INIT_MSG);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);

  button1.setDebounceTime(50);

  servo.attach(SERVO_PIN);
  servo.write(angle);

  plot(false);  // bring pen to tape surface for loading

  yStepper.setSpeed(12);
  xStepper.setSpeed(10);

  penUp();
  homeYAxis();

  ypos = 0;
  xpos = 0;

  releaseMotors();
  lcd.clear();
}
#pragma endregion SETUP

//////////////////////////////////////////////////
//                   LOOP                      //
//////////////////////////////////////////////////
#pragma region LOOP
void loop() {

  button1.loop();
  button1State = button1.getState();

  // =====================
  // USB SERIAL HANDLER
  // =====================
  if (Serial.available()) {
    String incoming = Serial.readStringUntil('\n');
    incoming.trim();

    if (incoming == "CONNECTED") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Connected");
      lcd.setCursor(0, 1);
      lcd.print("Waiting Print");
    }
    else if (incoming.startsWith("PRINT:")) {
      String label = incoming.substring(6);
      text = label;
      currentState = Printing;
      prevState    = PrintConfirmation;
    }
    else if (incoming.length() > 0) {
      text = incoming;
      currentState = Printing;
      prevState    = PrintConfirmation;
    }
  }

  joystickX = analogRead(joystickXPin);
  joystickY = analogRead(joystickYPin);
  joyUp   = joystickY < (512 - joystickButtonThreshold);
  joyDown = joystickY > (512 + joystickButtonThreshold);
  joyLeft = joystickX < (512 - joystickButtonThreshold);
  joyRight= joystickX > (512 + joystickButtonThreshold);

  switch (currentState) {

    case MainMenu: {
      if (prevState != MainMenu) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(MODE_NAME);
        lcd.setCursor(0, 1);
        lcd.print("      START     ");
        cursorPosition = 5;
        prevState = MainMenu;
      }

      lcd.setCursor(cursorPosition, 1);
      if (millis() % 600 < 400) lcd.print(">");
      else                      lcd.print(" ");

      if (button1.isPressed()) {
        lcd.clear();
        currentState = Editing;
        prevState    = MainMenu;
      }
    }
    break;

    case Editing: {
      if (prevState != Editing) {
        lcd.clear();
        prevState = Editing;
      }

      lcd.setCursor(0, 0);
      lcd.print(":");
      lcd.setCursor(1, 0);
      lcd.print(text);

      if (joyUp) {
        if (currentCharacter > 0) currentCharacter--;
        delay(250);
      } else if (joyDown) {
        if (currentCharacter < (alphabetSize - 1)) currentCharacter++;
        delay(250);
      }

      lcd.setCursor(text.length() + 1, 0);
      if (millis() % 600 < 450) lcd.print(alphabet[currentCharacter]);
      else                      lcd.print(" ");

      if (joyLeft) {
        if (text.length() > 0) {
          text.remove(text.length() - 1);
          lcd.setCursor(0, 0);
          lcd.print(MENU_CLEAR);
          lcd.setCursor(1, 0);
          lcd.print(text);
        }
        delay(250);
      } else if (joyRight) {
        if (currentCharacter == 0) text += ' ';
        else {
          text += alphabet[currentCharacter];
          currentCharacter = 0;
        }
        delay(250);
      }

      if (button1.isPressed()) {
        if (currentCharacter == 0) text += ' ';
        else {
          text += alphabet[currentCharacter];
          currentCharacter = 0;
        }
        lcd.clear();
        currentState = PrintConfirmation;
        prevState    = Editing;
      }
    }
    break;

    case PrintConfirmation: {
      if (prevState == Editing) {
        lcd.setCursor(0, 0);
        lcd.print(PRINT_CONF);
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(2, 1);
        cursorPosition = 2;
        prevState = PrintConfirmation;
      }

      if (joyLeft) {
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(2, 1);
        cursorPosition = 2;
        delay(200);
      } else if (joyRight) {
        lcd.setCursor(0, 1);
        lcd.print("   YES     NO   ");
        lcd.setCursor(10, 1);
        cursorPosition = 10;
        delay(200);
      }

      lcd.setCursor(cursorPosition, 1);
      if (millis() % 600 < 400) lcd.print(">");
      else                      lcd.print(" ");

      if (button1.isPressed()) {
        if (cursorPosition == 2) {
          lcd.clear();
          currentState = Printing;
          prevState    = PrintConfirmation;
        } else if (cursorPosition == 10) {
          lcd.clear();
          currentState = Editing;
          prevState    = PrintConfirmation;
        }
      }
    }
    break;

    case Printing: {
      if (prevState == PrintConfirmation) {
        lcd.setCursor(0, 0);
        lcd.print(PRINTING);
      }

      plotText(text, xpos, ypos);

      line(xpos + space, 0, 0);
      xpos = 0;
      ypos = 0;

      text = "";
      yStepper.step(-2250);
      releaseMotors();
      lcd.clear();
      currentState = Editing;
      prevState    = Printing;
    }
    break;
  }
}
#pragma endregion LOOP

//////////////////////////////////////////////////
//                 FUNCTIONS                    //
//////////////////////////////////////////////////
#pragma region FUNCTIONS

void plotText(String &str, int x, int y) {
  int pos = 0;
  Serial.println("plot string");
  Serial.println(str);
  for (int i = 0; i < str.length(); i++) {
    char c = char(str.charAt(i));
    if (byte(c) != 195) {
      if (c == ' ') {
        pos += space;
      } else {
        plotCharacter(c, x + pos, y);
        pos += space;
        if (c == 'I' || c == 'i') pos -= (scale * 4) / 1.1;
        if (c == ',')             pos -= (scale * 4) / 1.2;
      }
    }
  }
  Serial.println();
  releaseMotors();
}

void plotCharacter(char c, int x, int y) {
  Serial.print(uint8_t(c));
  Serial.print(">");

  uint8_t character = 38;
  if (uint8_t(c) > 64 && uint8_t(c) < 91)  character = uint8_t(c) - 65;
  if (uint8_t(c) > 96 && uint8_t(c) < 123) character = uint8_t(c) - 97;
  if (uint8_t(c) > 47 && uint8_t(c) < 58)  character = uint8_t(c) - 22;
  if (uint8_t(c) == 164 || uint8_t(c) == 132) character = 39;
  if (uint8_t(c) == 182 || uint8_t(c) == 150) character = 40;
  if (uint8_t(c) == 188 || uint8_t(c) == 156) character = 41;
  if (uint8_t(c) == 44)  character = 42;
  if (uint8_t(c) == 45)  character = 43;
  if (uint8_t(c) == 46)  character = 44;
  if (uint8_t(c) == 33)  character = 45;
  if (uint8_t(c) == 63)  character = 46;
  if (uint8_t(c) == 123) character = 47;
  if (uint8_t(c) == 39)  character = 48;
  if (uint8_t(c) == 38)  character = 49;
  if (uint8_t(c) == 43)  character = 50;
  if (uint8_t(c) == 58)  character = 51;
  if (uint8_t(c) == 59)  character = 52;
  if (uint8_t(c) == 34)  character = 53;
  if (uint8_t(c) == 35)  character = 54;
  if (uint8_t(c) == 40)  character = 55;
  if (uint8_t(c) == 41)  character = 56;
  if (uint8_t(c) == 61)  character = 57;
  if (uint8_t(c) == 64)  character = 58;
  if (uint8_t(c) == 42)  character = 59;
  if (uint8_t(c) == 125) character = 60;
  if (uint8_t(c) == 126) character = 61;
  if (uint8_t(c) == 36)  character = 62;

  Serial.print("letter: ");
  Serial.println(c);

  for (int i = 0; i < 14; i++) {
    int v = vector[character][i];
    if (v == 200) break;
    if (v == 222) {
      plot(true);
      delay(50);
      plot(false);
    } else {
      int draw = 0;
      if (v > 99) {
        draw = 1;
        v -= 100;
      }
      int cx = v / 10;
      int cy = v - cx * 10;

      int x_start = x;
      int x_end   = x + cx * x_scale;
      int y_start = y;
      int y_end   = y + cy * y_scale * 3.5;

      Serial.print("Scale: ");
      Serial.print(scale);
      Serial.print("  X Goal: ");
      Serial.print(x_end);
      Serial.print("  Y Goal: ");
      Serial.print(y_end);
      Serial.print("  Draw: ");
      Serial.println(draw);

      line(x_end, y_end, draw);
    }
  }
}

void line(int newx, int newy, bool drawing) {
  if (drawing < 2) {
    plot(drawing);
  } else {
    plot((stepCount / drawing) % 2);
  }

  int i;
  long over = 0;

  long dx = newx - xpos;
  long dy = newy - ypos;
  int dirx = dx > 0 ? -1 : 1;
  int diry = dy > 0 ?  1 : -1;

  dx = abs(dx);
  dy = abs(dy);

  if (dx > dy) {
    over = dx / 2;
    for (i = 0; i < dx; i++) {
      xStepper.step(dirx);
      over += dy;
      if (over >= dx) {
        over -= dx;
        yStepper.step(diry);
      }
    }
  } else {
    over = dy / 2;
    for (i = 0; i < dy; i++) {
      yStepper.step(diry);
      over += dx;
      if (over >= dy) {
        over -= dy;
        xStepper.step(dirx);
      }
    }
  }
  xpos = newx;
  ypos = newy;
}

void plot(boolean penOnPaper) {
  if (penOnPaper) angle = 80;
  else            angle = 25;

  servo.write(angle);
  if (penOnPaper != pPenOnPaper) delay(50);
  pPenOnPaper = penOnPaper;
}

void penUp() {
  servo.write(25);
}

void penDown() {
  servo.write(80);
}

void releaseMotors() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(xPins[i], 0);
    digitalWrite(yPins[i], 0);
  }
  plot(false);
}

void homeYAxis() {
  yStepper.step(-3000);
}

void resetScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(": ");
  lcd.setCursor(1, 0);
  cursorPosition = 1;
}
#pragma endregion FUNCTIONS

//////////////////////////////////////////////////
//                 END CODE                     //
//////////////////////////////////////////////////
