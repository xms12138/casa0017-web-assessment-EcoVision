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

mapboxgl.accessToken = import.meta.env.VITE_MAPBOX_TOKEN;

const BOROUGH_SOURCE_ID = "london-boroughs";
const BOROUGH_FILL_ID = "borough-fill";
const BOROUGH_BORDER_GLOW_ID = "borough-border-glow";
const LONDON_OUTER_GLOW_ID = "london-outer-glow";

export default {
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

    // 随机颜色：后面可以用 pm2.5 替换掉这里
    const randomColor = () => {
      const hue = Math.floor(Math.random() * 360);
      return `hsl(${hue}, 70%, 55%)`;
    };

    const addBoroughLayers = async () => {
      try {
        // 从 public 目录加载 GeoJSON
        const res = await fetch("/london_boroughs.geojson");
        const geojson = await res.json();

        // 为每个 borough 添加随机颜色（后面可换成真实数据）
        geojson.features = geojson.features.map((f) => {
          f.properties = {
            ...f.properties,
            fillColor: randomColor(),
          };
          return f;
        });

        // 数据源
        if (!map.getSource(BOROUGH_SOURCE_ID)) {
          map.addSource(BOROUGH_SOURCE_ID, {
            type: "geojson",
            data: geojson,
          });
        } else {
          map.getSource(BOROUGH_SOURCE_ID).setData(geojson);
        }

        // 1️⃣ Borough 填充
        if (!map.getLayer(BOROUGH_FILL_ID)) {
          map.addLayer({
            id: BOROUGH_FILL_ID,
            type: "fill",
            source: BOROUGH_SOURCE_ID,
            paint: {
              "fill-color": ["get", "fillColor"],
              "fill-opacity": 0.6,
            },
          });
        }

        // 2️⃣ 外圈 glow
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

        // 3️⃣ 内部边界亮线
        if (!map.getLayer(BOROUGH_BORDER_GLOW_ID)) {
          map.addLayer({
            id: BOROUGH_BORDER_GLOW_ID,
            type: "line",
            source: BOROUGH_SOURCE_ID,
            paint: {
              "line-color": "#ffffff",
              "line-width": 1.4,
              "line-opacity": 0.9,
            },
          });
        }

        // 4️⃣ 自动缩放到包含所有 borough 的范围（关键部分）
        const bounds = new mapboxgl.LngLatBounds();
        geojson.features.forEach((f) => {
          // 兼容 Polygon / MultiPolygon
          const geom = f.geometry;
          const coords =
            geom.type === "Polygon"
              ? geom.coordinates.flat()
              : geom.coordinates.flat(2);

          for (let i = 0; i < coords.length; i++) {
            const [lng, lat] = coords[i];
            bounds.extend([lng, lat]);
          }
        });

        map.fitBounds(bounds, {
          padding: 40,
          maxZoom: 9.2, // 限制别太近，这样看得到全貌
        });
      } catch (err) {
        console.error("❌ Failed to load london_boroughs.geojson", err);
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
            [-0.6, 51.2], // 西南角
            [0.4, 51.8], // 东北角
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
}

.map-container {
  width: 100%;
  height: 85vh; /* 🚀 原本70vh → 85vh，更占屏幕高度 */
  border: 5px solid #a855f7;
  box-shadow: 0 0 50px rgba(168, 85, 247, 0.6);
  border-radius: 0; /* ✅ 让边界更贴合顶部导航 */
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
