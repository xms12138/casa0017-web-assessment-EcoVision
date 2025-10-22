// MKR WiFi 1010 → MQTT → Vespera (Lumi)
// PIR gated + Beat-locked band + Trail + Standby breathing
// Color map: Blue→Green→Yellow→Red (loudness→brighter & redder)
// Layout: 6 rings × 12 columns; index = ring + 6 * col  (ring: 0=bottom…5=top)

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/******** 1) Wi-Fi / MQTT (EDIT THESE) ********/
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

/******** 2) Topic (EDIT your id) ********/
String lightId    = "0"; //  Google Sheet 
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

/******** 3) Layout & payload ********/
const int NUM_RINGS = 6;        
const int RING_SIZE = 12;       
const int NUM_LEDS  = NUM_RINGS * RING_SIZE; // 72
const int PAYLOAD_SIZE = NUM_LEDS * 3;       // 216 bytes
byte RGBpayload[PAYLOAD_SIZE];

inline int pix(int ring, int col){
  col = (col % RING_SIZE + RING_SIZE) % RING_SIZE;
  if(ring<0) ring=0; if(ring>=NUM_RINGS) ring=NUM_RINGS-1;
  return ring + NUM_RINGS * col;
}
inline void setPixRGB(int i, byte r, byte g, byte b){
  if(i<0||i>=NUM_LEDS) return;
  RGBpayload[i*3+0]=r; RGBpayload[i*3+1]=g; RGBpayload[i*3+2]=b;
}
inline void setRingRGB(int ring, byte r, byte g, byte b){
  for(int c=0;c<RING_SIZE;c++) setPixRGB(pix(ring,c), r,g,b);
}
inline void fillAll(byte r, byte g, byte b){
  for(int i=0;i<NUM_LEDS;i++){ RGBpayload[i*3]=r; RGBpayload[i*3+1]=g; RGBpayload[i*3+2]=b; }
}

/******** 4) Sensors ********/
// MIC (A2)  
const int PIN_MIC = A2;
float micMean=512.0f, micEnv=0.0f;
const float MIC_ALPHA_MEAN=0.01f;    
const float MIC_ALPHA_ENV =0.08f;    
float MIC_ENV_MAX=120.0f;           

 
float envSlow=0.0f;
const float ENV_SLOW_ALPHA=0.010f;  // very slow baseline
float onset=0.0f, onsetAvg=0.0f;
const float ONSET_ALPHA=0.08f;

// beat logic
unsigned long lastBeatMs=0;
float beatIBI_ms = 600.0f;          // ~100 BPM
bool  haveBeat = false;
const unsigned long BEAT_MIN_IBI = 220;   // ms  
const unsigned long BEAT_MAX_GAP = 2000;   

// PIR (WPSE314) on D2
const int PIN_PIR = 2;
const unsigned long ACTIVE_HOLD_MS = 15000;  
unsigned long activeUntil = 0;

/******** 5) Color mapping (Blue→Green→Yellow→Red; louder→brighter) ********/
const float HUE_BLUE   = 240.0f; // blue
const float HUE_RED    =   0.0f; // red
const float SAT        =   1.0f;

const float QUIET_MIN_VAL = 0.58f; // bright  
float BASE_V = 0.32f;
float V_GAIN = 0.70f;

/******** 6) Beat-locked band + Trail ********/
 
const float BAND_SHARPNESS   = 2.4f;  
const float HUE_PULL_GAIN    = 0.70f;  
const float VAL_BOOST_BASE   = 0.90f;
const float VAL_BOOST_GAIN   = 0.60f;

// Trail:  (30 FPS近似)
float ringTrail[NUM_RINGS] = {0};
const float TRAIL_DECAY = 0.88f;   

// flash on beat  
const float FLASH_DECAY_MS   = 240.0f;
inline float beatFlash(unsigned long now){
  if(!haveBeat) return 0.0f;
  float dt = (float)(now - lastBeatMs);
  if(dt<0) dt=0;
  return expf(-dt / FLASH_DECAY_MS);
}

/******** ==============7) Standby breathing (but failed)================ ********/
const float STBY_HUE   = 45.0f;        
const float STBY_SAT   = 1.0f;
const float STBY_MIN_V = 0.04f;
const float STBY_AMP_V = 0.04f;       
const uint32_t STBY_PERIOD_MS = 3600; // breath

/******** 8) MQTT ********/
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/******** Utils ********/
static inline float clamp01(float x){ return x<0?0:(x>1?1:x); }
void hsv2rgb(float h, float s, float v, byte &r, byte &g, byte &b){
  float c=v*s, x=c*(1 - fabsf(fmodf(h/60.0f,2)-1)), m=v-c;
  float r1,g1,b1;
  if(h<60){r1=c;g1=x;b1=0;} else if(h<120){r1=x;g1=c;b1=0;}
  else if(h<180){r1=0;g1=c;b1=x;} else if(h<240){r1=0;g1=x;b1=c;}
  else if(h<300){r1=x;g1=0;b1=c;} else {r1=c;g1=0;b1=x;}
  r=(byte)roundf((r1+m)*255); g=(byte)roundf((g1+m)*255); b=(byte)roundf((b1+m)*255);
}

/******** Wi-Fi / MQTT helpers ********/
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

