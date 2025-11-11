<template>
  <div class="home">
    <div id="loading" :class="{ hidden: !loading }">
      <div class="loading-title">ECOVISION</div>
      <div class="loading-status">{{ loadStatus }}</div>
      <div class="loading-bar">
        <div class="loading-fill" :style="{ width: progress + '%' }"></div>
      </div>
      <div class="loading-percent">{{ Math.floor(progress) }}%</div>
    </div>

    <div class="debug hidden" id="debug">
      <div><strong>DEBUG INFO</strong></div>
      <div id="debugInfo" ref="debugInfo"></div>
    </div>

    <div id="globeViz" ref="globeContainer"></div>

    <div class="stats">
      <div>CITIES: <span class="stat-val">17</span></div>
      <div>AQI AVG: <span class="stat-val">{{ currentAQI }}</span></div>
      <div>STATUS: <span class="stat-val">LIVE</span></div>
    </div>

    <div class="controls">
      🖱️ DRAG TO ROTATE<br>
      🔍 SCROLL TO ZOOM<br>
      🎯 CLICK MARKERS
    </div>

    <div id="content">
      <div class="hero">
        <h1 class="glitch" data-text="ECOVISION LONDON">ECOVISION LONDON</h1>
        <p class="subtitle">Global Air Quality Monitoring System</p>
        <div class="scroll-hint" @click="scrollToContent">↓ SCROLL TO EXPLORE ↓</div>
      </div>

      <div class="section">
        <div class="box">
          <h2>🌍 OUR MISSION</h2>
          <p>EcoVision London is a cutting-edge air quality monitoring platform that visualizes pollution data from major cities worldwide.</p>
          <p>Using advanced sensors and data analytics, we track PM2.5, PM10, NO2, O3, and other pollutants.</p>
          <router-link to="/page2" class="btn">EXPLORE LONDON →</router-link>
        </div>
      </div>

      <div class="section">
        <div class="box">
          <h2>⚙️ TECHNOLOGY</h2>
          <p>Our platform leverages Globe.GL, an open-source WebGL library that renders stunning 3D geographical data visualizations.</p>
          <p>The interactive globe displays real-time data with color-coded markers indicating air quality levels.</p>
          <router-link to="/map" class="btn">VIEW MAP →</router-link>
        </div>
      </div>

      <div class="section">
        <div class="box">
          <h2>🎯 GLOBAL IMPACT</h2>
          <p>Air pollution affects millions of lives worldwide. EcoVision London makes pollution data accessible and actionable for everyone.</p>
          <p>Join us in our mission to create awareness and drive change towards cleaner air for future generations.</p>
          <router-link to="/page4" class="btn">MEET THE TEAM →</router-link>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue'

const loading = ref(true)
const progress = ref(0)
const loadStatus = ref('Initializing...')
const currentAQI = ref(95)
const globeContainer = ref(null)
const debugInfo = ref(null)
let globe = null
let updateInterval = null

const cities = [
  {lat:51.5,lng:-0.1,name:'London',aqi:70},
  {lat:40.7,lng:-74.0,name:'New York',aqi:85},
  {lat:35.7,lng:139.7,name:'Tokyo',aqi:50},
  {lat:39.9,lng:116.4,name:'Beijing',aqi:180},
  {lat:28.6,lng:77.2,name:'Delhi',aqi:250},
  {lat:-23.6,lng:-46.7,name:'São Paulo',aqi:100},
  {lat:19.4,lng:-99.1,name:'Mexico City',aqi:140},
  {lat:30.0,lng:31.2,name:'Cairo',aqi:180},
  {lat:-33.9,lng:151.2,name:'Sydney',aqi:55},
  {lat:1.3,lng:103.8,name:'Singapore',aqi:60},
  {lat:48.9,lng:2.3,name:'Paris',aqi:65},
  {lat:52.5,lng:13.4,name:'Berlin',aqi:55},
  {lat:55.8,lng:37.6,name:'Moscow',aqi:95},
  {lat:34.1,lng:-118.2,name:'Los Angeles',aqi:110},
  {lat:22.3,lng:114.2,name:'Hong Kong',aqi:80},
  {lat:31.2,lng:121.5,name:'Shanghai',aqi:160},
  {lat:37.6,lng:127.0,name:'Seoul',aqi:90}
]

