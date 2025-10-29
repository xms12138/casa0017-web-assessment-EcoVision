// MKR WiFi 1010 → MQTT → Vespera (Lumi)
// Music band + Heart Visuals (Valve Arcs default)
//


#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <rgb_lcd.h>
#include "MAX30105.h"
#include "heartRate.h"   // SparkFun beat detection helper
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/******** Config ********/
#define HEART_STYLE 4   // 1=Fireworks, 2=Spiral, 3=Comet, 4=Valve Arcs
#define DEBUG_LOG 1     // 0=quiet, 1=serial logs

/******** Wi-Fi / MQTT ********/

const char* ssid          = "";
const char* password      = " ";
//const char* ssid        = "";
//const char* password    = "";

const char* mqtt_username = " ";
const char* mqtt_password = " ";
const char* mqtt_server   = "mqtt.cetools.org";
const int   mqtt_port     = 1884;

// MQTT topic for Chrono Lumina
String lightId    = "6";
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

/******** LED layout & payload ********/
// Chrono Lumina is addressed as a 6 (rings) × 12 (columns) logical grid
const int NUM_RINGS = 6;
const int RING_SIZE = 12;
const int NUM_LEDS  = NUM_RINGS * RING_SIZE; // 72 pixels total
const int PAYLOAD_SIZE = NUM_LEDS * 3;       // RGB payload, 3 bytes per pixel
byte RGBpayload[PAYLOAD_SIZE];

// Map (ring, col)  
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

/******** Microphone (A2) — envelope + audio beat ********/
// Envelope drives the background "music band" layer (bottom→top brightness & hue)
const int PIN_MIC = A2;
float micMean=512.0f, micEnv=0.0f;
const float MIC_ALPHA_MEAN=0.01f;   // slow bias tracking
const float MIC_ALPHA_ENV =0.08f;   // envelope smoothing
float MIC_ENV_MAX=120.0f;           // auto-calibrated at startup

// Onset detector for audio beat  
float envSlow=0.0f;
const float ENV_SLOW_ALPHA=0.010f;
float onset=0.0f, onsetAvg=0.0f;
const float ONSET_ALPHA=0.08f;

unsigned long lastBeatMs=0;   // audio beat timestamp 
float beatIBI_ms = 600.0f;
bool  haveBeat=false;
const unsigned long BEAT_MIN_IBI = 220;
const unsigned long BEAT_MAX_GAP = 2000;

/******** Color map ********/
// Hue: blue → red; valve arcs use magenta/red focus
const float HUE_BLUE   = 240.0f;
const float HUE_RED    =   0.0f;
const float HUE_PURPLE = 285.0f;
const float SAT        =   1.0f;

const float QUIET_MIN_VAL = 0.58f; // ensure visibility at low volume
float BASE_V = 0.32f;              // base value for music band
float V_GAIN = 0.70f;              // value gain from volume

/******** Music band shaping ********/
const float BAND_SHARPNESS   = 2.4f; // radial falloff around "headMusic"
const float HUE_PULL_GAIN    = 0.70f; // stronger volume → hue pulled toward red
const float VAL_BOOST_BASE   = 0.90f;
const float VAL_BOOST_GAIN   = 0.60f;

const float FLASH_DECAY_MS   = 240.0f; // brief flash decay around audio beat
inline float flashBeat(unsigned long now){
  if(!haveBeat) return 0.0f;
  float dt = (float)(now - lastBeatMs); if(dt<0) dt=0;
  return expf(-dt/FLASH_DECAY_MS);
}

/******** Heart sensor (MAX30102/05) ********/
 
// and only enable a simple backup peak finder after long inactivity.
MAX30105 particleSensor;

bool heartSensorOK=false;
bool fingerOn=false;          // finger on sensor
bool haveHeart=false;         // a valid heart beat

float heartIBI_ms = 780.0f;
unsigned long lastHeartMs=0;          // last confirmed heart beat timestamp
unsigned long lastHeartDetectMs=0;    // most recent "valid heart beat" time
float bpmEMA = 75.0f;                  

 
uint8_t irAmp = 0x24; // start at 36; we adjust up/down slightly
const long IR_TARGET_MIN = 50000;
const long IR_TARGET_MAX = 90000;

