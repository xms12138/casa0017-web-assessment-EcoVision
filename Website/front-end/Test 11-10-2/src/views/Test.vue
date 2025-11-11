<template>
  <div class="map-page">
    <nav>
      <div class="logo">ECOVISION MATCH TEST</div>
      <ul class="nav-links">
        <li><router-link to="/">BACK TO MAIN</router-link></li>
      </ul>
    </nav>

    <div class="main-container">
      <div class="page-header">
        <h1 class="page-title">3D Road Visualization & Map Matching</h1>
        <p class="page-subtitle">
          Draw busiest streets, snap to real roads, and see them in 3D style!
        </p>
      </div>

      <div class="map-container-wrapper">
        <div ref="mapContainer" class="map-container"></div>

        <div class="overlay-box">
          <div>🖊 左上角线段工具：沿着目标道路点几个节点，双击结束。</div>
          <div>🗑 删除工具：修改错误的线。</div>
          <button class="btn" @click="onSnapAndExport" :disabled="snapping">
            {{ snapping ? "Snapping..." : "Snap & Export" }}
          </button>
          <div class="hint">
            完成后在控制台查看
            <code>busiest_streets_snapped.geojson</code> 输出。
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import mapboxgl from "mapbox-gl";
import "mapbox-gl/dist/mapbox-gl.css";
import MapboxDraw from "@mapbox/mapbox-gl-draw";
import "@mapbox/mapbox-gl-draw/dist/mapbox-gl-draw.css";
import { ref, onMounted, onBeforeUnmount } from "vue";

mapboxgl.accessToken = import.meta.env.VITE_MAPBOX_TOKEN;

export default {
  setup() {
    const mapContainer = ref(null);
    const snapping = ref(false);

    let map = null;
    let draw = null;

    const initMap = () => {
      if (map || !mapContainer.value) return;

      map = new mapboxgl.Map({
        container: mapContainer.value,
        style: "mapbox://styles/mapbox/standard",
        center: [-0.1276, 51.5072],
        zoom: 12,
        pitch: 60, // 🎯 倾斜视角，让线有3D感
        bearing: -20,
        antialias: true,
      });

      map.addControl(new mapboxgl.NavigationControl(), "top-right");

      map.on("load", async () => {
        draw = new MapboxDraw({
          displayControlsDefault: false,
          controls: {
            line_string: true,
            trash: true,
          },
        });
        map.addControl(draw, "top-left");

        // 预加载已有参考线
        try {
          const res = await fetch("/busiest_streets.geojson");
          if (res.ok) {
            const geo = await res.json();
            if (!map.getSource("busiest-streets")) {
              map.addSource("busiest-streets", {
                type: "geojson",
                data: geo,
              });
            }
            // 🎯 添加 3D 高亮线层
            if (!map.getLayer("busiest-streets-line")) {
              map.addLayer({
                id: "busiest-streets-line",
                type: "line",
                source: "busiest-streets",
                layout: {
                  "line-cap": "round",
                  "line-join": "round",
                },
                paint: {
                  "line-color": "#ff3b30",
                  "line-width": 8, // 更粗
                  "line-blur": 2,
                  "line-opacity": 0.95,
                  "line-offset": 1.5, // 模拟立体层
                  "line-translate": [0, -1.5],
                },
              });
            }
          }
        } catch (e) {
          console.warn("No existing busiest_streets.geojson");
        }

        // 暴露控制台工具
        window._matchMap = map;
        window._draw = draw;
      });
    };

    // Map Matching
    const matchLineString = async (coords) => {
      const maxPoints = 100;
      let pts = coords;
      if (coords.length > maxPoints) {
        const step = Math.ceil(coords.length / maxPoints);
        pts = coords.filter((_, i) => i % step === 0);
        if (pts[pts.length - 1] !== coords[coords.length - 1]) {
          pts.push(coords[coords.length - 1]);
        }
      }

      const coordStr = pts.map((c) => `${c[0]},${c[1]}`).join(";");
      const url =
        `https://api.mapbox.com/matching/v5/mapbox/driving/` +
        coordStr +
        `?geometries=geojson&overview=full&access_token=${mapboxgl.accessToken}`;

      const res = await fetch(url);
      const data = await res.json();
      if (!data.matchings?.length) throw new Error("No matchings returned");
      return data.matchings[0].geometry;
    };

    // Snap & Export
    const onSnapAndExport = async () => {
      if (!draw) return;
      const all = draw.getAll();
      if (!all.features?.length) {
        alert("请先画几条线再导出");
        return;
      }

      snapping.value = true;
      const snapped = [];

      for (const f of all.features) {
        if (f.geometry?.type === "LineString") {
          try {
            const snappedGeom = await matchLineString(f.geometry.coordinates);
            snapped.push({
              type: "Feature",
              properties: f.properties || {},
              geometry: snappedGeom,
            });
          } catch (e) {
            console.error("Snap failed:", e);
          }
        }
      }

      snapping.value = false;

      const out = {
        type: "FeatureCollection",
        features: snapped,
      };

      console.log(
        "✅ Snapped GeoJSON (save as busiest_streets.geojson):",
        JSON.stringify(out, null, 2)
      );

      alert("贴合完成！控制台已输出 GeoJSON。");

      // 更新地图展示
      if (map.getSource("busiest-streets")) {
        map.getSource("busiest-streets").setData(out);
      } else {
        map.addSource("busiest-streets", { type: "geojson", data: out });
      }

      if (!map.getLayer("busiest-streets-line")) {
        map.addLayer({
          id: "busiest-streets-line",
          type: "line",
          source: "busiest-streets",
          layout: { "line-cap": "round", "line-join": "round" },
          paint: {
            "line-color": "#ff3b30",
            "line-width": 8,
            "line-blur": 2,
            "line-opacity": 0.95,
            "line-offset": 1.5,
            "line-translate": [0, -1.5],
          },
        });
      }
    };

    onMounted(initMap);

    onBeforeUnmount(() => {
      if (map) map.remove();
    });

    return { mapContainer, snapping, onSnapAndExport };
  },
};
</script>

