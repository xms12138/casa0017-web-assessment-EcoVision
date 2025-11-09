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

      <!-- Map Container -->
      <div class="map-container-wrapper">
        <div ref="mapContainer" class="map-container"></div>
        <MapLegend />
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
              Powered by the London Air Quality Network and TfL open datasets
              for accurate, reliable information.
            </p>
          </div>
        </div>

        <!-- CTA -->
        <div class="cta-container">
          <router-link to="/page4" class="pixel-button">
            Support Our Research
          </router-link>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import mapboxgl from "mapbox-gl";
import "mapbox-gl/dist/mapbox-gl.css";
import { onMounted, onBeforeUnmount, onActivated, ref } from "vue";
import MapLegend from "../components/MapLegend.vue";
mapboxgl.accessToken = import.meta.env.VITE_MAPBOX_TOKEN;

const BOROUGH_SOURCE_ID = "london-boroughs";
const BOROUGH_FILL_ID = "borough-fill";
const BOROUGH_BORDER_GLOW_ID = "borough-border-glow";
const LONDON_OUTER_GLOW_ID = "london-outer-glow";
const BOROUGH_LABEL_ID = "borough-labels";
// Default active hour (0–23). Later this can be linked with the time slider.
const ACTIVE_HOUR = 1;

export default {
  components: {
    MapLegend,
  },
  setup() {
    const mapContainer = ref(null);
    let map = null;

    const resetScroll = () => {
      window.scrollTo(0, 0);
      document.body.scrollTop = 0;
      document.documentElement.scrollTop = 0;
      const app = document.getElementById("app");
      if (app) app.scrollTop = 0;
    };

    const addBoroughLayers = async () => {
      try {
        // Load both boundary and test data
        const [geoRes, aqRes] = await Promise.all([
          fetch("/london_boroughs.geojson"),
          fetch("/borough_air_quality.json"),
        ]);

        const geojson = await geoRes.json();
        const aq = await aqRes.json();

        // Map air quality data: borough name -> hourly data
        const aqMap = new Map();
        aq.data.forEach((entry) => {
          aqMap.set(entry.borough, entry.hourly);
        });

        // Attach pm2.5 value to each borough feature
        geojson.features = geojson.features.map((f) => {
          const name =
            f.properties.NAME ||
            f.properties.BOROUGH ||
            f.properties.LAD13NM ||
            f.properties.name;

          const hourly = aqMap.get(name);
          const pm25Array = hourly?.pm25;
          const pm25 =
            Array.isArray(pm25Array) && pm25Array.length > 0
              ? pm25Array[ACTIVE_HOUR] ?? pm25Array[0]
              : 0;

          f.properties = {
            ...f.properties,
            pm25,
          };

          return f;
        });

        // Add or update data source
        if (!map.getSource(BOROUGH_SOURCE_ID)) {
          map.addSource(BOROUGH_SOURCE_ID, {
            type: "geojson",
            data: geojson,
          });
        } else {
          map.getSource(BOROUGH_SOURCE_ID).setData(geojson);
        }

        // 1️⃣ Fill color by pm2.5 level
        if (!map.getLayer(BOROUGH_FILL_ID)) {
          map.addLayer({
            id: BOROUGH_FILL_ID,
            type: "fill",
            source: BOROUGH_SOURCE_ID,
            paint: {
              "fill-color": [
                "interpolate",
                ["linear"],
                ["get", "pm25"],
                0,
                "#22c55e",
                10,
                "#84cc16",
                20,
                "#eab308",
                30,
                "#f97316",
                40,
                "#ef4444",
                60,
                "#7f1d1d",
              ],
              "fill-opacity": 0.78,
            },
          });
        }

        // 2️⃣ Outer glow outline
        if (!map.getLayer(LONDON_OUTER_GLOW_ID)) {
          map.addLayer({
            id: LONDON_OUTER_GLOW_ID,
            type: "line",
            source: BOROUGH_SOURCE_ID,
            paint: {
              "line-color": "#a855f7",
              "line-width": 6,
              "line-opacity": 0.22,
            },
          });
        }

        // 3️⃣ Borough borders
        if (!map.getLayer(BOROUGH_BORDER_GLOW_ID)) {
          map.addLayer({
            id: BOROUGH_BORDER_GLOW_ID,
            type: "line",
            source: BOROUGH_SOURCE_ID,
            paint: {
              "line-color": "#ffffff",
              "line-width": 1.2,
              "line-opacity": 0.9,
            },
          });
        }

        // 4️⃣ Borough name labels
        if (!map.getLayer(BOROUGH_LABEL_ID)) {
          map.addLayer({
            id: BOROUGH_LABEL_ID,
            type: "symbol",
            source: BOROUGH_SOURCE_ID,
            layout: {
              "text-field": [
                "coalesce",
                ["get", "NAME"],
                ["get", "BOROUGH"],
                ["get", "LAD13NM"],
                ["get", "name"],
              ],
              "text-font": ["Open Sans Semibold", "Arial Unicode MS Bold"],
              "text-size": 11,
              "text-offset": [0, 0],
              "text-anchor": "center",
              "text-allow-overlap": false,
            },
            paint: {
              "text-color": "#ffffff",
              "text-halo-color": "rgba(10,10,15,0.9)",
              "text-halo-width": 1.4,
            },
          });
        }

        // 5️⃣ Auto-fit map to borough bounds
        const bounds = new mapboxgl.LngLatBounds();
        geojson.features.forEach((f) => {
          const geom = f.geometry;
          const coords =
            geom.type === "Polygon"
              ? geom.coordinates.flat()
              : geom.coordinates.flat(2);

          coords.forEach(([lng, lat]) => {
            bounds.extend([lng, lat]);
          });
        });

        map.fitBounds(bounds, {
          padding: 40,
          maxZoom: 9.2,
        });

        // 6️⃣ Change cursor to pointer when hovering over boroughs
        map.on("mouseenter", BOROUGH_FILL_ID, () => {
          map.getCanvas().style.cursor = "pointer";
        });
        map.on("mouseleave", BOROUGH_FILL_ID, () => {
          map.getCanvas().style.cursor = "";
        });
      } catch (err) {
        console.error("❌ Failed to load map or air quality data", err);
      }
    };

    const initMap = () => {
      if (!map && mapContainer.value) {
        map = new mapboxgl.Map({
          container: mapContainer.value,
          style: "mapbox://styles/mapbox/standard",
          center: [-0.1276, 51.5072],
          zoom: 9,
          maxBounds: [
            [-0.6, 51.2],
            [0.4, 51.8],
          ],
        });

        map.addControl(new mapboxgl.NavigationControl(), "top-right");

        map.on("load", () => {
          console.log("✅ London basemap loaded");
          addBoroughLayers();
        });
      }
    };

    onMounted(() => {
      resetScroll();
      initMap();
    });

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
  width: 100%;
  margin: 0;
  padding: 0;
  position: relative; /* 为 legend 定位 */
}

.map-container {
  width: 100%;
  height: 85vh;
  border: 5px solid #a855f7;
  box-shadow: 0 0 50px rgba(168, 85, 247, 0.6);
  border-radius: 0;
  overflow: hidden;
}

/* Legend */
.map-legend {
  position: absolute;
  right: 18px;
  bottom: 18px;
  padding: 10px 12px;
  background: rgba(5, 5, 10, 0.96);
  border: 1px solid #a855f7;
  border-radius: 8px;
  box-shadow: 0 0 16px rgba(168, 85, 247, 0.6);
  font-size: 10px;
  color: #e5e7eb;
  z-index: 5;
}

.legend-title {
  font-size: 10px;
  margin-bottom: 4px;
  color: #a855f7;
  text-transform: uppercase;
  letter-spacing: 0.06em;
}

.legend-row {
  display: flex;
  align-items: center;
  gap: 6px;
  margin-bottom: 2px;
}

.legend-color {
  width: 14px;
  height: 8px;
  border-radius: 4px;
}

.legend-note {
  margin-top: 4px;
  font-size: 8px;
  color: #9ca3af;
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
