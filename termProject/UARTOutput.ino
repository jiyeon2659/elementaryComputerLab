#include <LiquidCrystal.h>
#include <LedControl.h>
#include <Servo.h>

Servo myServo;
LedControl myDotMatrix = LedControl(12, 11, 10, 1);
LiquidCrystal myLCD(22, 23, 24, 25, 26, 27);
LiquidCrystal myLCD2(30, 31, 32, 33, 34, 35);

int redPin = 7;
int bluePin = 6;
int greenPin = 5;

int lcdToggleButton = 14;
int toggleLastButtonState = 0;
int toggleButtonState = 0;
int toggleButtonCount = 0;

int buttonTime = 0;
long debounce = 50;

int servoPin = 8;

String str = "";
double temperature, humidity, dust_density;

byte humidityChar[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B11111,
  B11111,
  B01110
};

byte temperatureChar[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

byte dustChar[] = {
  B01010,
  B10101,
  B00000,
  B01010,
  B10101,
  B00000,
  B01010,
  B10101
};

byte C[] = {
  B10000,
  B00110,
  B01001,
  B01000,
  B01000,
  B01001,
  B00110,
  B00000
};

byte microChar[] = {
  B00000,
  B00101,
  B00101,
  B00101,
  B01111,
  B01001,
  B10000,
  B10000
};

byte squareChar[] = {
  B11100,
  B00100,
  B11100,
  B00100,
  B11100,
  B00000,
  B00000,
  B00000
};

byte goodClimate[] = {
  B00000000,
  B01000010,
  B00100100,
  B01000010,
  B00000000,
  B01000010,
  B00111100,
  B00000000
};

byte sosoClimate[] = {
  B00000000,
  B01000010,
  B01000010,
  B00000000,
  B01000010,
  B01000010,
  B00111100,
  B00000000
};

byte badClimate[] = {
  B00000000,
  B01000010,
  B00100100,
  B00000000,
  B00000000,
  B00111100,
  B01000010,
  B00000000
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(lcdToggleButton, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);

  myLCD.createChar(0, humidityChar);
  myLCD.createChar(1, temperatureChar);
  myLCD.createChar(2, dustChar);
  myLCD.createChar(3, C);
  myLCD.createChar(4, microChar);
  myLCD.createChar(5, squareChar);
  myLCD.begin(16, 2);
  myLCD.clear();
  myLCD2.begin(16, 2);
  myLCD2.clear();

  myServo.attach(servoPin);
  
  myDotMatrix.shutdown(0, false);
  myDotMatrix.setIntensity(0, 5);
  myDotMatrix.clearDisplay(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial1.available()) {
    str = Serial1.readString();
    Serial1.setTimeout(20);
    Serial.println(str);

    if (str.charAt(0) == '<' and str.charAt(str.length()-1) == '>') {
      int first = str.indexOf(",");
      int second = str.indexOf(",", first + 1);
      int third = str.indexOf(">", second);

      String str1 = str.substring(1, first);
      String str2 = str.substring(first+1, second);
      String str3 = str.substring(second+1, third);

      temperature = str1.toDouble();
      humidity = str2.toDouble();
      dust_density = str3.toDouble();

      Serial.println(temperature);
      Serial.println(humidity);
      Serial.println(dust_density);
    }

  toggleButtonState = digitalRead(lcdToggleButton);
  
  motorControl(dust_density);
  dotMatrixControl(temperature, humidity, dust_density);
  myLCDOutput(temperature, humidity, dust_density);
  myLCD2Output(temperature, humidity, dust_density);
  coloredLEDControl(temperature, humidity, dust_density);
  delay(100);

  }

}

void motorControl(double dust) {
  if (dust < 75) {
    myServo.write(180);
  }

  if (dust > 75) {
    myServo.write(60);
  }
}

void dotMatrixControl(double temp, double humid, double dust) {
  if (dust > 75 || humid < 20 || humid > 80 || temp < 10 || temp > 30) {
    for (int i = 0; i < 8; i++) {
    myDotMatrix.setRow(0, i, badClimate[i]);
    }
  }
  else if (40 < humid < 60 && 15 < temp < 25 && dust < 75) {
    for (int i = 0; i < 8; i++) {
    myDotMatrix.setRow(0, i, goodClimate[i]);
    }; 
  }
  else {
    for (int i = 0; i < 8; i++) {
    myDotMatrix.setRow(0, i, sosoClimate[i]);
    }
  }
}

void myLCDOutput(double temp, double humid, double dust) {

    myLCD.clear();
    myLCD.setCursor(0, 0);
    myLCD.write(byte(1));
    myLCD.setCursor(1, 0);
    myLCD.write(':');
    myLCD.setCursor(2, 0);
    myLCD.print(temp);
    myLCD.setCursor(7, 0);
    myLCD.write(byte(3));

    myLCD.setCursor(9, 0);
    myLCD.write(byte(0));
    myLCD.setCursor(10, 0);
    myLCD.write(':');
    myLCD.setCursor(11, 0);
    myLCD.print(humid);

    myLCD.setCursor(0, 1);
    myLCD.write(byte(2));
    myLCD.setCursor(1, 1);
    myLCD.write(':');
    myLCD.setCursor(2, 1);
    myLCD.print(dust);
    myLCD.setCursor(8, 1);
    myLCD.write(byte(4));
    myLCD.setCursor(9, 1);
    myLCD.write('g');
    myLCD.setCursor(10, 1);
    myLCD.write('/');
    myLCD.setCursor(11, 1);
    myLCD.write('m');
    myLCD.setCursor(12, 1);
    myLCD.write(byte(5));
  }

void myLCD2Output(double temp, double humid, double dust) {
  if (dust > 75) {
      myLCD2.clear();
      myLCD2.setCursor(0, 0);
      myLCD2.print("Don't open");
      myLCD2.setCursor(0, 1);
      myLCD2.print("the window!");
    }
    else if (temp > 30) {
      myLCD2.clear();
      myLCD2.setCursor(0, 0);
      myLCD2.print("Don't do");
      myLCD2.setCursor(0, 1);
      myLCD2.print("outdoor activity");
    }
    else if (humid > 70) {
      myLCD2.clear();
      myLCD2.setCursor(0, 0);
      myLCD2.print("Turn on");
      myLCD2.setCursor(0, 1);
      myLCD2.print("dehumidifier");  
    }
    else {
      myLCD2.clear();
      myLCD2.setCursor(0, 0);
      myLCD2.print("Have a good day!");
    }
}

void coloredLEDControl(double temp, double humid, double dust) {
  if (dust > 75 || humid < 20 || humid > 80 || temp < 10 || temp > 30) { // bad
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW); // red
  }
  else if (40 < humid < 60 && 15 < temp < 25 && dust < 75) { // good 
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH); // blue

  }
  else {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH); // green
  }


}

