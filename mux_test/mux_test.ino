#include <Adafruit_NeoPixel.h>
#define LED_PIN1 14
#define LED_PIN2 15
#define LED_COUNT 20
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int hys = 50;
// 一番明るいときに足される数字
// TODO: こいつをキャリブしたい
int max_range = 200;

//1st Mux control pins
int s0 = 2;
int s1 = 3;
int s2 = 4;
int s3 = 5;

// 2nd mux
int s4 = 6;
int s5 = 7;
int s6 = 8;
int s7 = 9;

//3rd mux
int s8 = 10;
int s9 = 11;
int s10 = 12;
int s11 = 13;

//Mux in "SIG" pin
int SIG_pin = 0;
int SIG_pin2 = 1;
int SIG_pin3 = 2;

int sensor_count = 40;
int calibration_count = 20;
int calibrated_number[40];
int current_value[40];

int muxChannel[16][4] = {
  {0, 0, 0, 0}, //channel 0
  {1, 0, 0, 0}, //channel 1
  {0, 1, 0, 0}, //channel 2
  {1, 1, 0, 0}, //channel 3
  {0, 0, 1, 0}, //channel 4
  {1, 0, 1, 0}, //channel 5
  {0, 1, 1, 0}, //channel 6
  {1, 1, 1, 0}, //channel 7
  {0, 0, 0, 1}, //channel 8
  {1, 0, 0, 1}, //channel 9
  {0, 1, 0, 1}, //channel 10
  {1, 1, 0, 1}, //channel 11
  {0, 0, 1, 1}, //channel 12
  {1, 0, 1, 1}, //channel 13
  {0, 1, 1, 1}, //channel 14
  {1, 1, 1, 1} //channel 15
};

void setup() {
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(s4, OUTPUT);
  pinMode(s5, OUTPUT);
  pinMode(s6, OUTPUT);
  pinMode(s7, OUTPUT);
  pinMode(s8, OUTPUT);
  pinMode(s9, OUTPUT);
  pinMode(s10, OUTPUT);
  pinMode(s11, OUTPUT);

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  digitalWrite(s4, LOW);
  digitalWrite(s5, LOW);
  digitalWrite(s6, LOW);
  digitalWrite(s7, LOW);
  digitalWrite(s8, LOW);
  digitalWrite(s9, LOW);
  digitalWrite(s10, LOW);
  digitalWrite(s11, LOW);

  Serial.begin(57600);
  calibration();
  strip.begin();
  strip2.begin();

}

void loop() {
  strip.clear(); // Set all pixel colors to 'off'
  strip2.clear(); // Set all pixel colors to 'off'
  for (int i = 0; i < sensor_count; i ++) {
    Serial.print("Value at channel ");
    Serial.print(i);
    //1st mux
    if (i < 16) {
      Serial.print(" 1 : ");
      Serial.println(readMux(i));
      current_value[i] = readMux(i);
    } else if (i >= 16 && i < 32) {
      Serial.print(" 2 : ");
      Serial.println(readMux2(i - 16));
      current_value[i] = readMux2(i - 16);
    } else {}
  }


  // LEDを光らせる
  for (int i = 0; i < LED_COUNT1; i++) {
    // Here we're using a moderately bright green color:
    if (current_value[i] > calibrated_number[i] + hys) {
      strip.setPixelColor(i, strip.Color(0, 255 * current_value[i] / (calibrated_number[i] + max_range), 0));
    }
  }
  strip.show();
  delay(10);

}

// 各センサの20回分の値を足し算し、回数で割る
// その値を配列に突っ込む
// Sum up 20 times values for each sensors and divide it by calibration count
// And push them to array
void calibration() {
  for (int i = 0; i < calibration_count; i++ ) {
    for (int j = 0; j < sensor_count; j ++) {
      Serial.print("Calib Value at channel ");
      Serial.print(j);
      Serial.print("is : ");
      if (j < 16) {
        Serial.println(readMux(j));
        calibrated_number[j] += readMux(j);
      } else if (j >= 16 && j < 20) {
        Serial.println(readMux2(j - 16));
        calibrated_number[j] += readMux2(j - 16);
      } else if (j >= 20 && j < 28) {
        Serial.println(readMux3(j - 20));
        calibrated_number[j] += readMux3(j - 20);
      } else if (j >= 28) {
         Serial.println(readMux2(j - 28 + 4));
        calibrated_number[j] += readMux2(j - 28 + 4);
      }
      delay(10);
    }
  }
  // get average
  for (int i = 0; i < ch_count; i++) {
    calibrated_number[i] = calibrated_number[i] / calibration_count;
  }
  Serial.println("Calibration is done");
  for (int i = 0; i < sensor_count; i++) {
    Serial.print(i);
    Serial.print(": Calibrated value is:");
    Serial.println(calibrated_number[i]);
  }
}
int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};
  //loop through the 4 sig
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  //read the value at the SIG pin
  int val = analogRead(SIG_pin);
  //return the value
  return val;
}

int readMux2(int channel) {
  int controlPin[] = {s4, s5, s6, s7};
  //loop through the 4 sig
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  //read the value at the SIG pin
  int val = analogRead(SIG_pin2);
  //return the value
  return val;
}
int readMux3(int channel) {
  int controlPin[] = {s8, s9, s10, s11};
  //loop through the 4 sig
  for (int i = 0; i < 4; i ++) {
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }
  //read the value at the SIG pin
  int val = analogRead(SIG_pin3);
  //return the value
  return val;
}
