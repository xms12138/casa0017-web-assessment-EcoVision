// MKR WiFi 1010 → MQTT → Vespera (Lumi)
// Music band (bottom→top) + Heart "Fireworks" (ONLY when finger is on sensor & real beats detected)
// Robust HR detection: DC removal + envelope + dynamic threshold + slope + refractory + IR AGC.

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30105.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/******** 1) Wi-Fi / MQTT ********/
#define SECRET_SSID      " "
#define SECRET_PASS      " "
#define SECRET_MQTTUSER  " "
#define SECRET_MQTTPASS  " "

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int   mqtt_port     = 1884;

/******** 2) Topic ********/
String lightId    = "6"; // ← Google Sheet id
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

/******** 3)  payload ********/
const int NUM_RINGS = 6;     //  
const int RING_SIZE = 12;    //  
const int NUM_LEDS  = NUM_RINGS * RING_SIZE; // 72
const int PAYLOAD_SIZE = NUM_LEDS * 3;       // 216 bytes
byte RGBpayload[PAYLOAD_SIZE];

inline int pix(int ring, int col){
  col = (col % RING_SIZE + RING_SIZE) % RING_SIZE;
  if(ring<0) ring=0; if(ring>=NUM_RINGS) ring=NUM_RINGS-1;
  return ring + NUM_RINGS * col;
}
inline void setRingRGB(int ring, byte r, byte g, byte b){
  for(int c=0;c<RING_SIZE;c++){
    int i = pix(ring,c);
    RGBpayload[i*3+0]=r; RGBpayload[i*3+1]=g; RGBpayload[i*3+2]=b;
  }
}
inline void fillAll(byte r, byte g, byte b){
  for(int i=0;i<NUM_LEDS;i++){ RGBpayload[i*3]=r; RGBpayload[i*3+1]=g; RGBpayload[i*3+2]=b; }
}

/******** 4) MIC (A2) envelope + audio beat  ********/
const int PIN_MIC = A2;
float micMean=512.0f, micEnv=0.0f;
const float MIC_ALPHA_MEAN=0.01f;
const float MIC_ALPHA_ENV =0.08f;
float MIC_ENV_MAX=120.0f;

float envSlow=0.0f;
const float ENV_SLOW_ALPHA=0.010f;
float onset=0.0f, onsetAvg=0.0f;
const float ONSET_ALPHA=0.08f;

unsigned long lastBeatMs=0;
float beatIBI_ms = 600.0f;
bool  haveBeat=false;
const unsigned long BEAT_MIN_IBI = 220;
const unsigned long BEAT_MAX_GAP = 2000;

/******** 5) PIR ********/
const int PIN_PIR = 2;
const unsigned long ACTIVE_HOLD_MS = 15000;
unsigned long activeUntil = 0;

/******** 6) 色彩（Blue→Red；越响越亮） ********/
const float HUE_BLUE = 240.0f;
const float HUE_RED  =   0.0f;
const float SAT      =   1.0f;

const float QUIET_MIN_VAL = 0.58f;
float BASE_V = 0.32f;
float V_GAIN = 0.70f;

/******** 7) 音乐带参数 ********/
const float BAND_SHARPNESS   = 2.4f;
const float HUE_PULL_GAIN    = 0.70f;
const float VAL_BOOST_BASE   = 0.90f;
const float VAL_BOOST_GAIN   = 0.60f;

const float FLASH_DECAY_MS   = 240.0f;
inline float flashBeat(unsigned long now){
  if(!haveBeat) return 0.0f;
  float dt = (float)(now - lastBeatMs); if(dt<0) dt=0;
  return expf(-dt/FLASH_DECAY_MS);
}

/******** 8) Heart sensor (MAX30102) + robust peak detection + fireworks ********/
MAX30105 particleSensor;

bool heartSensorOK=false;
bool fingerOn=false;
bool haveHeart=false;

unsigned long fingerStateChangeMs=0;

