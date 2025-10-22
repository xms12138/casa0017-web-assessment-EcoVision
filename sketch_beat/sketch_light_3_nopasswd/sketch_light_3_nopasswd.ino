// MKR WiFi 1010 → MQTT → Vespera (Lumi)
// PIR gated continuous sweep: motion → start color sweep (bottom→top, never stops);
// no motion → lights off. Loudness makes it brighter & redder.
// Layout: 6 rings × 12 pixels; physical index = ring + 6 * col (ring: 0=bottom…5=top)

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <math.h>

/******** 1) Wi-Fi / MQTT (EDIT THESE) ********/
#define SECRET_SSID      " "                 // 
#define SECRET_PASS      " " // 
#define SECRET_MQTTUSER  " "                        // 
#define SECRET_MQTTPASS  " "       // 

const char* ssid          = SECRET_SSID;
const char* password      = SECRET_PASS;
const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;
const char* mqtt_server   = "mqtt.cetools.org";
const int   mqtt_port     = 1884;

/******** 2) Topic (EDIT your id) ********/
String lightId    = "0"; // ← Google Sheet 
String mqtt_topic = "student/CASA0014/luminaire/" + lightId;

/******** 3) Layout & payload ********/
const int NUM_RINGS = 6;       // rows (bottom→top)
const int RING_SIZE = 12;      // columns
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

/******** 4) Sensors ********/
// MIC (A2)
const int PIN_MIC = A2;
float micMean=512.0f, micEnv=0.0f;
const float MIC_ALPHA_MEAN=0.01f;
const float MIC_ALPHA_ENV =0.08f;
float MIC_ENV_MAX=120.0f; // auto-calibrated on boot

// PIR (WPSE314) on D2
const int PIN_PIR = 2;
const unsigned long ACTIVE_HOLD_MS = 15000; // keep active for 15s after last motion
unsigned long activeUntil = 0;

/******** 5) Color mapping (yellow→red; louder→brighter) ********/
const float HUE_YELLOW = 60.0f;
const float HUE_RED    = 0.0f;
const float SAT        = 1.0f;

const float QUIET_MIN_VAL = 0.58f;  // bright quiet baseline (when active)
float BASE_V = 0.32f;
float V_GAIN = 0.70f;

/******** 6) Continuous sweep (always scrolling while active) ********/
float sweepHead = 0.0f;
unsigned long lastTick = 0;
const float BASE_SPEED_RPS = 1.2f;  // quiet speed (rings/sec)
const float SPEED_GAIN     = 3.0f;  // louder → faster
const float SWEEP_SHAPE    = 2.2f;  // band width (gaussian)
const float HUE_PULL_GAIN  = 0.60f; // pull toward red on the band
const float VAL_BOOST_BASE = 0.85f; // band brightness base factor
const float VAL_BOOST_GAIN = 0.50f; // band brightness extra boost

/******** 7) MQTT ********/
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

/******** Setup ********/
void setup(){
  Serial.begin(115200); delay(800);
  Serial.println("\nPIR-gated Continuous Sweep (approach→run; away→off)");

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

  lastTick = millis();
}

/******** Loop ********/
void loop(){
  if(WiFi.status()!=WL_CONNECTED) startWifi();
  if(!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  // --- PIR gating ---
  int pir = digitalRead(PIN_PIR);                // HIGH when motion
  unsigned long now = millis();
  if(pir == HIGH) activeUntil = now + ACTIVE_HOLD_MS;
  bool active = (now < activeUntil);

  if(active){
    
    int s=analogRead(PIN_MIC);
    micMean += (s - micMean)*MIC_ALPHA_MEAN;
    float ac=fabsf(s - micMean);
    micEnv  += (ac - micEnv)*MIC_ALPHA_ENV;

    
    float vol  = clamp01(micEnv / MIC_ENV_MAX);
    float vmap = powf(fminf(vol * 1.6f, 1.0f), 0.7f); // 0..1

    //  val: louder → red & bright; quiet still yellow
    float baseHue = HUE_YELLOW + (HUE_RED - HUE_YELLOW) * vmap; // 60→0
    float baseVal = clamp01(BASE_V + V_GAIN * vmap);
    if(baseVal < QUIET_MIN_VAL) baseVal = QUIET_MIN_VAL;

    //  
    static unsigned long lastTickLocal = 0;
    float dt = (now - lastTick) / 1000.0f; if(dt<0) dt=0; lastTick = now;

    // speed never 0; louder → faster
    float speed = BASE_SPEED_RPS + SPEED_GAIN * vmap; // rings/sec
    sweepHead += speed * dt;
    while(sweepHead >= NUM_RINGS) sweepHead -= NUM_RINGS;

    // render frame
    for(int r=0;r<NUM_RINGS;r++){
      float d = fabsf(r - sweepHead);
      d = fminf(d, NUM_RINGS - d);
      float boost = expf(-SWEEP_SHAPE * d * d);    // 0..1

      float v2 = clamp01(baseVal * (VAL_BOOST_BASE + VAL_BOOST_GAIN * boost));
      float pull = HUE_PULL_GAIN * vmap * boost;   // 0..0.6
      float h2 = baseHue * (1.0f - pull);
      if(h2 < 0) h2 = 0;

      byte rr,rg,rb; hsv2rgb(h2, SAT, v2, rr,rg,rb);
      setRingRGB(r, rr,rg,rb);
    }
  } else {
    //  
    fillAll(0,0,0);
  }

  //  
  mqttClient.publish(mqtt_topic.c_str(), RGBpayload, PAYLOAD_SIZE);

  // ~30 FPS
  delay(33);
}
