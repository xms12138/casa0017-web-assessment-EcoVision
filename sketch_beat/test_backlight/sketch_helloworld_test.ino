#include <Wire.h>

const uint8_t LCD_ADDR = 0x3E;
const uint8_t RGB_ADDR = 0x62;

void setRGB(uint8_t r, uint8_t g, uint8_t b){
  Wire.beginTransmission(RGB_ADDR);
  Wire.write(0x00); Wire.write(0x00);
  Wire.endTransmission();
  Wire.beginTransmission(RGB_ADDR);
  Wire.write(0x01); Wire.write(0x00);
  Wire.endTransmission();
  Wire.beginTransmission(RGB_ADDR);
  Wire.write(0x08); Wire.write(0xAA);
  Wire.endTransmission();
  Wire.beginTransmission(RGB_ADDR);
  Wire.write(0x04); Wire.write(r);
  Wire.endTransmission();
  Wire.beginTransmission(RGB_ADDR);
  Wire.write(0x03); Wire.write(g);
  Wire.endTransmission();
  Wire.beginTransmission(RGB_ADDR);
  Wire.write(0x02); Wire.write(b);
  Wire.endTransmission();
}

void lcdCmd(uint8_t cmd){
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x80);
  Wire.write(cmd);
  Wire.endTransmission();
  delay(2);
}

void lcdData(uint8_t data){
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x40);
  Wire.write(data);
  Wire.endTransmission();
  delayMicroseconds(100);
}

void lcdPrint(const char* str){
  while(*str) lcdData(*str++);
}

void setup(){
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n╔═══════════════════════════════════╗");
  Serial.println("║  Grove LCD Final Test             ║");
  Serial.println("╚═══════════════════════════════════╝\n");
  
  Wire.begin();
  Wire.setClock(100000);
  delay(100);
  
  // I2C
  Serial.println("Checking I2C devices...");
  Wire.beginTransmission(LCD_ADDR);
  bool lcd_ok = (Wire.endTransmission() == 0);
  Wire.beginTransmission(RGB_ADDR);
  bool rgb_ok = (Wire.endTransmission() == 0);
  
  Serial.print("LCD (0x3E): ");
  Serial.println(lcd_ok ? "✓ Found" : "✗ Not found");
  Serial.print("RGB (0x62): ");
  Serial.println(rgb_ok ? "✓ Found" : "✗ Not found");
  
  if(!lcd_ok || !rgb_ok){
    Serial.println("\n❌ ERROR: Device not responding!");
    Serial.println("Check wiring:\n");
    Serial.println("Grove → MKR WiFi 1010");
    Serial.println("GND  → GND");
    Serial.println("VCC  → VCC or 5V");
    Serial.println("SDA  → D11");
    Serial.println("SCL  → D12");
    while(1);
  }
  
  Serial.println("\n⚠️  POWER SUPPLY CHECK:");
  Serial.println("MKR WiFi 1010 provides 3.3V on VCC pin");
  Serial.println("Grove LCD needs 5V for best contrast!");
  Serial.println("\nIf you see NO text:");
  Serial.println("→ Connect LCD VCC to external 5V source");
  Serial.println("→ Keep GND connected to both\n");
  
  // 关闭背光
  Serial.println("Turning OFF backlight...");
  setRGB(0, 0, 0);
  delay(500);
  
   
  Serial.println("Initializing LCD with MAX contrast...");
  delay(200);
  lcdCmd(0x38); delay(5);
  lcdCmd(0x39); delay(5);
  lcdCmd(0x14); delay(5);
  
   
  lcdCmd(0x7F); delay(5);   
  lcdCmd(0x5F); delay(5);   
  
  lcdCmd(0x6F); delay(500);  // Follower ON, Rab=111
  lcdCmd(0x38); delay(5);
  lcdCmd(0x0C); delay(5);    // Display ON
  lcdCmd(0x01); delay(10);   // Clear
  lcdCmd(0x06); delay(5);
  
  Serial.println("LCD initialized!\n");
  
  // HELLO WORLD 
  Serial.println("Writing 'HELLO WORLD' (no backlight)...");
  lcdCmd(0x80);  //  1
  lcdPrint("HELLO WORLD");
  lcdCmd(0xC0);  //  2
  lcdPrint("NO BACKLIGHT");
  
  Serial.println("\n👁️  CHECK LCD NOW (no backlight):");
  Serial.println("   • Look from side angle");
  Serial.println("   • Use flashlight to illuminate");
  Serial.println("   • Should see DARK text on LIGHT background");
  Serial.println("\nWaiting 10 seconds...\n");
  delay(10000);
 
  Serial.println("Turning ON very dim backlight (level 30)...");
  setRGB(30, 30, 30);
  
  lcdCmd(0x01); delay(10);  // Clear
  lcdCmd(0x80);
  lcdPrint("HELLO WORLD");
  lcdCmd(0xC0);
  lcdPrint("DIM BACKLIGHT");
  
  Serial.println("Waiting 5 seconds...\n");
  delay(5000);
  
  
  Serial.println("Medium backlight (level 100)...");
  setRGB(100, 100, 100);
  
  lcdCmd(0x01); delay(10);
  lcdCmd(0x80);
  lcdPrint("HELLO WORLD");
  lcdCmd(0xC0);
  lcdPrint("MEDIUM LIGHT");
  
  Serial.println("Waiting 5 seconds...\n");
  delay(5000);
  
   
  Serial.println("Full backlight (level 255)...");
  setRGB(255, 255, 255);
  
  lcdCmd(0x01); delay(10);
  lcdCmd(0x80);
  lcdPrint("HELLO WORLD");
  lcdCmd(0xC0);
  lcdPrint("FULL BRIGHT");
  
  Serial.println("\n╔═══════════════════════════════════╗");
  Serial.println("║  Test Complete                    ║");
  Serial.println("╚═══════════════════════════════════╝\n");
  
  Serial.println("REPORT:");
  Serial.println("1. Did you see ANY text at ANY point?");
  Serial.println("2. Which backlight level worked best?");
  Serial.println("3. Are you using 3.3V or external 5V?\n");
  
  Serial.println("If NO text visible at all:");
  Serial.println("→ Module may be defective");
  Serial.println("→ Try external 5V power supply");
  Serial.println("→ Test with different Arduino board (UNO/Mega)");
}

void loop(){
  
  static bool toggle = false;
  toggle = !toggle;
  setRGB(toggle ? 255 : 0, 0, 0);
  delay(500);
}
