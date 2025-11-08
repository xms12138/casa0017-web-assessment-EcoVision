<template>
  <div class="map-page">
    <!-- Navigation -->
    <nav>
      <div class="logo">ECOVISION</div>
      <ul class="nav-links">
        <li><router-link to="/">HOME</router-link></li>
        <li><router-link to="/page2">ABOUT</router-link></li>
        <li><router-link to="/map" class="active">MAP</router-link></li>
        <li><router-link to="/page4">TEAM</router-link></li>
      </ul>
    </nav>

    <!-- Main Container -->
    <div class="main-container">
      <!-- Page Header -->
      <div class="page-header">
        <h1 class="page-title">Interactive London Map</h1>
        <p class="page-subtitle">
          Real-time air quality visualization across 33 boroughs
        </p>
      </div>

      <!-- Map Container (Real Mapbox Map Replaces Placeholder) -->
      <div class="map-container-wrapper">
        <div ref="mapContainer" class="map-container"></div>
      </div>

      <!-- Info Section -->
      <div class="map-info">
        <div class="info-grid">
          <div class="info-card">
            <h3>Historical Data</h3>
            <p>
              Access previous month's air quality data to identify patterns and
              trends across London boroughs.
            </p>
          </div>

          <div class="info-card">
            <h3>Interactive Features</h3>
            <p>
              Zoom, pan, and click on boroughs to see detailed air quality
              metrics and pollution sources.
            </p>
          </div>

          <div class="info-card">
            <h3>Time Slider</h3>
            <p>
              Navigate through 24 hours of data to understand how air quality
              changes throughout the day.
            </p>
          </div>

          <div class="info-card">
            <h3>Data Sources</h3>
            <p>
              Powered by London Air Quality Network and TfL open datasets for
              accurate, reliable information.
            </p>
          </div>
        </div>

        <!-- CTA -->
        <div class="cta-container">
          <router-link to="/page4" class="pixel-button"
            >Support Our Research</router-link
          >
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import mapboxgl from "mapbox-gl";
import "mapbox-gl/dist/mapbox-gl.css";
import { onMounted, onBeforeUnmount, onActivated, ref } from "vue";

mapboxgl.accessToken = import.meta.env.VITE_MAPBOX_TOKEN;

export default {
  setup() {
    const mapContainer = ref(null);
    let map = null;

    const resetScroll = () => {
      // 通杀几种可能的滚动容器
      window.scrollTo(0, 0);
      document.body.scrollTop = 0;
      document.documentElement.scrollTop = 0;
      const app = document.getElementById("app");
      if (app) app.scrollTop = 0;
    };

    const initMap = () => {
      if (!map && mapContainer.value) {
        map = new mapboxgl.Map({
          container: mapContainer.value,
          style: "mapbox://styles/mapbox/standard",
          center: [-0.12, 51.5], // London
          zoom: 9,
        });
      }
    };

    onMounted(() => {
      resetScroll();
      initMap();
    });

    // 若使用 <keep-alive>，再次激活时也回到顶部
    onActivated(() => {
      resetScroll();
    });

    onBeforeUnmount(() => {
      if (map) {
        map.remove();
        map = null;
      }
    });

    return { mapContainer };
  },
};
</script>

<style scoped>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

.map-page {
  font-family: "Space Grotesk", sans-serif;
  background: #0a0a0f;
  color: #fff;
  overflow-x: hidden;
}

/* Navigation */
nav {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  background: rgba(0, 0, 0, 0.95);
  border-bottom: 2px solid #a855f7;
  padding: 1rem 2rem;
  z-index: 100;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.logo {
  font-family: "Press Start 2P", cursive;
  font-size: 0.8rem;
  color: #a855f7;
  text-shadow: 0 0 10px rgba(168, 85, 247, 0.8);
}

.nav-links {
  display: flex;
  gap: 2rem;
  list-style: none;
}

.nav-links a {
  color: #fff;
  text-decoration: none;
  font-family: "Press Start 2P", cursive;
  font-size: 0.6rem;
  transition: all 0.3s ease;
}

.nav-links a:hover {
  color: #a855f7;
  text-shadow: 0 0 10px rgba(168, 85, 247, 0.8);
}

.nav-links a.active {
  color: #ff0088;
}

/* Main Container */
.main-container {
  padding-top: 80px;
  min-height: 100vh;
}

/* Page Title */
.page-header {
  text-align: center;
  padding: 3rem 2rem;
  background: linear-gradient(
    180deg,
    rgba(138, 43, 226, 0.2) 0%,
    transparent 100%
  );
}

.page-title {
  font-family: "Press Start 2P", cursive;
  font-size: clamp(1.5rem, 4vw, 2.5rem);
  color: #a855f7;
  margin-bottom: 1rem;
  text-shadow: 0 0 20px rgba(168, 85, 247, 0.8);
  animation: pulse 2s ease-in-out infinite;
}

@keyframes pulse {
  0%,
  100% {
    text-shadow: 0 0 20px rgba(168, 85, 247, 0.8);
  }
  50% {
    text-shadow: 0 0 40px rgba(168, 85, 247, 1);
  }
}

.page-subtitle {
  font-size: 1.1rem;
  color: #ccc;
  line-height: 1.8;
}

/* Map Container */
.map-container-wrapper {
  max-width: 1400px;
  margin: 2rem auto;
  padding: 0 2rem;
}

.map-container {
  width: 100%;
  height: 70vh;
  min-height: 500px;
  border: 4px solid #a855f7;
  box-shadow: 0 0 40px rgba(168, 85, 247, 0.5);
  border-radius: 10px;
  overflow: hidden;
}

/* Info Section */
.map-info {
  max-width: 1200px;
  margin: 4rem auto;
  padding: 0 2rem 4rem;
}

.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 2rem;
}

.info-card {
  background: rgba(168, 85, 247, 0.1);
  border: 2px solid #a855f7;
  padding: 2rem;
  transition: all 0.3s ease;
}

.info-card:hover {
  transform: translateY(-10px);
  box-shadow: 0 20px 40px rgba(168, 85, 247, 0.3);
  background: rgba(168, 85, 247, 0.15);
}

.info-card h3 {
  font-family: "Press Start 2P", cursive;
  font-size: 0.9rem;
  color: #a855f7;
  margin-bottom: 1rem;
  line-height: 1.8;
}

.info-card p {
  color: #ccc;
  line-height: 1.8;
}

/* CTA Button */
.cta-container {
  text-align: center;
  margin-top: 3rem;
}

.pixel-button {
  display: inline-block;
  padding: 1rem 2rem;
  background: #a855f7;
  color: #fff;
  text-decoration: none;
  font-family: "Press Start 2P", cursive;
  font-size: 0.7rem;
  border: 3px solid #fff;
  box-shadow: 5px 5px 0 #000;
  transition: all 0.1s ease;
  cursor: pointer;
  text-transform: uppercase;
}

.pixel-button:hover {
  transform: translate(2px, 2px);
  box-shadow: 3px 3px 0 #000;
}

.pixel-button:active {
  transform: translate(5px, 5px);
  box-shadow: none;
}

/* Responsive */
@media (max-width: 768px) {
  nav {
    flex-direction: column;
    gap: 1rem;
  }

  .nav-links {
    gap: 1rem;
  }

  .nav-links a {
    font-size: 0.5rem;
  }

  .map-container {
    height: 50vh;
    min-height: 400px;
  }

  .info-grid {
    grid-template-columns: 1fr;
  }
}
</style>