float heartIBI_ms = 780.0f;
unsigned long lastHeartMs=0;         //  
unsigned long lastHeartDetectMs=0;   //  

// IR LED  
uint8_t irAmp = 0x18;                //  
const long IR_TARGET_MIN = 30000;    //  
const long IR_TARGET_MAX = 120000;

//  8000 
long IR_PRESENT_THRESH = 8000;
const unsigned long FINGER_OFF_HYST_MS = 500;

//  
float dcMean = 0.0f;   // DC
const float DC_ALPHA = 0.01f;
float acEnv  = 0.0f;   // 包络
const float ENV_ALPHA = 0.10f;
float sPrev  = 0.0f;   //  AC
bool  wasRising=false; //  
float bpmEMA = 75.0f;  //  

// AGC  
unsigned long agcLastMs=0;
long irAvgAcc=0; int irAvgCnt=0;

//  
const float HUE_PURPLE = 285.0f;
const int   N_EMITTERS = 3;
int   eRing[N_EMITTERS];
int   eCol[N_EMITTERS];

const float COL_SCALE = (float)NUM_RINGS / (float)RING_SIZE; // 0.5
const float EMIT_THICKNESS = 2.2f;
const float EMIT_V_BASE    = 0.85f;
const float EMIT_V_GAIN    = 0.50f;

inline float wrapColDist(int c0, int c1){
  int dc = abs(c0 - c1);
  dc = min(dc, RING_SIZE - dc);
  return (float)dc * COL_SCALE;
}
inline float gridDist(int r0,int c0,int r1,int c1){
  float dr = (float)abs(r0 - r1);
  float dc = wrapColDist(c0, c1);
  return sqrtf(dr*dr + dc*dc);
}
void pickEmitters(){
  for(int i=0;i<N_EMITTERS;i++){
    eRing[i] = random(1, NUM_RINGS-1);
    eCol[i]  = random(0, RING_SIZE);
  }
}
float heartPhase(unsigned long now){
  if(!haveHeart) return 0.0f;
  float ph = (float)(now - lastHeartMs) / heartIBI_ms;
  if(ph >= 1.0f){
    int k = (int)floorf(ph);
    lastHeartMs += (unsigned long)(k * heartIBI_ms);
    pickEmitters();  //  
    ph = fmodf(ph, 1.0f);
  }
  if(ph < 0) ph = 0;
  return ph;
}

/******** 9) MQTT ********/
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/******** utils ********/
static inline float clamp01(float x){ return x<0?0:(x>1?1:x); }
void hsv2rgb(float h, float s, float v, byte &r, byte &g, byte &b){
  while(h<0) h+=360.0f; while(h>=360) h-=360.0f;
  float c=v*s, x=c*(1 - fabsf(fmodf(h/60.0f,2)-1)), m=v-c;
  float r1,g1,b1;
  if(h<60){r1=c;g1=x;b1=0;} else if(h<120){r1=x;g1=c;b1=0;}
  else if(h<180){r1=0;g1=c;b1=x;} else if(h<240){r1=0;g1=x;b1=c;}
  else if(h<300){r1=x;g1=0;b1=c;} else {r1=c;g1=0;b1=x;}
  r=(byte)roundf((r1+m)*255); g=(byte)roundf((g1+m)*255); b=(byte)roundf((b1+m)*255);
}

/******** Wi-Fi / MQTT ********/
void startWifi(){
  if(WiFi.status()==WL_CONNECTED) return;
  Serial.print("Connecting WiFi: "); Serial.println(ssid);
  WiFi.disconnect(); WiFi.begin(ssid, password);
  unsigned long t0=millis(); while(WiFi.status()!=WL_CONNECTED && millis()-t0<20000){ delay(300); Serial.print("."); }
  Serial.println(WiFi.status()==WL_CONNECTED? "\nWiFi OK":"\nWiFi FAIL");
}
void reconnectMQTT(){
  while(!mqttClient.connected() && WiFi.status()==WL_CONNECTED){
    Serial.print("MQTT connecting...");
    String cid="MKR1010-"+String(random(0xFFFF),HEX);
    if(mqttClient.connect(cid.c_str(), mqtt_username, mqtt_password)){ Serial.println("connected"); }
    else { Serial.print("failed rc="); Serial.println(mqttClient.state()); delay(1200); }
  }
}
void callback(char*, byte*, unsigned int){}

