#include <Wire.h>
#include "MAX30105.h"

MAX30105 sensor;

 
uint8_t irAmp = 0x0C;         
long IR_PRESENT_THRESH = 5000;  //finger
const long IR_TARGET_MIN = 30000;
const long IR_TARGET_MAX = 120000;

unsigned long agcLastMs=0; long irAvg=0; int irCount=0;

float dcMean=0.0f;
const float DC_ALPHA=0.01f;
float acEnv=0.0f;
const float ENV_ALPHA=0.10f;

float sPrev=0.0f;
unsigned long lastBeatMs=0;
float bpmEMA=75.0f;

void applyAmp(){ sensor.setPulseAmplitudeIR(irAmp); sensor.setPulseAmplitudeRed(0x00); }

void setup(){
  Serial.begin(115200);
  while(!Serial){}
  Serial.println("\nMAX30102 peak detector (with finger/amp/Beat!)");

  Wire.begin();
  if(!sensor.begin(Wire, I2C_SPEED_FAST)){ Serial.println("Sensor not found"); while(1){} }

  // ledBrightness(=irAmp), sampleAverage, ledMode(2=IR only), sampleRate, pulseWidth(411us), adcRange
  sensor.setup(irAmp, 8, 2, 100, 411, 16384);
  applyAmp();
  sensor.clearFIFO();

  Serial.println("Cover sensor. Light finger touch. Open Serial Plotter.");
}

void loop(){
  sensor.check();
  while(sensor.available()){
    long ir = sensor.getFIFOIR();
    bool finger = (ir > IR_PRESENT_THRESH);

    // DC/AC
    dcMean += (ir - dcMean) * DC_ALPHA;
    float s  = (float)ir - dcMean;         
    acEnv   += (fabsf(s) - acEnv) * ENV_ALPHA;

    // (  0.45/120）
    float th = 0.45f * acEnv + 120.0f;

    //  
    float ds = s - sPrev;
    static bool wasRising=false;
    if(ds > 0) wasRising = true;
    bool peak=false;
    if(wasRising && ds<=0 && s>th && (millis()-lastBeatMs)>300){
      peak=true; wasRising=false;
    }
    if(ds<0 && s<0) wasRising=false;

    if(finger && peak){
      unsigned long ibi = millis()-lastBeatMs;
      lastBeatMs = millis();
      if(ibi>300 && ibi<2000){
        float bpm = 60000.0f / ibi;
        bpmEMA = bpmEMA*0.7f + bpm*0.3f;
        Serial.print("Beat! BPM≈"); Serial.println(bpmEMA,1);
      }
    }

    // AGC：120ms  
    irAvg += ir; irCount++;
    if(millis()-agcLastMs>120){
      long avg = (irCount? irAvg/irCount : ir);
      irAvg=0; irCount=0; agcLastMs=millis();
      if(finger){
        if(avg>240000 && irAmp>0x04){ irAmp-=4; applyAmp(); }
        else if(avg>IR_TARGET_MAX && irAmp>0x03){ irAmp-=2; applyAmp(); }
        else if(avg<IR_TARGET_MIN && irAmp<0x7F){ irAmp+=2; applyAmp(); }
      }
    }

    //  putuot
    static unsigned long t=0;
    if(millis()-t>20){
      t=millis();
      Serial.print("IR:");  Serial.print(ir);
      Serial.print("\tAC:"); Serial.print((long)acEnv);
      Serial.print("\tTH:"); Serial.print((long)th);
      Serial.print("\tamp:");Serial.print(irAmp);
      Serial.print("\tfinger:"); Serial.println(finger?1:0);
    }

    sPrev=s;
    sensor.nextSample();
  }
}
