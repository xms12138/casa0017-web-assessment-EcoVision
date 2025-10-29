# BEAT — Music Color Band × Heartbeat Shapes

**Arduino MKR WiFi 1010 + MAX3010x + Sound Sensor**, using **MQTT** to control the **Luminaire Color Grid** (6×12 = **72 pixels**).

![Arduino](https://img.shields.io/badge/Arduino-MKR_WiFi_1010-00979D?style=flat&logo=arduino)
![MQTT](https://img.shields.io/badge/MQTT-Protocol-660066?style=flat)
![License](https://img.shields.io/badge/License-Educational-blue?style=flat)

---

## 📖 Table of Contents
- [Overview](#overview)
- [Demo Points](#demo-points)
- [Hardware](#hardware)
- [Wiring](#wiring)
- [Quick Start](#quick-start)
- [Pixel Layout](#pixel-layout)
- [Visual Design](#visual-design)
- [LCD Display](#lcd-display)
- [Key Parameters](#key-parameters)
- [Troubleshooting](#troubleshooting)
- [Test Checklist](#test-checklist)
- [Repository Structure](#repository-structure)
- [Disclaimer](#disclaimer)

---

## 🎨 Overview

**BEAT** : **music beat**   +   **heartbeat**.

### Features:
* **Music :** Blue →Green → Orange → Red color gradient dots based on sound volume
* **Heart:** Heart shape that "breathes" with heartbeat
* **Real-time:** MQTT transmission to Luminaire light  (72 RGB pixels)
* **Interactive:** LCD shows approximate BPM when finger is placed

### Visual Flow:
```
🎵 No beat → 💙 Soft blue dots moving upward
🎵 With beat → 🧡 Warmer colors + flashes
❤️ Heartbeat → 💗 heart breathing light
```

---

## 🎯 Demo Points 

### Core Concept:
* **Two layers blend together:** Music band (bottom→top) + Heart arcs (from center)
* **216-byte RGB payload** sent via MQTT → 72-pixel light grid
* **Story:** I started with RIP sensor, then switched to MAX3010x for better theme fit

### What I Learned:
1. **Sensor tuning:** Thresholds, AGC, debounce, watchdog timers
2. **Signal processing:** 
   - IBI (Inter-Beat Interval) → BPM estimation →using time between heart peaks
   - AC/DC ratio for finger detection
3. **Hardware limits:** Small sound sensors vs professional microphones
4. **Data limits:**  Without database, I can not store long time of heart data.

---

## 🔧 Hardware

### Components:
| Component | Model | Interface | Notes |
|-----------|-------|-----------|-------|
| MCU | Arduino MKR WiFi 1010 | - | WiFi + MQTT |
| Heart Sensor | MAX30102 / MAX30105 | I²C | Pulse oximetry |
| LCD | Grove RGB LCD | I²C | 16x2 backlit display |
| Sound Sensor | Sound Sensor v1.6 | Analog (A2) | Envelope detector |
| Power | USB | 5V | No external power needed |

### Arduino Libraries:
Install via Library Manager:
```
WiFiNINA
PubSubClient
rgb_lcd
MAX30105
```

---

## 🔌 Wiring

### Quick Reference:
```
MAX3010x:
├─ SDA → MKR SDA
├─ SCL → MKR SCL
├─ VIN → 3.3V
└─ GND → GND

Grove RGB LCD:
├─ SDA → MKR SDA
├─ SCL → MKR SCL
├─ VCC → 5V (or 3.3V)
└─ GND → GND

Sound Sensor v1.6:
├─ OUT → A2
├─ VCC → 5V (check your module)
└─ GND → GND
```

### Notes:
* Both MAX3010x and LCD share the I²C bus
* Sound sensor output must go to analog pin
* Check your sound sensor voltage (3.3V or 5V compatible)

---

## 🚀 Quick Start

### 1. Upload
1. Install required libraries
2. Select **Arduino MKR WiFi 1010** in Tools → Board
3. Upload the sketch
4. Open Serial Monitor (115200 baud): test heartbeat

---

## 📐 Pixel Layout (72 pixels)

### Grid Structure:
**6 rings** (bottom→top) × **12 columns** (around)

```
       col:  0    1    2    3    4    5    6    7    8    9   10   11

ring5:     (0,5) (1,5) (2,5) (3,5) (4,5) (5,5) (6,5) (7,5) (8,5) (9,5) (10,5)(11,5)
ring4:     (0,4) (1,4) (2,4) (3,4) (4,4) (5,4) (6,4) (7,4) (8,4) (9,4) (10,4)(11,4)
ring3:     (0,3) (1,3) (2,3) (3,3) (4,3) (5,3) (6,3) (7,3) (8,3) (9,3) (10,3)(11,3)
ring2:     (0,2) (1,2) (2,2) (3,2) (4,2) (5,2) (6,2) (7,2) (8,2) (9,2) (10,2)(11,2)
ring1:     (0,1) (1,1) (2,1) (3,1) (4,1) (5,1) (6,1) (7,1) (8,1) (9,1) (10,1)(11,1)
ring0:     (0,0) (1,0) (2,0) (3,0) (4,0) (5,0) (6,0) (7,0) (8,0) (9,0) (10,0)(11,0)
           ↑ bottom                                                            ↑
```

### Index Formula:
```cpp
// Linear index (0-71)
idx = ring + NUM_RINGS * col;   // NUM_RINGS = 6

// Byte offset in payload
offset = idx * 3;                // R,G,B (3 bytes per pixel)

// Example: (col=3, ring=2)
// idx = 2 + 6*3 = 20
// payload[60] = R, payload[61] = G, payload[62] = B
```

---

## 🎨 Visual Design

### A) Music Layer (Blue → Red)

#### Behavior:
| Condition | Color | Effect |
|-----------|-------|--------|
| Quiet / No beat | 💙 Blue | Slow upward motion |
| Medium volume | 🧡 Orange | Faster motion + brighter |
| Loud / Beat | ❤️ Red | Flash on beats |

#### Visual Pattern:
```
head ↑ (moves with beat)
r5:  .  .  .  .  .  .  .  .  .  .  .  .
r4:  .  .  .  .  ** ** ** .  .  .  .  .
r3:  .  .  .  ** ** ** ** ** .  .  .  .   brightness
r2:  .  .  .  .  ** ** ** .  .  .  .  .
r1:  .  .  .  .  .  ** .  .  .  .  .  .
r0:  .  .  .  .  .  .  .  .  .  .  .  .
     💙 blue → 🧡 orange → ❤️ red
```

### B) Heart  

#### Design:
* **Two arc centers:** Column 3 (left) and Column 9 (right)
* **Breathing effect:** Bulge and shrink with heartbeat
* **Color:** Base magenta (330°) → Red (0°) when heart beats
* **Activation:** Only draws when finger is stable and heartbeat detected

#### Pattern:
```
cols:     0   1   2  [3]  4   5   6   7   8  [9]  10  11
          
r5:                  ·                   ·
r4:              ·       ·           ·       ·
r3:          ·       ·       ·   ·       ·       ·
r2:              ·       ·           ·       ·
r1:                  ·                   ·
r0:     (arcs start from bottom, bulge upward)

```

### Layer Blending:
```cpp
// Step 1: Draw music layer (base)
for each pixel: set RGB based on volume/beat

// Step 2: Add heart layer (overlay)
if (fingerOn && haveHeart) {
  for each heart pixel: 
    RGB_final = RGB_music + RGB_heart  // Additive blend
}

// Step 3: Send to MQTT
publish(mqtt_topic, RGBpayload, 216);
```

---

## 🖥️ LCD Display

### Display States:

| State | Line 1 | Line 2 | Backlight |
|-------|--------|--------|-----------|
| No finger | `Place finger` | `and HOLD 1s` | 💜 Purple |
| Detecting | `Hold steady...` | `Reading...` | ⚪ Gray |
| Measuring | `BPM~ 78` | `IBI:769ms OK` | 💓 Pulses with heartbeat |

### LCD Backlight Colors (by BPM):
```
BPM < 70:   💙 Blue    (resting)
70-100:     💚 Green   (normal)
100-120:    💛 Yellow  (elevated)
BPM > 120:  ❤️ Red     (high)
```

**Note:** BPM display is **approximate** for visualization .  

---

### Finger Detection:
```cpp
IR_ON_THRESH   = 18000   // Finger present (IR threshold)
IR_OFF_THRESH  = 12000   // Finger removed (IR threshold)
AC_ON_THRESH   = 1200    // Finger present (AC threshold)
AC_OFF_THRESH  = 600     // Finger removed (AC threshold)
AC_DC_MIN      = 0.005f  // Min AC/DC ratio (0.5%)
AC_DC_MAX      = 0.30f   // Max AC/DC ratio (30%)

FINGER_ON_STABLE_MS  = 150   // Debounce time (finger on)
FINGER_OFF_STABLE_MS = 600   // Debounce time (finger off)
```

## 🔍 Troubleshooting

### Problem: No heartbeat detected
**Symptoms:**

```
IR:32796  AC:8480  AC/DC:0.259  finger:1
(no "Beat! BPM≈..." appears)
```

---

**Built with ❤️ and 🎵 for CASA0014**