/******** MIC auto-cal ********/
void autoCalibrateMic(uint16_t ms){
  unsigned long t0=millis(); micMean=512; micEnv=0; envSlow=0; onset=0; onsetAvg=0; float envMax=0;
  while(millis()-t0<ms){
    int s=analogRead(PIN_MIC);
    micMean += (s - micMean)*MIC_ALPHA_MEAN;
    float ac=fabsf(s - micMean);
    micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;
    if(micEnv>envMax) envMax=micEnv;
    envSlow += (micEnv - envSlow)*ENV_SLOW_ALPHA;
    delay(5);
  }
  MIC_ENV_MAX = constrain(envMax*8.0f, 60.0f, 300.0f);
}

/******** Audio beat（原来那套） ********/
bool detectBeat(unsigned long now){
  envSlow += (micEnv - envSlow)*ENV_SLOW_ALPHA;
  float rawOnset = micEnv - envSlow; if(rawOnset<0) rawOnset=0;
  onset += (rawOnset - onset)*ONSET_ALPHA;
  onsetAvg += (onset - onsetAvg)*0.02f;

  const float K=1.6f, OFFSET=2.0f;
  float th = onsetAvg*K + OFFSET;

  if(onset > th){
    unsigned long gap = now - lastBeatMs;
    if(gap > BEAT_MIN_IBI){
      float newIBI=(float)gap;
      newIBI = fminf(fmaxf(newIBI,250.0f),1500.0f);
      beatIBI_ms = beatIBI_ms*0.65f + newIBI*0.35f;
      lastBeatMs=now; haveBeat=true; return true;
    }
  }
  if(haveBeat && (now - lastBeatMs) > BEAT_MAX_GAP) haveBeat=false;
  return false;
}

/******** Heart init & update  ********/
void applyIrAmp(){ particleSensor.setPulseAmplitudeIR(irAmp); particleSensor.setPulseAmplitudeRed(0x00); }

void initHeartSensor(){
  Wire.begin();
  if(!particleSensor.begin(Wire, I2C_SPEED_FAST)){
    Serial.println("MAX3010x not found.");
    heartSensorOK=false; return;
  }
  // ledBrightness(=irAmp), sampleAverage, ledMode(2=IR only), sampleRate, pulseWidth, adcRange
  particleSensor.setup(irAmp, 8, 2, 100, 411, 16384);
  applyIrAmp();
  particleSensor.clearFIFO();
  heartSensorOK=true;
  Serial.println("MAX3010x OK.");
}