// Finger presence thresholds & debounce timing  
long IR_PRESENT_THRESH = 9000;
long AC_PRESENT_THRESH = 200;
const unsigned long FINGER_ON_STABLE_MS  = 150;
const unsigned long FINGER_OFF_STABLE_MS = 600;
const unsigned long FINGER_LONG_MS       = 800;
unsigned long fingerOnCandSince  = 0;
unsigned long fingerOffCandSince = 0;
unsigned long fingerOnSince      = 0;

// Backup peak detector state (for fallback only)
float dcMean = 0.0f; // IR DC
float acEnv  = 0.0f; // IR envelope
float sPrev  = 0.0f;
bool  wasRising=false;

// AGC accumulators (update ~every 120 ms)
unsigned long agcLastMs=0;
long irAvgAcc=0; int irAvgCnt=0;

/******** Shared math helpers ********/
static inline float clamp01(float x){ return x<0?0:(x>1?1:x); }
float lerp(float a,float b,float t){ if(t<0)t=0; if(t>1)t=1; return a + (b-a)*t; }
inline float wrapDistCol(int c0,int c1){ int dc=abs(c0-c1); dc=min(dc, RING_SIZE-dc); return (float)dc; }

/*** Fireworks params ***/
const int   N_EMITTERS = 3;
int   eRing[N_EMITTERS];
int   eCol[N_EMITTERS];
const float COL_SCALE = (float)NUM_RINGS / (float)RING_SIZE;
const float EMIT_THICKNESS = 2.2f;
const float EMIT_V_BASE    = 0.85f;
const float EMIT_V_GAIN    = 0.50f;

/*** Spiral params ***/
const int   SP_TWIST = 2;
const float SP_SHARP = 1.6f;
const float SP_VBASE = 0.22f;
const float SP_VGAIN = 0.95f;
const float SP_COLLISION_GAIN = 0.8f; // brighter where two spirals collide
const float SP_RED_PULL = 0.50f;
const float HEART_AC_NORM = 12000.0f;

/*** Comet Columns params ***/
const int   COMET_COUNT = 3;
int   cometCol[COMET_COUNT];
float cometHeadR = 0.0f;
const float COMET_DECAY = 0.80f; // trail decay per frame
float cometTrail[NUM_RINGS][RING_SIZE];

/******** Networking & LCD ********/
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

rgb_lcd lcd;
bool lcdOK=false;
unsigned long lcdNextMs=0;

/******** Valve Arcs (heart) ********/
// Two half-ellipses (left & right “valves”) breathing with heart phase
int   ARC_LEFT_COL  = 3;
int   ARC_RIGHT_COL = 9;
const float ARC_COL_SPREAD = 3.0f;
const float ARC_THICK      = 6.0f;
const float ARC_SIDE_FALLOFF = 0.7f;
const float BULGE_MIN = 0.25f;
const float BULGE_MAX = 0.95f;
const float HEART_RED_PULL = 0.65f;
const float HEART_FLASH_DECAY_MS = 180.0f;

/******** Housekeeping ********/
unsigned long lastLogMs = 0;
const unsigned long LOG_INTERVAL = 250; // ~4 Hz logs

// Timeouts & retry pacing
const unsigned long HEART_LOSS_TIMEOUT_MS = 12000; // keep ghost animation
const unsigned long SENSOR_STALL_MS       = 1500;  // clear FIFO
const unsigned long SENSOR_REINIT_MS      = 3000;  // full reinit
const unsigned long WIFI_RETRY_MS         = 5000;
const unsigned long MQTT_RETRY_MS         = 3000;

// Inactivity cleanup (soft reset of state, not WiFi/MQTT)
const unsigned long INACTIVITY_RESET_MS = 60000;

// MQTT publish pacing
unsigned long lastPubMs = 0;
const unsigned long PUB_INTERVAL_MS = 40;

// Non-blocking retry schedulers
unsigned long nextWifiTryMs = 0;
unsigned long nextMqttTryMs = 0;

// Sensor watchdog
unsigned long lastSampleMs = 0;
unsigned long lastActivityMs = 0;

// Fallback peak detector is enabled only after long no-beat period
const unsigned long BACKUP_PEAK_ENABLE_MS = 8000;