<style scoped>
.map-page {
  display: flex;
  flex-direction: column;
  min-height: 100vh;
  background: #020817;
  color: #e5e7eb;
}

nav {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 32px;
  background: #020817;
}

.logo {
  font-weight: 700;
  letter-spacing: 1px;
}

.nav-links {
  display: flex;
  gap: 16px;
  list-style: none;
}

.nav-links a {
  color: #9ca3af;
  text-decoration: none;
}

.main-container {
  padding: 16px 32px;
}

.page-header {
  text-align: center;
}

.page-title {
  color: #a855f7;
  font-size: 24px;
  font-weight: 700;
  margin: 0;
}

.page-subtitle {
  font-size: 13px;
  color: #9ca3af;
  margin-top: 4px;
}

.map-container-wrapper {
  position: relative;
  height: 500px;
  margin-top: 18px;
  border-radius: 14px;
  overflow: hidden;
  border: 2px solid #a855f7;
  box-shadow: 0 14px 32px rgba(148, 27, 255, 0.35);
}

.map-container {
  width: 100%;
  height: 100%;
}

.overlay-box {
  position: absolute;
  left: 16px;
  bottom: 16px;
  padding: 10px 12px;
  border-radius: 10px;
  background: rgba(2, 8, 23, 0.94);
  border: 1px solid #4c1d95;
  font-size: 12px;
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.btn {
  padding: 6px 10px;
  font-size: 12px;
  border-radius: 6px;
  border: none;
  cursor: pointer;
  background: linear-gradient(90deg, #9333ea, #a855f7);
  color: #fff;
  font-weight: 600;
}

.btn:disabled {
  opacity: 0.6;
  cursor: default;
}

.hint {
  font-size: 11px;
  color: #9ca3af;
}
</style>