void updateHeartFromSensor(unsigned long now){
  if(!heartSensorOK){ fingerOn=false; haveHeart=false; return; }

  particleSensor.check();
  while(particleSensor.available()){
    long ir = particleSensor.getFIFOIR();

    //  
    bool nowFinger = (ir > IR_PRESENT_THRESH);
    if(nowFinger != fingerOn){ fingerOn = nowFinger; fingerStateChangeMs = now; }
    if(!fingerOn && (now - fingerStateChangeMs) > FINGER_OFF_HYST_MS){
      haveHeart = false;  
    }
    if(!fingerOn){  
      // FIFO  
      particleSensor.nextSample();
      continue;
    }

    // DC / AC  
    dcMean += (ir - dcMean) * DC_ALPHA;
    float s  = (float)ir - dcMean;
    acEnv   += (fabsf(s) - acEnv) * ENV_ALPHA;

    //  0.55 / +160）
    float th = 0.45f * acEnv + 120.0f;

    //  
    float ds = s - sPrev;
    if(ds > 0) wasRising = true;
    bool peak=false;
    if(wasRising && ds<=0 && s>th && (now - lastHeartMs) > 300){
      peak=true; wasRising=false;
    }
    if(ds<0 && s<0) wasRising=false;

    if(peak){
      unsigned long ibi = now - lastHeartMs;
      lastHeartMs = now;
      if(ibi>300 && ibi<2000){
        heartIBI_ms = heartIBI_ms*0.6f + (float)ibi*0.4f;
        lastHeartDetectMs = now;
        if(!haveHeart) haveHeart = true;
        pickEmitters();
        //  BPM
        float bpm = 60000.0f / heartIBI_ms;
        bpmEMA = bpmEMA*0.7f + bpm*0.3f;
        Serial.print("Beat! BPM≈"); Serial.println(bpmEMA,1);
      }
    }else{
      if(haveHeart && (now - lastHeartDetectMs) > 5000){
        haveHeart = false;  
      }
    }

    // —— AGC： 
    irAvgAcc += ir; irAvgCnt++;
    if(now - agcLastMs > 120){
      long avg = (irAvgCnt ? irAvgAcc/irAvgCnt : ir);
      irAvgAcc=0; irAvgCnt=0; agcLastMs=now;
      if(fingerOn){
        if(avg>240000 && irAmp>0x04){ irAmp-=4; applyIrAmp(); }
        else if(avg>IR_TARGET_MAX && irAmp>0x03){ irAmp-=2; applyIrAmp(); }
        else if(avg<IR_TARGET_MIN && irAmp<0x7F){ irAmp+=2; applyIrAmp(); }
      }
      //  
      Serial.print("IR:");  Serial.print(avg);
      Serial.print("\tAC:"); Serial.print((long)acEnv);
      Serial.print("\tTH:"); Serial.print((long)th);
      Serial.print("\tamp:");Serial.print(irAmp);
      Serial.print("\tfinger:"); Serial.println(fingerOn?1:0);
    }

    sPrev = s;
    particleSensor.nextSample();
  }
}

/******** Setup ********/
void setup(){
  Serial.begin(115200); delay(800);
  Serial.println("\nMusic Band + Heart Fireworks (finger-gated, robust HR)");

  pinMode(PIN_PIR, INPUT);
  startWifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setCallback(callback);
  reconnectMQTT();

  memset(RGBpayload,0,PAYLOAD_SIZE);
  Serial.println("Calibrating mic (2s)... Keep quiet");
  autoCalibrateMic(2000);
  Serial.print("MIC_ENV_MAX="); Serial.println(MIC_ENV_MAX);

  initHeartSensor();

  lastBeatMs   = millis();
  lastHeartMs  = millis();
  lastHeartDetectMs = millis();
  fingerOn=false; haveHeart=false;
  pickEmitters();
}