/******** Utils ********/
void hsv2rgb(float h, float s, float v, byte &r, byte &g, byte &b){
  while(h<0) h+=360.0f; while(h>=360) h-=360.0f;
  float c=v*s, x=c*(1 - fabsf(fmodf(h/60.0f,2)-1)), m=v-c;
  float r1,g1,b1;
  if(h<60){r1=c;g1=x;b1=0;} else if(h<120){r1=x;g1=c;b1=0;}
  else if(h<180){r1=0;g1=c;b1=x;} else if(h<240){r1=0;g1=x;b1=c;}
  else if(h<300){r1=x;g1=0;b1=c;} else {r1=c;g1=0;b1=x;}
  r=(byte)roundf((r1+m)*255); g=(byte)roundf((g1+m)*255); b=(byte)roundf((b1+m)*255);
}

/******** Wi-Fi / MQTT — non-blocking connect/retry ********/
void startWifi(){
  if (WiFi.status() == WL_CONNECTED) return;
  unsigned long now = millis();
  if (now < nextWifiTryMs) return;
#if DEBUG_LOG
  if (Serial) Serial.println("WiFi try...");
#endif
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  nextWifiTryMs = now + WIFI_RETRY_MS;
}

void reconnectMQTT(){
  if (mqttClient.connected()) return;
  if (WiFi.status() != WL_CONNECTED) return;
  unsigned long now = millis();
  if (now < nextMqttTryMs) return;
#if DEBUG_LOG
  if (Serial) Serial.print("MQTT try...");
#endif
  String cid = "MKR1010-" + String(random(0xFFFF), HEX);
  if (mqttClient.connect(cid.c_str(), mqtt_username, mqtt_password)) {
#if DEBUG_LOG
    if (Serial) Serial.println("OK");
#endif
  } else {
#if DEBUG_LOG
    if (Serial) { Serial.print("fail rc="); Serial.println(mqttClient.state()); }
#endif
  }
  nextMqttTryMs = now + MQTT_RETRY_MS;
}

void callback(char*, byte*, unsigned int){}

/******** Microphone auto-calibration ********/
// Measure typical envelope for a short period and scale MIC_ENV_MAX accordingly.
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