const log = (msg, isError = false) => {
  console.log(msg)
  if (debugInfo.value) {
    const div = document.createElement('div')
    div.textContent = msg
    div.style.color = isError ? '#f00' : '#0f0'
    debugInfo.value.appendChild(div)
  }
}

const updateProgress = (pct, status) => {
  progress.value = pct
  if (status) loadStatus.value = status
}

const generatePaths = () => {
  const paths = []
  const N = 20
  for (let i = 0; i < N; i++) {
    let lat = (Math.random() - 0.5) * 90
    let lng = (Math.random() - 0.5) * 360
    let alt = 0
    const points = [[lat, lng, alt]]
    const numPoints = Math.floor(Math.random() * 5000)
    for (let j = 0; j < numPoints; j++) {
      lat += (Math.random() * 2 - 1)
      lng += (Math.random() * 2 - 1)
      alt += (Math.random() * 2 - 1) * 0.01
      alt = Math.max(0, alt)
      points.push([lat, lng, alt])
    }
    paths.push(points)
  }
  return paths
}

const getColor = (aqi) => {
  if (aqi < 50) return '#10b981'
  if (aqi < 100) return '#84cc16'
  if (aqi < 150) return '#fbbf24'
  if (aqi < 200) return '#f97316'
  return '#ef4444'
}

const getStatus = (aqi) => {
  if (aqi < 50) return 'Good ✅'
  if (aqi < 100) return 'Moderate ⚠️'
  if (aqi < 150) return 'Unhealthy 🟠'
  if (aqi < 200) return 'Very Unhealthy 🔴'
  return 'Hazardous ☢️'
}

const initGlobe = () => {
  try {
    updateProgress(50, 'Creating globe...')
    log('Creating globe instance...')
    if (!globeContainer.value) throw new Error('Globe container not found')
    log('✓ Container found')
    globe = Globe()(globeContainer.value)
    log('✓ Globe instance created')
    updateProgress(60, 'Loading textures...')
    globe
      .globeImageUrl('https://unpkg.com/three-globe@2.27.2/example/img/earth-blue-marble.jpg')
      .bumpImageUrl('https://unpkg.com/three-globe@2.27.2/example/img/earth-topology.png')
      .backgroundImageUrl('https://unpkg.com/three-globe@2.27.2/example/img/night-sky.png')
    log('✓ Textures configured')
    updateProgress(70, 'Adding paths...')
    const pathsData = generatePaths()
    globe
      .pathsData(pathsData)
      .pathPoints(d => d)
      .pathPointLat(p => p[0])
      .pathPointLng(p => p[1])
      .pathPointAlt(p => p[2])
      .pathColor(() => ['rgba(168,85,247,0.8)', 'rgba(236,72,153,0.8)'])
      .pathDashLength(0.01)
      .pathDashGap(0.004)
      .pathDashAnimateTime(100000)
      .pathStroke(2)
    log('✓ Paths added')
    updateProgress(80, 'Adding cities...')
    globe
      .pointsData(cities)
      .pointAltitude(0.01)
      .pointRadius(d => Math.sqrt(d.aqi) * 0.03)
      .pointColor(d => getColor(d.aqi))
      .pointLabel(d => `<div style="background:#000;padding:12px;border:2px solid #a855f7;font-family:'Press Start 2P';font-size:10px;color:#a855f7;"><b style="color:#fff;font-size:11px;">🌆 ${d.name}</b><br/><b>AQI:</b> <span style="color:${getColor(d.aqi)};font-size:14px;">${d.aqi}</span><br/><b>Status:</b> ${getStatus(d.aqi)}</div>`)
    log('✓ Cities added')
    updateProgress(85, 'Configuring atmosphere...')
    globe.atmosphereColor('#a855f7').atmosphereAltitude(0.3)
    log('✓ Atmosphere configured')
    updateProgress(90, 'Setting up view...')
    globe.width(window.innerWidth).height(window.innerHeight).pointOfView({lat: 30, lng: 0, altitude: 2.2}, 0)
    log('✓ View configured')
    updateProgress(95, 'Enabling controls...')
    globe.controls().enableZoom = true
    globe.controls().autoRotate = true
    globe.controls().autoRotateSpeed = 0.3
    log('✓ Controls enabled')
    window.addEventListener('scroll', () => {
      const scroll = window.pageYOffset
      const max = document.body.scrollHeight - window.innerHeight
      const pct = Math.min(scroll / max, 1)
      const alt = 2.2 + pct * 1.3
      globe.pointOfView({altitude: alt}, 500)
    })
    window.addEventListener('resize', () => {
      if (globe) {
        globe.width(window.innerWidth)
        globe.height(window.innerHeight)
      }
    })
    updateInterval = setInterval(() => {
      const city = cities[Math.floor(Math.random() * cities.length)]
      currentAQI.value = city.aqi
    }, 3000)
    updateProgress(100, 'Complete!')
    log('=== GLOBE READY ===')
    setTimeout(() => { loading.value = false }, 500)
  } catch (error) {
    log('✗ ERROR: ' + error.message, true)
    loadStatus.value = '✗ Error: ' + error.message
  }
}