/******** Loop ********/
void loop(){
  if(WiFi.status()!=WL_CONNECTED) startWifi();
  if(!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  unsigned long now = millis();

  // PIR 门控
  if(digitalRead(PIN_PIR)==HIGH) activeUntil = now + ACTIVE_HOLD_MS;
  bool active = (now < activeUntil);

  if(active){
    // —— 声音包络
    int s = analogRead(PIN_MIC);
    micMean += (s - micMean)*MIC_ALPHA_MEAN;
    float ac=fabsf(s - micMean);
    micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;

    float vol  = clamp01(micEnv / MIC_ENV_MAX);
    float vmap = powf(fminf(vol * 1.6f, 1.0f), 0.7f);
    (void)detectBeat(now);

    // —— 心跳更新（只有手指在位才会触发 haveHeart）
    updateHeartFromSensor(now);

    // —— 音乐底图（下→上）
    float baseHue = HUE_BLUE + (HUE_RED - HUE_BLUE) * vmap;
    float baseVal = clamp01(BASE_V + V_GAIN * vmap);
    if(baseVal < QUIET_MIN_VAL) baseVal = QUIET_MIN_VAL;

    float headMusic;
    if(haveBeat){
      float phase = fmodf((float)(now - lastBeatMs) / beatIBI_ms, 1.0f);
      if(phase < 0) phase=0;
      headMusic = phase * NUM_RINGS;
    }else{
      static float fm=0; fm += 1.2f*(33.0f/1000.0f); while(fm>=NUM_RINGS) fm-=NUM_RINGS; headMusic=fm;
    }
    float fBeat = flashBeat(now);

    memset(RGBpayload, 0, PAYLOAD_SIZE);
    for(int r=0;r<NUM_RINGS;r++){
      float d = fabsf(r - headMusic); d = fminf(d, NUM_RINGS - d);
      float wM = expf(-BAND_SHARPNESS * d * d);
      float v2 = clamp01(baseVal * (VAL_BOOST_BASE + VAL_BOOST_GAIN * (wM * (1.0f + fBeat))));
      float pull = HUE_PULL_GAIN * vmap * (wM * (1.0f + fBeat));
      float h2 = baseHue * (1.0f - pull); if(h2<0) h2=0;
      byte rr,rg,rb; hsv2rgb(h2, SAT, v2, rr,rg,rb);
      setRingRGB(r, rr,rg,rb);
    }

    // —— 叠加心跳“烟花环”，仅 fingerOn && haveHeart
    if(fingerOn && haveHeart){
      const float MAX_R = sqrtf((float)(NUM_RINGS*NUM_RINGS) + (COL_SCALE*RING_SIZE*0.5f)*(COL_SCALE*RING_SIZE*0.5f));
      float ph = heartPhase(now);
      float R  = ph * MAX_R;

      bool musicOn = (haveBeat || vmap > 0.15f);
      for(int r=0;r<NUM_RINGS;r++){
        for(int c=0;c<RING_SIZE;c++){
          float wSum = 0.0f;
          for(int k=0;k<N_EMITTERS;k++){
            float d = gridDist(r,c, eRing[k], eCol[k]);
            float w = expf(-EMIT_THICKNESS * (d - R)*(d - R));
            wSum += w;
          }
          if(wSum > 1.5f) wSum = 1.5f;
          if(wSum > 0.01f){
            float mix = musicOn ? clamp01(0.55f*vmap + 0.35f*flashBeat(now)) : 0.0f;
            float hE  = (1.0f - mix)*HUE_PURPLE + mix*baseHue;
            float vE  = clamp01(EMIT_V_BASE + EMIT_V_GAIN * wSum);
            byte er,eg,eb; hsv2rgb(hE, SAT, vE, er,eg,eb);
            int i = pix(r,c);
            int rr = RGBpayload[i*3+0] + er; if(rr>255) rr=255;
            int gg = RGBpayload[i*3+1] + eg; if(gg>255) gg=255;
            int bb = RGBpayload[i*3+2] + eb; if(bb>255) bb=255;
            RGBpayload[i*3+0] = (byte)rr;
            RGBpayload[i*3+1] = (byte)gg;
            RGBpayload[i*3+2] = (byte)bb;
          }
        }
      }
    }

  } else {
    fillAll(0,0,0);
  }

  mqttClient.publish(mqtt_topic.c_str(), RGBpayload, PAYLOAD_SIZE);
  delay(33);

   
  static unsigned long dbg=0;
  if(millis()-dbg>1000){
    dbg=millis();
    Serial.print("active="); Serial.print((millis()<activeUntil));
    Serial.print("  vol="); Serial.print(clamp01(micEnv/MIC_ENV_MAX),2);
    if(haveBeat){ float bpm=60000.0f/beatIBI_ms; Serial.print("  music BPM≈"); Serial.print(bpm,1); }
    else { Serial.print("  music(no lock)"); }
    Serial.print("  fingerOn="); Serial.print(fingerOn);
    if(fingerOn && haveHeart){ float hbpm=60000.0f/heartIBI_ms; Serial.print("  heart BPM≈"); Serial.print(hbpm,1); }
    else if(fingerOn){ Serial.print("  heart(waiting)"); }
    else { Serial.print("  heart(off)"); }
    Serial.println();
  }
}

