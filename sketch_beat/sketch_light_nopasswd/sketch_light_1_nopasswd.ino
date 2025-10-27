// MKR WiFi 1010 → MQTT → Vespera(Lumi)
// 6 × 12; row (ring) index 0..5 = bottom → top; column (azimuth) index 0..11
// Display modes: 
//   1 = Volume bar (bottom to top) 
//   2 = Ring scan (speed follows volume) 
//   3 = "Beat" ripple (upward from bottom)
#define MODE 1

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <math.h>

/******** Wi-Fi / MQTT ********/
#define SECRET_SSID      " "                 
#define SECRET_PASS      " " 
#define SECRET_MQTTUSER  " "                        
#define SECRET_MQTTPASS  " "       

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int   mqtt_port     = 1884;/Users/yimisheng/Downloads/CASA 2025.pptx /Users/yimisheng/Downloads/CASA 2025 Feedback for both samples.docx /Users/yimisheng/Downloads/BLOG SAMPLE B.pdf /Users/yimisheng/Downloads/BLOG SAMPLE A.pdf

 
String lightId    = "0"; // Google Sheet  
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

/******** look like ********/
const int NUM_RINGS   = 6;      
const int RING_SIZE   = 12;     
const int NUM_LEDS    = NUM_RINGS * RING_SIZE; // 72
const int PAYLOAD_SIZE= NUM_LEDS * 3;          // 216
byte RGBpayload[PAYLOAD_SIZE];

// ring(0..5), col(0..11) 
// index = ring + NUM_RINGS * col
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

/******** sound（A2）→ sound envelope ********/
const int PIN_MIC = A2;
float micMean=512.0f, micEnv=0.0f;
const float MIC_ALPHA_MEAN=0.01f;
const float MIC_ALPHA_ENV =0.08f;
float MIC_ENV_MAX=120.0f; //  

/******** color（yellow→red；loudness） ********/
const float HUE_YELLOW=60.0f;
const float HUE_RED   =0.0f;
const float SAT       =1.0f;
float BASE_V=0.38f, V_GAIN=0.70f; //  
const float MIN_VAL=0.35f;        // light 1

/******** MQTT ********/
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

