int latchPin = 5;
int clockPin = 6;
int dataPin = 4;


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
    case ' ':
    default: return getByteFromDigit(digit_blank, isDotted);
  }
}

byte getByteFromInt(byte i, bool isDotted = 0) {
  if (i <= 9)
    return getByteFromChar(char(i + 48), isDotted);
  else
    return getByteFromChar(' ', isDotted);
}

byte getStatus(int num) {
  if (num >= 0 && num < 8) {
    byte b = 0;
    bitSet(b, num);
    return b;
  }
  return 0;
}

void updateShiftRegister(byte digit, byte digitNumber)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, getStatus(digitNumber)); // getStatus(0) -> 1st = 1, getStatus(1) -> 2nd = 1, ... , getStatus(7) -> 8th = 1
  shiftOut(dataPin, clockPin, MSBFIRST, digit); // 1st = A, 2nd = B, ... , 8th = dot
  digitalWrite(latchPin, HIGH);
  debugDigit(digit, getStatus(digitNumber));
}

void setup()
{
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i <= 9; i++) {
    byte digit = getByteFromInt(i, 1);
    updateShiftRegister(digit, 7);
  }
}

void loop()
{


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