const scrollToContent = () => {
  window.scrollTo({top: window.innerHeight, behavior: 'smooth'})
}

onMounted(() => {
  log('=== ECOVISION STARTING ===')
  updateProgress(10, 'Checking Globe.GL...')
  let checkCount = 0
  const checkGlobe = setInterval(() => {
    checkCount++
    if (typeof Globe !== 'undefined') {
      clearInterval(checkGlobe)
      log('✓ Globe.GL loaded')
      updateProgress(30, 'Globe.GL loaded')
      initGlobe()
    } else if (checkCount > 50) {
      clearInterval(checkGlobe)
      log('✗ Globe.GL failed to load', true)
      loadStatus.value = '✗ Error: Globe.GL not loaded'
    }
  }, 100)
  window.addEventListener('keydown', (e) => {
    if (e.key === 'd' || e.key === 'D') {
      const debug = document.getElementById('debug')
      if (debug) debug.classList.toggle('hidden')
    }
  })
})

onBeforeUnmount(() => {
  if (updateInterval) clearInterval(updateInterval)
})
</script>
<style scoped>
#globeViz {
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  z-index: 1;
  background: #000;
}

#loading {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: #000;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  z-index: 9999;
}

#loading.hidden {
  display: none;
}

.loading-title {
  font-family: 'Press Start 2P', cursive;
  color: #a855f7;
  font-size: 1.5rem;
  margin-bottom: 2rem;
  text-shadow: 0 0 20px #a855f7;
}

.loading-bar {
  width: 400px;
  height: 30px;
  border: 3px solid #a855f7;
  background: #000;
  position: relative;
  overflow: hidden;
}

.loading-fill {
  height: 100%;
  width: 0%;
  background: linear-gradient(90deg, #a855f7, #ec4899);
  transition: width 0.3s;
}

.loading-percent {
  font-family: 'Press Start 2P', cursive;
  color: #a855f7;
  font-size: 0.9rem;
  margin-top: 1rem;
}

.loading-status {
  font-family: 'Press Start 2P', cursive;
  color: #fff;
  font-size: 0.7rem;
  margin-top: 1rem;
}

#content {
  position: relative;
  z-index: 10;
  width: 100%;
}

.hero {
  min-height: 100vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 2rem;
  position: relative;
}

.glitch {
  font-family: 'Press Start 2P', cursive;
  font-size: 3rem;
  color: #fff;
  text-shadow: 0 0 20px #a855f7;
  margin-bottom: 2rem;
  position: relative;
  animation: glitch-skew 1s infinite;
}

.glitch::before,
.glitch::after {
  content: attr(data-text);
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
}

.glitch::before {
  left: 2px;
  text-shadow: -2px 0 #ff0088;
  clip: rect(24px, 550px, 90px, 0);
  animation: glitch-anim 3s infinite linear alternate-reverse;
}

.glitch::after {
  left: -2px;
  text-shadow: -2px 0 #00fff9;
  clip: rect(85px, 550px, 140px, 0);
  animation: glitch-anim 2.5s infinite linear alternate-reverse;
}