/******** tools ********/
static inline float clamp01(float x){ return x<0?0:(x>1?1:x); }
void hsv2rgb(float h, float s, float v, byte &r, byte &g, byte &b){
  float c=v*s, x=c*(1 - fabsf(fmodf(h/60.0f,2)-1)), m=v-c;
  float r1,g1,b1;
  if(h<60){r1=c;g1=x;b1=0;} else if(h<120){r1=x;g1=c;b1=0;}
  else if(h<180){r1=0;g1=c;b1=x;} else if(h<240){r1=0;g1=x;b1=c;}
  else if(h<300){r1=x;g1=0;b1=c;} else {r1=c;g1=0;b1=x;}
  r=(byte)roundf((r1+m)*255); g=(byte)roundf((g1+m)*255); b=(byte)roundf((b1+m)*255);
}
void startWifi(){
  if(WiFi.status()==WL_CONNECTED) return;
  Serial.print("Connecting WiFi: "); Serial.println(ssid);
  WiFi.disconnect(); WiFi.begin(ssid,password);
  unsigned long t0=millis(); while(WiFi.status()!=WL_CONNECTED && millis()-t0<20000){ delay(300); Serial.print("."); }
  Serial.println(WiFi.status()==WL_CONNECTED ? "\nWiFi OK" : "\nWiFi FAIL");
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

/******** 自校准（安静 2s） ********/
void autoCalibrateMic(uint16_t ms){
  unsigned long t0=millis(); micMean=512; micEnv=0; float envMax=0;
  while(millis()-t0<ms){
    int s=analogRead(PIN_MIC);
    micMean += (s - micMean)*MIC_ALPHA_MEAN;
    float ac=fabsf(s - micMean);
    micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;
    if(micEnv>envMax) envMax=micEnv;
    delay(5);
  }
  MIC_ENV_MAX = constrain(envMax*8.0f, 60.0f, 300.0f);
}

/*********   *********/
static float sweepPos=0.0f;      // MODE2  
static float ringHead=0.0f;      // MODE2  
static unsigned long lastBeat=0; // MODE3  
static bool rippleActive=false;  // MODE3  
static unsigned long rippleT0=0; // MODE3  

/********* SETUP *********/
void setup(){
  Serial.begin(115200); delay(800);
  Serial.println("\nVespera Ring Mapper — MODE "
#if MODE==1
    "1: VU Rings"
#elif MODE==2
    "2: Ring Sweep"
#else
    "3: Beat Ripple"
#endif
  );

  startWifi();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setCallback(callback);
  reconnectMQTT();

  memset(RGBpayload,0,PAYLOAD_SIZE);
  Serial.println("Calibrating mic (2s)... Keep quiet");
  autoCalibrateMic(2000);
  Serial.print("MIC_ENV_MAX="); Serial.println(MIC_ENV_MAX);
}

/********* LOOP *********/
void loop(){
  if(WiFi.status()!=WL_CONNECTED) startWifi();
  if(!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  // --- sound envelope ---
  int s=analogRead(PIN_MIC);
  micMean += (s - micMean)*MIC_ALPHA_MEAN;
  float ac=fabsf(s - micMean);
  micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;
  float vol = clamp01(micEnv / MIC_ENV_MAX);

  // Hue & brightness (base color)
  float hue = HUE_YELLOW + (HUE_RED - HUE_YELLOW) * vol; // 60 -> 0
  float val = clamp01(BASE_V + V_GAIN * vol);
  if(val<MIN_VAL) val=MIN_VAL;

  // background: default cleared to base color
  byte br,bg,bb; hsv2rgb(hue, SAT, val, br,bg,bb);
  fillAll(br,bg,bb);

#if MODE==1
  /*********** 1：Volume bar (bottom-up ring fill) ***********/
  {
    float units = vol * NUM_RINGS;           // 0..6
    int full = (int)floorf(units);           // 
    float frac = units - full;               // next ring light 0..1
    byte rr,rg,rb; hsv2rgb(hue, SAT, val, rr,rg,rb);

    for(int r=0;r<NUM_RINGS;r++){
      float scale = (r<full) ? 1.0f : (r==full ? frac : 0.12f); // last ring
      byte R = (byte)roundf(rr*scale);
      byte G = (byte)roundf(rg*scale);
      byte B = (byte)roundf(rb*scale);
      setRingRGB(r, R,G,B);
    }
  }
#elif MODE==2
  /*********** 2： ***********/
  {
    // speed
    float speed = 0.30f + 2.50f * vol;   // 
    ringHead += speed;                    // 
    while(ringHead>=NUM_RINGS) ringHead-=NUM_RINGS;

    // background
    int head = (int)roundf(ringHead) % NUM_RINGS;

    for(int r=0;r<NUM_RINGS;r++){
      float d = fabsf((float)r - ringHead);
      // wrap  
      d = fminf(d, NUM_RINGS - d);
      float boost = expf(-2.5f * d * d);     //  
      float v2 = clamp01(val*(0.6f + 0.8f*boost));
      float s2 = clamp01(SAT*(0.8f + 0.2f*boost));
      byte rr,rg,rb; hsv2rgb(hue, s2, v2, rr,rg,rb);
      setRingRGB(r, rr,rg,rb);
    }
  }
#else
  /*********** 3：Beat ripple (triggered on rising edge) ***********/
  {
    // rising edge + refractory period
    static float prevEnv=0;
    bool rising = (micEnv - prevEnv) > 2.5f; // 
    unsigned long now = millis();
    if(rising && (now - lastBeat) > 280){    // ~> 214 BPM
      lastBeat = now; rippleActive=true; rippleT0=now;
    }
    prevEnv = micEnv;

    if(rippleActive){
      float t = (now - rippleT0) / 1000.0f;   // s
      float head = t * 6.0f;                  // 
      if(head > (NUM_RINGS-1)+1.0f){ rippleActive=false; }
      for(int r=0;r<NUM_RINGS;r++){
        float d = fabsf(r - head);
        float amp = expf(-2.2f * d * d);      // peak
        float v2 = clamp01(val*(0.6f + 0.9f*amp));
        float s2 = clamp01(SAT*(0.9f));       // 
        byte rr,rg,rb; hsv2rgb(hue, s2, v2, rr,rg,rb);
        setRingRGB(r, rr,rg,rb);
      }
    }
  }
#endif

  // 
  mqttClient.publish(mqtt_topic.c_str(), RGBpayload, PAYLOAD_SIZE);

  // 30 FPS
  delay(33);
}