/******** Audio beat detector (for music band flavor) ********/
bool detectBeat(unsigned long now){
  envSlow += (micEnv - envSlow)*ENV_SLOW_ALPHA;
  float rawOnset = micEnv - envSlow; if(rawOnset<0) rawOnset=0;
  onset     += (rawOnset - onset)*ONSET_ALPHA;
  onsetAvg  += (onset - onsetAvg)*0.02f;

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

/******** Heart sensor init & helpers ********/
void applyIrAmp(){ particleSensor.setPulseAmplitudeIR(irAmp); particleSensor.setPulseAmplitudeRed(0x00); }

void initHeartSensor(){
  Wire.begin();
  if(!particleSensor.begin(Wire, I2C_SPEED_FAST)){
#if DEBUG_LOG
    if (Serial) Serial.println("MAX3010x not found.");
#endif
    heartSensorOK=false; return;
  }
  // setup(ledBrightness, sampleAverage, ledMode(2=IR), sampleRate, pulseWidth, adcRange)
  particleSensor.setup(irAmp, 4, 2, 100, 411, 16384);
  applyIrAmp();
  particleSensor.clearFIFO();
  heartSensorOK=true;
#if DEBUG_LOG
  if (Serial) { Serial.print("MAX3010x OK | irAmp="); Serial.println(irAmp); }
#endif
}

void pickEmitters(){              // randomize Fireworks centers on each beat
  for(int i=0;i<N_EMITTERS;i++){
    eRing[i] = random(1, NUM_RINGS-1);
    eCol[i]  = random(0, RING_SIZE);
  }
}
void pickCometCols(){             // pick distinct columns for comets
  bool used[12]={0};
  for(int i=0;i<COMET_COUNT;i++){
    int c; do{ c=random(0,RING_SIZE);}while(used[c]);
    used[c]=true; cometCol[i]=c;
  }
}

/******** Core: read sensor + simple AGC + checkForBeat + fallback + recovery ********/
void updateHeartFromSensor(unsigned long now){
  if(!heartSensorOK){ fingerOn=false; haveHeart=false; return; }

  particleSensor.check();

  while(particleSensor.available()){
    long ir = particleSensor.getFIFOIR();

    lastSampleMs  = now;
    lastActivityMs= now;

    // Basic DC/AC tracking of IR for finger gating & fallback detector
    dcMean += (ir - dcMean) * 0.01f;
    float s  = (float)ir - dcMean;
    acEnv   += (fabsf(s) - acEnv) * 0.10f;

    // Finger gating with debounce (enter/exit clears old history)
    bool onCandidate  = (ir > IR_PRESENT_THRESH) && (acEnv > AC_PRESENT_THRESH);
    if(onCandidate){
      if(fingerOnCandSince==0) fingerOnCandSince = now;
      if(!fingerOn && (now - fingerOnCandSince) >= FINGER_ON_STABLE_MS){
        fingerOn = true; fingerOnSince = now;
        particleSensor.clearFIFO();      // clear history when finger arrives
        wasRising=false; sPrev=0.0f;     // reset fallback state
      }
      fingerOffCandSince = 0;
    }else{
      if(fingerOffCandSince==0) fingerOffCandSince = now;
      if(fingerOn && (now - fingerOffCandSince) >= FINGER_OFF_STABLE_MS){
        fingerOn = false; haveHeart = false; fingerOnSince = 0;
        particleSensor.clearFIFO();      // clear history when finger leaves
        wasRising=false; sPrev=0.0f;
      }
      fingerOnCandSince = 0;
    }

    // Very simple AGC: keep IR average around 50k–90k; avoid saturation
    irAvgAcc += ir; irAvgCnt++;
    if(now - agcLastMs > 120){
      long avg = irAvgCnt ? irAvgAcc/irAvgCnt : ir;
      irAvgAcc=0; irAvgCnt=0; agcLastMs=now;

      if(fingerOn){
        if(avg < IR_TARGET_MIN && irAmp < 0x7F){ irAmp += 2; applyIrAmp(); }
        else if((avg > IR_TARGET_MAX || acEnv > 20000) && irAmp > 0x08){ irAmp -= 2; applyIrAmp(); }
      }

#if DEBUG_LOG
      if(now - lastLogMs >= LOG_INTERVAL){
        if (Serial){
          Serial.print("IR:");  Serial.print(avg);
          Serial.print("\tAC:"); Serial.print((long)acEnv);
          Serial.print("\tfinger:"); Serial.println(fingerOn?1:0);
        }
        lastLogMs = now;
      }
#endif
    }

    // (1) Primary detector — SparkFun's checkForBeat (enable after finger stable 0.4s)
    bool got=false;
    if(fingerOn && (now - fingerOnSince) > 400){
      if(checkForBeat(ir)){
        unsigned long ibi = now - lastHeartMs;
        if(ibi>280 && ibi<2200){                    // reasonable IBI range
          heartIBI_ms = heartIBI_ms*0.6f + (float)ibi*0.4f;
          lastHeartMs = now;
          lastHeartDetectMs = now;
          haveHeart = true;
          float bpm = 60000.0f/heartIBI_ms;
          bpmEMA = bpmEMA*0.7f + bpm*0.3f;
          got = true;
#if DEBUG_LOG
          if(Serial){ Serial.print("Beat! BPM≈"); Serial.println(bpmEMA,1); }
#endif
        }
      }
    }

    // (2) Fallback peak detector — enabled only after long no-beat periods
    if(!got && fingerOn){
      if(lastHeartDetectMs==0 || (now - lastHeartDetectMs) > BACKUP_PEAK_ENABLE_MS){
        float th = 0.28f*acEnv + 25.0f;    
        float ds = s - sPrev;
        if(ds > 0) wasRising = true;
        bool peak=false;
        if(wasRising && ds<=0 && s>th && (now - lastHeartMs) > 260){
          peak=true; wasRising=false;
        }
        if(ds<0 && s<0) wasRising=false;

        if(peak){
          unsigned long ibi = now - lastHeartMs;
          lastHeartMs = now;
          if(ibi>280 && ibi<2200){
            heartIBI_ms = heartIBI_ms*0.6f + (float)ibi*0.4f;
            lastHeartDetectMs = now;
            haveHeart = true;
            float bpm = 60000.0f/heartIBI_ms;
            bpmEMA = bpmEMA*0.7f + bpm*0.3f;
#if DEBUG_LOG
            if(Serial){ Serial.print("BackupBeat BPM≈"); Serial.println(bpmEMA,1); }
#endif
          }
        }
      }
    }

    sPrev = s;
    particleSensor.nextSample();
  } // while available

  // Watchdog: short stall → clear FIFO; long stall → soft reinit
  // online
  if (lastSampleMs == 0) lastSampleMs = now;
  unsigned long stall = now - lastSampleMs;
  if (stall > SENSOR_STALL_MS) {
    particleSensor.clearFIFO();
#if DEBUG_LOG
    if (Serial) Serial.println("MAX3010x: FIFO cleared (stall)");
#endif
    if (stall > SENSOR_REINIT_MS) {
      particleSensor.shutDown(); delay(5); particleSensor.wakeUp();
      applyIrAmp(); particleSensor.clearFIFO();
      particleSensor.setup(irAmp, 4, 2, 100, 411, 16384);
#if DEBUG_LOG
      if (Serial) Serial.println("MAX3010x: reinit after long stall");
#endif
      lastSampleMs = now;
    }
  }

  // Soft cleanup after long inactivity (does not touch WiFi/MQTT)
  if (now - lastActivityMs > INACTIVITY_RESET_MS) {
    acEnv = 0.0f; envSlow = 0.0f; onset = 0.0f; onsetAvg = 0.0f;
    wasRising = false; sPrev = 0.0f;
    haveHeart = false; fingerOn = false;
    fingerOnCandSince = fingerOffCandSince = fingerOnSince = 0;
    particleSensor.clearFIFO();
    lastActivityMs = now;
#if DEBUG_LOG
    if (Serial) Serial.println("autoReset60s");
#endif
  }
}

/******** Heart phase (supports ghost phase) ********/
float heartPhase(unsigned long now){
  bool ghostOK = fingerOn && ((now - lastHeartDetectMs) <= HEART_LOSS_TIMEOUT_MS);
  if (!haveHeart && !ghostOK) return 0.0f;

  // Advance phase using last known IBI to keep animation continuous
  float ph = (float)(now - lastHeartMs) / heartIBI_ms;
  if (ph >= 1.0f){
    int k = (int)floorf(ph);
    lastHeartMs += (unsigned long)(k * heartIBI_ms);
    ph = fmodf(ph, 1.0f);
  }
  if (ph < 0) ph = 0;
  return ph;
}

/******** Render: heart styles ********/
// Fireworks: expanding rings from a few random emitters on every beat
void renderHeart_Fireworks(float baseHue, bool musicOn, unsigned long now){
  const float maxR = sqrtf((float)(NUM_RINGS*NUM_RINGS) + (0.5f*RING_SIZE*(float)NUM_RINGS/(float)RING_SIZE)*(0.5f*RING_SIZE*(float)NUM_RINGS/(float)RING_SIZE));
  float ph = heartPhase(now);
  float R  = ph * maxR;

  for(int r=0;r<NUM_RINGS;r++){
    for(int c=0;c<RING_SIZE;c++){
      float wSum=0;
      for(int k=0;k<N_EMITTERS;k++){
        float dr = (float)abs(r - eRing[k]);
        float dc = wrapDistCol(c, eCol[k]) * (float)NUM_RINGS / (float)RING_SIZE;
        float d  = sqrtf(dr*dr + dc*dc);
        float w  = expf(-EMIT_THICKNESS * (d - R)*(d - R));
        wSum += w;
      }
      if(wSum<=0.01f) continue;
      if(wSum>1.5f) wSum=1.5f;

      float vE  = clamp01(EMIT_V_BASE + EMIT_V_GAIN * wSum);
      float mix = musicOn ? clamp01(0.55f + 0.35f*flashBeat(now)) : 0.0f;
      float hE  = (1.0f - mix)*HUE_PURPLE + mix*baseHue;

      byte er,eg,eb; hsv2rgb(hE, SAT, vE, er,eg,eb);
      int i = pix(r,c);
      int rr = RGBpayload[i*3+0] + er; if(rr>255) rr=255;
      int gg = RGBpayload[i*3+1] + eg; if(gg>255) gg=255;
      int bb = RGBpayload[i*3+2] + eb; if(bb>255) bb=255;
      RGBpayload[i*3+0]=rr; RGBpayload[i*3+1]=gg; RGBpayload[i*3+2]=bb;
    }
  }
}

// Spiral: two counter-rotating arms; brighter/redder at collisions
void renderHeart_Spiral(float baseHue, bool musicOn, unsigned long now){
  float ph = heartPhase(now);
  float acN = clamp01(acEnv / HEART_AC_NORM);
  for(int r=0;r<NUM_RINGS;r++){
    for(int c=0;c<RING_SIZE;c++){
      float head1 = fmodf(ph*RING_SIZE + r*SP_TWIST, (float)RING_SIZE);
      float head2 = fmodf(head1 + RING_SIZE*0.5f, (float)RING_SIZE);
      float d1 = wrapDistCol(c, (int)roundf(head1));
      float d2 = wrapDistCol(c, (int)roundf(head2));

      float w1 = expf(-SP_SHARP * d1*d1);
      float w2 = expf(-SP_SHARP * d2*d2);
      float w  = w1 + w2;
      if(w < 0.01f) continue;

      float collide = w1 * w2;
      float collBoost = 1.0f + SP_COLLISION_GAIN * collide * (1.0f + flashBeat(now));

      float vE = clamp01(SP_VBASE + SP_VGAIN * w * (0.6f + 0.8f*acN));
      vE = clamp01(vE * collBoost);

      float mixMusic = musicOn ? clamp01(0.45f + 0.35f*acN) : 0.0f;
      float hE = (1.0f - mixMusic)*HUE_PURPLE + mixMusic*baseHue;
      hE = lerp(hE, HUE_RED, clamp01(SP_RED_PULL * collide));

      byte er,eg,eb; hsv2rgb(hE, SAT, vE, er,eg,eb);
      int i = pix(r,c);
      int rr = RGBpayload[i*3+0] + er; if(rr>255) rr=255;
      int gg = RGBpayload[i*3+1] + eg; if(gg>255) gg=255;
      int bb = RGBpayload[i*3+2] + eb; if(bb>255) bb=255;
      RGBpayload[i*3+0]=rr; RGBpayload[i*3+1]=gg; RGBpayload[i*3+2]=bb;
    }
  }
}

// Comet Columns: vertical moving heads with exponential trails
void renderHeart_Comet(float baseHue, bool musicOn, unsigned long now){
  float ph = heartPhase(now);
  float head = ph * (NUM_RINGS + 2.0f);

  for(int r=0;r<NUM_RINGS;r++)
    for(int c=0;c<RING_SIZE;c++)
      cometTrail[r][c] *= COMET_DECAY;

  for(int i=0;i<COMET_COUNT;i++){
    int c = cometCol[i];
    float rHead = head;
    int r0 = (int)floorf(rHead);
    int r1 = r0 + 1;
    float t  = rHead - r0;
    if(r0>=0 && r0<NUM_RINGS) cometTrail[r0][c] = max(cometTrail[r0][c], 1.0f - t);
    if(r1>=0 && r1<NUM_RINGS) cometTrail[r1][c] = max(cometTrail[r1][c], t);
  }

  for(int r=0;r<NUM_RINGS;r++){
    for(int c=0;c<RING_SIZE;c++){
      float w = cometTrail[r][c];
      if(w < 0.04f) continue;
      float mixMusic = musicOn ? 0.55f : 0.0f;
      float hE = (1.0f - mixMusic)*HUE_PURPLE + mixMusic*baseHue;
      float vE = clamp01(0.20f + 0.95f*w);
      byte er,eg,eb; hsv2rgb(hE, SAT, vE, er,eg,eb);
      int i = pix(r,c);
      int rr = RGBpayload[i*3+0] + er; if(rr>255) rr=255;
      int gg = RGBpayload[i*3+1] + eg; if(gg>255) gg=255;
      int bb = RGBpayload[i*3+2] + eb; if(bb>255) bb=255;
      RGBpayload[i*3+0]=rr; RGBpayload[i*3+1]=gg; RGBpayload[i*3+2]=bb;
    }
  }
}

/******** Valve Arcs (default heart style) ********/
inline float flashHeart(unsigned long now){
  float dt = (float)(now - lastHeartMs);
  if(dt<0) dt=0;
  return expf(-dt / HEART_FLASH_DECAY_MS);
}
void renderHeart_ValveArcs(float /*baseHue*/, bool /*musicOn*/, unsigned long now){
  bool ghostOK = fingerOn && ((now - lastHeartDetectMs) <= HEART_LOSS_TIMEOUT_MS);
  if(!(fingerOn && (haveHeart || ghostOK))) return;

  // Breathing half-ellipses: bulge grows/shrinks with heart phase
  float ph = heartPhase(now);
  float breath = 0.5f - 0.5f*cosf(2.0f*M_PI*ph);
  float bulge  = BULGE_MIN + (BULGE_MAX - BULGE_MIN) * breath;
  float acN    = clamp01(acEnv / HEART_AC_NORM);
  float flashH = flashHeart(now);

  float hueHeart = 330.0f; // magenta base
  hueHeart = lerp(hueHeart, HUE_RED, clamp01(HEART_RED_PULL*(0.5f*acN + 0.6f*flashH)));

  for(int r=0;r<NUM_RINGS;r++){
    float y = ((float)r - (NUM_RINGS-1)*0.5f) / ((NUM_RINGS-1)*0.5f);
    for(int c=0;c<RING_SIZE;c++){
      float dL = wrapDistCol(c, ARC_LEFT_COL);
      float dR = wrapDistCol(c, ARC_RIGHT_COL);
      float xL = dL / ARC_COL_SPREAD;
      float xR = dR / ARC_COL_SPREAD;

      float yArcL = (xL<1.0f) ? bulge * sqrtf(fmaxf(0.0f, 1.0f - xL*xL)) : 0.0f;
      float yArcR = (xR<1.0f) ? bulge * sqrtf(fmaxf(0.0f, 1.0f - xR*xR)) : 0.0f;

      float wL = (xL<1.0f) ? expf(-ARC_THICK * (y - yArcL)*(y - yArcL)) * expf(-ARC_SIDE_FALLOFF * xL*xL) : 0.0f;
      float wR = (xR<1.0f) ? expf(-ARC_THICK * (y - yArcR)*(y - yArcR)) * expf(-ARC_SIDE_FALLOFF * xR*xR) : 0.0f;

      float w = wL + wR;
      if(w < 0.02f) continue;
      if(w > 1.4f)  w = 1.4f;

      float vE = clamp01(0.22f + (0.95f * w) * (0.55f + 0.70f*acN) + 0.25f*flashH);

      byte er,eg,eb; hsv2rgb(hueHeart, 1.0f, vE, er,eg,eb);
      int i = pix(r,c);
      int rr = RGBpayload[i*3+0] + er; if(rr>255) rr=255;
      int gg = RGBpayload[i*3+1] + eg; if(gg>255) gg=255;
      int bb = RGBpayload[i*3+2] + eb; if(bb>255) bb=255;
      RGBpayload[i*3+0]=rr; RGBpayload[i*3+1]=gg; RGBpayload[i*3+2]=bb;
    }
  }
}

/******** LCD helpers ********/
// Backlight color reflects BPM ranges; pulses on each phase start
void lcdBeatBacklight(unsigned long now){
  if(!lcdOK) return;
  float bpm = bpmEMA;
  uint8_t r=0,g=0,b=0;
  if(bpm < 70){ r=0; g=60; b=140; }
  else if(bpm < 100){ r=0; g=100; b=80; }
  else if(bpm < 120){ r=100; g=120; b=0; }
  else { r=150; g=30; b=0; }

  float ph = heartPhase(now);
  bool showPulse = (ph<0.25f) && (haveHeart || (fingerOn && (now - lastHeartDetectMs)<=HEART_LOSS_TIMEOUT_MS));
  float pulse = showPulse ? (0.6f*(1.0f - (ph/0.25f))) : 0.0f;

  int rr = (int)(r*(1.0f+pulse)); if(rr>255) rr=255;
  int gg = (int)(g*(1.0f+pulse)); if(gg>255) gg=255;
  int bb = (int)(b*(1.0f+pulse)); if(bb>255) bb=255;
  lcd.setRGB(rr, gg, bb);
}

// Two-line status: prompt → reading → BPM/IBI or waiting
void lcdUpdate(unsigned long now){
  if(!lcdOK) return;
  if(now < lcdNextMs) return;
  lcdNextMs = now + 180;

  char line1[17], line2[17];
  bool longHold = fingerOn && (fingerOnSince>0) && (now - fingerOnSince >= FINGER_LONG_MS);

  if(!fingerOn){
    snprintf(line1, sizeof(line1), "Place finger   ");
    snprintf(line2, sizeof(line2), "and HOLD 1s    ");
    lcd.setRGB(20, 0, 40);
  }else if(fingerOn && !longHold){
    snprintf(line1, sizeof(line1), "Hold steady... ");
    snprintf(line2, sizeof(line2), "Reading...     ");
    lcd.setRGB(50, 50, 50);
  }else{
    bool ghostOK = (now - lastHeartDetectMs) <= HEART_LOSS_TIMEOUT_MS;
    if(haveHeart || ghostOK){
      snprintf(line1, sizeof(line1), "BPM~ %3d       ", (int)round(bpmEMA));
      snprintf(line2, sizeof(line2), "IBI:%4dms %s   ", (int)round(heartIBI_ms), haveHeart?"OK":"EST");
      lcdBeatBacklight(now);
    }else{
      snprintf(line1, sizeof(line1), "BPM: ---       ");
      snprintf(line2, sizeof(line2), "Waiting beats..");
      lcd.setRGB(60, 40, 0);
    }
  }

  lcd.setCursor(0,0); lcd.print(line1);
  lcd.setCursor(0,1); lcd.print(line2);
}

/******** Setup ********/
void setup(){
#if DEBUG_LOG
  Serial.begin(115200); delay(800);
  if (Serial) Serial.println("\nMusic Band + Valve Arcs | Simplified HR: checkForBeat + backup + simple AGC + watchdog");
#endif

  Wire.begin();
  lcd.begin(16, 2);
  lcd.setRGB(0, 80, 180);
  lcd.setCursor(0,0); lcd.print("Heart Visuals  ");
  lcd.setCursor(0,1); lcd.print("LCD Ready      ");
  lcdOK = true;

  startWifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);          // >= 216 bytes payload
  mqttClient.setCallback(callback);
  reconnectMQTT();

  memset(RGBpayload,0,PAYLOAD_SIZE);
  memset(cometTrail, 0, sizeof(cometTrail));

#if DEBUG_LOG
  if (Serial) Serial.println("Calibrating mic (2s)... Keep quiet");
#endif
  autoCalibrateMic(2000);
#if DEBUG_LOG
  if (Serial) { Serial.print("MIC_ENV_MAX="); Serial.println(MIC_ENV_MAX); }
#endif

  initHeartSensor();

  lastBeatMs   = millis();
  lastHeartMs  = millis();
  lastHeartDetectMs = 0;
  fingerOn=false; haveHeart=false; fingerOnSince=0;

  lastActivityMs = millis();  // start "activity" watchdog
  lastSampleMs   = millis();

  pickEmitters();
  pickCometCols();
}