@keyframes glitch-anim {
  0% { clip: rect(61px, 9999px, 52px, 0); }
  10% { clip: rect(94px, 9999px, 77px, 0); }
  20% { clip: rect(47px, 9999px, 34px, 0); }
  30% { clip: rect(73px, 9999px, 95px, 0); }
  40% { clip: rect(16px, 9999px, 88px, 0); }
  50% { clip: rect(53px, 9999px, 41px, 0); }
  60% { clip: rect(28px, 9999px, 65px, 0); }
  70% { clip: rect(82px, 9999px, 19px, 0); }
  80% { clip: rect(35px, 9999px, 76px, 0); }
  90% { clip: rect(68px, 9999px, 42px, 0); }
  100% { clip: rect(21px, 9999px, 58px, 0); }
}

@keyframes glitch-skew {
  0% { transform: skew(0deg); }
  10% { transform: skew(-2deg); }
  20% { transform: skew(2deg); }
  30% { transform: skew(-1deg); }
  40% { transform: skew(1deg); }
  50% { transform: skew(0deg); }
}

.subtitle {
  font-size: 1.5rem;
  color: #ffffff;
  margin-bottom: 3rem;
}

.scroll-hint {
  position: absolute;
  bottom: 2rem;
  font-family: 'Press Start 2P', cursive;
  font-size: 0.8rem;
  color: #a855f7;
  animation: bounce 2s infinite;
  cursor: pointer;
}

@keyframes bounce {
  0%, 100% { transform: translateY(0); }
  50% { transform: translateY(-10px); }
}

.section {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 4rem 2rem;
}

.box {
  max-width: 900px;
  background: rgba(0, 0, 0, 0.95);
  border: 3px solid #a855f7;
  padding: 3rem;
  box-shadow: 0 0 30px rgba(168, 85, 247, 0.5);
}

.box h2 {
  font-family: 'Press Start 2P', cursive;
  color: #a855f7;
  font-size: 1.8rem;
  margin-bottom: 2rem;
  line-height: 1.8;
}

.box p {
  font-size: 1.2rem;
  line-height: 1.8;
  color: #e0e0e0;
  margin-bottom: 1.5rem;
}

.btn {
  display: inline-block;
  padding: 1rem 2rem;
  background: #a855f7;
  color: #fff;
  text-decoration: none;
  font-family: 'Press Start 2P', cursive;
  font-size: 0.8rem;
  border: 3px solid #fff;
  box-shadow: 5px 5px 0 #000;
  transition: all 0.1s;
  margin-top: 2rem;
}

.btn:hover {
  transform: translate(2px, 2px);
  box-shadow: 3px 3px 0 #000;
}

.stats {
  position: fixed;
  top: 2rem;
  right: 2rem;
  background: rgba(0, 0, 0, 0.95);
  border: 2px solid #a855f7;
  padding: 1rem;
  font-family: 'Press Start 2P', cursive;
  font-size: 0.6rem;
  color: #a855f7;
  z-index: 100;
  line-height: 2;
}

.stat-val {
  color: #ff0088;
}

.controls {
  position: fixed;
  bottom: 2rem;
  right: 2rem;
  background: rgba(0, 0, 0, 0.95);
  border: 2px solid #a855f7;
  padding: 1rem;
  font-family: 'Press Start 2P', cursive;
  font-size: 0.6rem;
  color: #a855f7;
  z-index: 100;
  line-height: 1.8;
}

.debug {
  position: fixed;
  top: 2rem;
  left: 2rem;
  background: rgba(0, 0, 0, 0.95);
  border: 2px solid #0f0;
  padding: 1rem;
  font-family: monospace;
  font-size: 0.7rem;
  color: #0f0;
  z-index: 9998;
  max-width: 300px;
}

.debug.hidden {
  display: none;
}

@media (max-width: 768px) {
  .glitch { font-size: 1.5rem; }
  .subtitle { font-size: 1rem; }
  .box { padding: 2rem; }
  .box h2 { font-size: 1.2rem; }
  .loading-bar { width: 300px; }
}
</style>