/******** Auto-calibrate MIC (quiet 2s) ********/
void autoCalibrateMic(uint16_t ms){
  unsigned long t0=millis(); micMean=512; micEnv=0; envSlow=0; onset=0; onsetAvg=0;
  float envMax=0;
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

/******** Beat detection ********/
bool detectBeat(unsigned long now){
  envSlow += (micEnv - envSlow)*ENV_SLOW_ALPHA;
  float rawOnset = micEnv - envSlow; if(rawOnset < 0) rawOnset = 0;
  onset += (rawOnset - onset)*ONSET_ALPHA;
  onsetAvg += (onset - onsetAvg)*0.02f;

  const float K = 1.6f, OFFSET = 2.0f;
  float th = onsetAvg * K + OFFSET;

  if(onset > th){
    unsigned long gap = now - lastBeatMs;
    if(gap > BEAT_MIN_IBI){
      float newIBI = (float)gap;
      newIBI = fminf(fmaxf(newIBI, 250.0f), 1500.0f); // 40–240 BPM
      beatIBI_ms = beatIBI_ms*0.65f + newIBI*0.35f;
      lastBeatMs = now;
      haveBeat = true;
      return true;
    }
  }
  if(haveBeat && (now - lastBeatMs) > BEAT_MAX_GAP) haveBeat = false;
  return false;
}

/******** Setup ********/
void setup(){
  Serial.begin(115200); delay(800);
  Serial.println("\nPIR-gated Beat band + Trail + Standby breathing");

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

  lastBeatMs = millis();
}

/******** Loop ********/
void loop(){
  if(WiFi.status()!=WL_CONNECTED) startWifi();
  if(!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  unsigned long now = millis();

  // --- PIR gating ---
  if(digitalRead(PIN_PIR) == HIGH) activeUntil = now + ACTIVE_HOLD_MS;
  bool active = (now < activeUntil);

  if(active){
    // --- MIC envelope update ---
    int s=analogRead(PIN_MIC);                 // 0..1023
    micMean += (s - micMean)*MIC_ALPHA_MEAN;   // remove DC slowly
    float ac=fabsf(s - micMean);
    micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;     

  
    float vol  = clamp01(micEnv / MIC_ENV_MAX);
    float vmap = powf(fminf(vol * 1.6f, 1.0f), 0.7f); // 0..1

    // beat detection
    (void)detectBeat(now);

    //   loudness: Blue(240) -> Red(0)
    float baseHue = HUE_BLUE + (HUE_RED - HUE_BLUE) * vmap; // 240→0
    float baseVal = clamp01(BASE_V + V_GAIN * vmap);
    if(baseVal < QUIET_MIN_VAL) baseVal = QUIET_MIN_VAL;

    // beat-locked sweep: within one beat, band travels 0..NUM_RINGS
    float sweepHead; // in rings
    if(haveBeat){
      float phase = fmodf((float)(now - lastBeatMs) / beatIBI_ms, 1.0f); // 0..1
      if(phase < 0) phase = 0;
      sweepHead = phase * NUM_RINGS;
    } else {
      // fallback when no beats: constant slow scroll
      static float fallbackHead=0;
      fallbackHead += 1.2f * (33.0f/1000.0f); // ~1.2 rings/sec
      while(fallbackHead >= NUM_RINGS) fallbackHead -= NUM_RINGS;
      sweepHead = fallbackHead;
    }

    float flash = beatFlash(now); // 0..1

    // --- Render with TRAIL per ring ---
    for(int r=0;r<NUM_RINGS;r++){
      float d = fabsf(r - sweepHead);
      d = fminf(d, NUM_RINGS - d);                
      float band = expf(-BAND_SHARPNESS * d * d); 

      // Trail:   ====but failed=======
      float energy = band * (1.0f + flash);      // 脉冲更亮
      ringTrail[r] = fmaxf(ringTrail[r] * TRAIL_DECAY, energy);

      // brightness: base * (boost_base + boost_gain * trail)
      float v2 = clamp01(baseVal * (VAL_BOOST_BASE + VAL_BOOST_GAIN * ringTrail[r]));

      // hue: trail  ====but failed=======
      float pull = HUE_PULL_GAIN * vmap * ringTrail[r];
      float h2 = baseHue * (1.0f - pull); if(h2 < 0) h2 = 0;

      byte rr,rg,rb; hsv2rgb(h2, SAT, v2, rr,rg,rb);
      setRingRGB(r, rr,rg,rb);
    }
  } else {
    // --- Standby breathing (very dim warm) ---
    float p = (now % STBY_PERIOD_MS) / (float)STBY_PERIOD_MS; // 0..1
    float breath = 0.5f + 0.5f * sinf(2.0f * M_PI * p);
    float v = STBY_MIN_V + STBY_AMP_V * breath;
    byte r,g,b; hsv2rgb(STBY_HUE, STBY_SAT, v, r,g,b);
    fillAll(r,g,b);

    // 慢慢清空 trail，回到活动态更自然=======but failed=======
    for(int r=0;r<NUM_RINGS;r++) ringTrail[r] *= 0.95f;
  }

   
  mqttClient.publish(mqtt_topic.c_str(), RGBpayload, PAYLOAD_SIZE);

  // ~30 FPS
  delay(33);

  // debug (1s)
  static unsigned long dbg=0;
  if(millis()-dbg>1000){
    dbg=millis();
    Serial.print("env=");Serial.print(micEnv);
    Serial.print(" vol=");Serial.print(clamp01(micEnv/MIC_ENV_MAX),2);
    if(haveBeat){
      float bpm = 60000.0f / beatIBI_ms;
      Serial.print("  BPM≈"); Serial.print(bpm,1);
    } else {
      Serial.print("  (no beat lock)");
    }
    Serial.println();
  }
}

