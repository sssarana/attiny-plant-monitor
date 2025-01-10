#include <Adafruit_NeoPixel.h>

const int sensorpin = A0;
const int neopixel = 0;
const int displ = 1;
const int numPixels = 8;
const int button = 2;
const int tempSensor = A2;
const int lightSensor = A3;

int sensor;
float smoothedSensor = 0; 
const float alpha = 0.2;
const int delayTime = 6000; 

int wet = 135;
int dry = 45;
volatile int mode = 0;
volatile bool buttonPressed = false;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neopixel, NEO_GRB + NEO_KHZ800);

const float tempLow = 10.0;
const float tempHigh = 40.0;

void setup() {
  pinMode(lightSensor, INPUT);
  pinMode(sensorpin, INPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(tempSensor, INPUT);
  pinMode(displ, OUTPUT);
  digitalWrite(displ, HIGH);
  strip.begin();
  strip.show();
  attachInterrupt(0, handleButtonPress, FALLING);
}

void loop() {
  if (buttonPressed) {
    buttonPressed = false; 
    digitalWrite(displ, LOW);
  } else {
    digitalWrite(displ, HIGH);
  }

  switch (mode) {
    case 0:
      wet = 135;
      dry = 45;
      break;
    case 1:
      wet = 270;
      dry = 135;
      break;
    case 2:
      wet = 395;
      dry = 270;
      break;
    case 3:
      wet = 566;
      dry = 395;
      break;
    case 4:
      wet = 750;
      dry = 566;
      break;
  }

  sensor = analogRead(sensorpin);
  smoothedSensor = (smoothedSensor * (1 - alpha)) + (sensor * alpha);
  if (smoothedSensor >= wet) {
    strip.setPixelColor(0, strip.Color(0, 0, 255));
  } else if (smoothedSensor < dry) {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
  } else {
    strip.setPixelColor(0, strip.Color(0, 255, 0));
  }
  
  float temperature = readTemperature();
  if (temperature < tempLow) {
    strip.setPixelColor(1, strip.Color(0, 0, 255));
  } else if (temperature > tempHigh) {
    strip.setPixelColor(1, strip.Color(255, 0, 0)); 
  } else {
    strip.setPixelColor(1, strip.Color(0, 255, 0)); 
  }
  
  int lightLevel = analogRead(lightSensor);
  int numLEDs = map(lightLevel, 0, 1160, 1, numPixels);
  for (int i = 2; i < numPixels; i++) {
    if (i < numLEDs+1) {
      strip.setPixelColor(i, strip.Color(255, 0, 255)); 
    } else {
      strip.setPixelColor(i, strip.Color(0, 0, 0)); 
    }
  }
  
  strip.show();
}

void handleButtonPress() {
  mode = (mode + 1) % 5;
  buttonPressed = true;
}

float readTemperature() {
  int tempReading = analogRead(tempSensor);
  float voltage = tempReading * (4.25 / 1023.0); 
  float temperature = (voltage - 0.5) * 100; 
  return temperature;
}
