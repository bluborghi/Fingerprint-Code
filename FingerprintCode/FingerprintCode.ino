const int buttonPin = 4;
const int dataPin = 5;  // pin 14 on ic
const int latchPin = 6; // pin 12 on ic
const int clockPin = 7; // pin 11 on ic
const int digitPins[] = {9, 10, 11, 12};



bool previousButtonState;
unsigned long lastTimePressed;

byte counter = 0;

struct digit_type {
  bool A, B, C, D, E, F, G;
};

const digit_type digit_0 = {1, 1, 1, 1, 1, 1, 0};
const digit_type digit_1 = {0, 1, 1, 0, 0, 0, 0};
const digit_type digit_2 = {1, 1, 0, 1, 1, 0, 1};
const digit_type digit_3 = {1, 1, 1, 1, 0, 0, 1};
const digit_type digit_4 = {0, 1, 1, 0, 0, 1, 1};
const digit_type digit_5 = {1, 0, 1, 1, 0, 1, 1};
const digit_type digit_6 = {1, 0, 1, 1, 1, 1, 1};
const digit_type digit_7 = {1, 1, 1, 0, 0, 0, 0};
const digit_type digit_8 = {1, 1, 1, 1, 1, 1, 1};
const digit_type digit_9 = {1, 1, 1, 1, 0, 1, 1};
const digit_type digit_blank = {0, 0, 0, 0, 0, 0, 0};
const digit_type digit_dash = {0, 0, 0, 0, 0, 0, 1};

byte getByteFromDigit(digit_type d, bool isDotted) {
  byte b;
  bitWrite(b, 0, d.A);
  bitWrite(b, 1, d.B);
  bitWrite(b, 2, d.C);
  bitWrite(b, 3, d.D);
  bitWrite(b, 4, d.E);
  bitWrite(b, 5, d.F);
  bitWrite(b, 6, d.G);
  bitWrite(b, 7, isDotted);
  return b;
}

byte getByteFromChar(char c, bool isDotted = 0) {
  switch (c) {
    case '0': return getByteFromDigit(digit_0, isDotted);
    case '1': return getByteFromDigit(digit_1, isDotted);
    case '2': return getByteFromDigit(digit_2, isDotted);
    case '3': return getByteFromDigit(digit_3, isDotted);
    case '4': return getByteFromDigit(digit_4, isDotted);
    case '5': return getByteFromDigit(digit_5, isDotted);
    case '6': return getByteFromDigit(digit_6, isDotted);
    case '7': return getByteFromDigit(digit_7, isDotted);
    case '8': return getByteFromDigit(digit_8, isDotted);
    case '9': return getByteFromDigit(digit_9, isDotted);
    case '-': return getByteFromDigit(digit_dash, isDotted);
    case ' ':
    default: return getByteFromDigit(digit_blank, isDotted);
  }
}

byte getByteFromInt(byte i, bool isDotted = false) {
  if (i <= 9)
    return getByteFromChar(char(i + 48), isDotted);
  else
    return getByteFromChar(' ', isDotted);
}

void updateShiftRegister(byte digit, byte digitNumber)
{
  digitalWrite(latchPin, LOW);
  //  unsigned long t1 = micros();
  setDisplayDigit(digitNumber); // 0 -> 1st digit, 1 -> 2nd digit ......
  shiftOut(dataPin, clockPin, MSBFIRST, digit); // 1st = A, 2nd = B, ... , 8th = dot
  //  unsigned long t2 = micros() - t1;
  //  Serial.println(t2);
  digitalWrite(latchPin, HIGH);
  //debugDigit(digit, getStatus(digitNumber));
}

void setDisplayDigit(int n) { // 0 -> 1st digit, 1 -> 2nd digit ......
  for (int i = 0; i < 4; i++) {
    if (3 - i == n) digitalWrite(digitPins[i], LOW);
    else digitalWrite(digitPins[i], HIGH);
  }
}

void setup()
{
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
  }

  pinMode(buttonPin, INPUT);
  previousButtonState = LOW;
  lastTimePressed = 0;

}

