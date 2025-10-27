#include <Wire.h>
//the hartbeat sensor does not work 


void setup() {
  Serial.begin(115200);
  while(!Serial) {}
  Serial.println("\nI2C scanner...");
  Wire.begin(); // MKR1010  SDA/SCL  
}

void loop() {
  byte count = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    byte err = Wire.endTransmission();
    if (err == 0) {
      Serial.print("Found I2C device at 0x");
      if (addr < 16) Serial.print("0");
      Serial.println(addr, HEX);
      count++;
    }
  }
  if (count == 0) Serial.println("No I2C devices found");
  delay(1500);
}