/******** Loop ********/
void loop(){
  // Non-blocking connectivity servicing
  startWifi();
  reconnectMQTT();
  mqttClient.loop();

  unsigned long now = millis();

  // Microphone envelope for music band background
  int s = analogRead(PIN_MIC);
  micMean += (s - micMean)*MIC_ALPHA_MEAN;
  float ac=fabsf(s - micMean);
  micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;

  float vol  = clamp01(micEnv / MIC_ENV_MAX);
  float vmap = powf(fminf(vol * 1.6f, 1.0f), 0.7f);
  (void)detectBeat(now);  // optional: adds slight flashes to band

  // Heart sensor pipeline (simplified but robust)
  updateHeartFromSensor(now);

  // Music band base layer (hue/value shaped by volume; head slides up)
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

  // Heart overlay (ghost allowed for a short time after last beat)
  bool ghostOK = fingerOn && ((now - lastHeartDetectMs) <= HEART_LOSS_TIMEOUT_MS);
  if (fingerOn && (haveHeart || ghostOK)) {
    bool musicOn = (haveBeat || vmap > 0.15f);
#if HEART_STYLE==1
    renderHeart_Fireworks(baseHue, musicOn, now);
#elif HEART_STYLE==2
    renderHeart_Spiral(baseHue, musicOn, now);
#elif HEART_STYLE==3
    renderHeart_Comet(baseHue, musicOn, now);
#else
    renderHeart_ValveArcs(baseHue, musicOn, now);
#endif
  }

  // MQTT publish (rate-limited; only if connected)
  if(now - lastPubMs >= PUB_INTERVAL_MS && mqttClient.connected()){
    mqttClient.publish(mqtt_topic.c_str(), RGBpayload, PAYLOAD_SIZE);
    lastPubMs = now;
  }

  // LCD status / backlight
  lcdUpdate(now);

  delay(10); // small pacing; keep loop responsive
}