void loop() {
  unsigned long t1 = micros();

  if (digitalRead(buttonPin)) {
    if (previousButtonState == LOW) {
      lastTimePressed = millis();
    }
    else { //still pressing, this is just a prediction of what is going to happen in case of release
      unsigned long timePressed = millis() - lastTimePressed;
      showPrediction(timePressed);
    }

  }
  else {
    if (previousButtonState == HIGH) { //the button has been released! Smithers, release the hounds
      unsigned long timePressed = millis() - lastTimePressed;
      if (timePressed < 1 * 1000) {}
    }
    else {//button is untouched, normal program flow
      writeSingleDigit(1234, -1);//print 1234, no dot

    }
  }

  unsigned long t2 = micros() - t1;
  Serial.println(t2);
}

void showPrediction(unsigned long& t) {
  int dot = map(t % 4000, 0, 3999, 0, 3);
  
  if (t < 4000) writeString("----", dot);
  else if (t < 8000) writeString(" Add", dot);
  else if (t < 12000) writeString("----", dot);
  else if (t < 16000) writeString(" dEL", dot);
  else writeString("----", -1);
}

int writeString(String s, int dotPosition) {
  char currentChar = s.charAt(dotPosition);
  bool writeDot = (dotPosition == counter);
  byte digit = getByteFromChar(currentChar, writeDot); //gets the digit from a char
  updateShiftRegister(digit, counter); //writes the digit in the right spot
  counter ++;
  if (counter > 3) counter = 0;
}


int getSingleDigit(int fullNumber, int digitSpot) {
  int power = 1;
  for (int i = 0; i < digitSpot; i++) power *= 10;
  //  Serial.print("power: "); Serial.print(power);
  //  Serial.print(" fullNumber: "); Serial.println(fullNumber);
  return (fullNumber / power) % 10;
}

void writeSingleDigit(int fullNumber, int dotPosition) { //this is basically a multiplexing function
  int currentInt = getSingleDigit(fullNumber, counter); //gets a single digit int
  bool writeDot = (dotPosition == counter);
  byte digit = getByteFromInt(currentInt, writeDot); //gets the digit from an int value
  //  Serial.print("Counter: "); Serial.print(counter);
  //  Serial.print(" currentInt: "); Serial.println(currentInt);
  updateShiftRegister(digit, counter); //writes the digit in the right spot
  counter ++;
  if (counter > 3) counter = 0;
}



void debugDigit(byte d, byte s) { //seven segment display emulator on Serial
  Serial.print("Segments state:\t");
  for (int i = 0; i < 8; i++) {
    Serial.print(bitRead(d, i));
    Serial.print(" ");
  }
  Serial.println();

  Serial.print("Digits state:\t");
  for (int i = 0; i < 8; i++) {
    Serial.print(bitRead(s, i));
    Serial.print(" ");
  }
  Serial.println();

  if (bitRead(d, 0)) Serial.println(" -- ");
  else Serial.println("    ");

  if (bitRead(d, 5)) Serial.print('|');
  else Serial.print(' ');
  Serial.print("  ");
  if (bitRead(d, 1)) Serial.println('|');
  else Serial.println(' ');

  if (bitRead(d, 5)) Serial.print('|');
  else Serial.print(' ');
  Serial.print("  ");
  if (bitRead(d, 1)) Serial.println('|');
  else Serial.println(' ');

  if (bitRead(d, 6)) Serial.println(" -- ");
  else Serial.println("    ");

  if (bitRead(d, 4)) Serial.print('|');
  else Serial.print(' ');
  Serial.print("  ");
  if (bitRead(d, 2)) Serial.println('|');
  else Serial.println(' ');

  if (bitRead(d, 4)) Serial.print('|');
  else Serial.print(' ');
  Serial.print("  ");
  if (bitRead(d, 2)) Serial.println('|');
  else Serial.println(' ');

  if (bitRead(d, 3)) Serial.print(" -- ");
  else Serial.print("    ");

  if (bitRead(d, 7)) Serial.println('.');
  else Serial.println(' ');
}
