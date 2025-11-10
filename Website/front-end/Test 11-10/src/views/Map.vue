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
        <TimeSlider
          :model-value="activeHour"
          @update:modelValue="onHourChange"
        />

        <!-- Back button only in street view -->
        <button
          v-if="isStreetViewActive"
          class="map-back-btn"
          @click="exitStreetView"
        >
          ← BACK TO OVERVIEW
        </button>

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
import { ref, onMounted, onBeforeUnmount, onActivated, createApp } from "vue";

import "../assets/styles/map.css";
import MapLegend from "../components/MapLegend.vue";
import BoroughPopup from "../components/BoroughPopup.vue";
import TimeSlider from "../components/TimeSlider.vue";
import RoutePopup from "../components/RoutePopup.vue";

mapboxgl.accessToken = import.meta.env.VITE_MAPBOX_TOKEN;

// Borough layers
const BOROUGH_SOURCE_ID = "london-boroughs";
const BOROUGH_FILL_ID = "borough-fill";
const BOROUGH_BORDER_GLOW_ID = "borough-border-glow";
const LONDON_OUTER_GLOW_ID = "london-outer-glow";
const BOROUGH_LABEL_ID = "borough-labels";

// Busiest streets
const BUSIEST_SOURCE_ID = "busiest-streets";
const BUSIEST_LINE_ID = "busiest-streets-line";

const DEFAULT_HOUR = 0;
const API_BASE_URL = "http://10.129.111.34:3000";

const normalizeForBackend = (name) =>
  name
    .toLowerCase()
    .replace(/london borough of /g, "")
    .replace(/royal borough of /g, "")
    .replace(/city of /g, "")
    .replace(/&/g, "and")
    .replace(/\./g, "")
    .replace(/\s+/g, " ")
    .trim();

// 粗略计算线段方向，用于设置 bearing
const computeBearing = (start, end) => {
  const toRad = (d) => (d * Math.PI) / 180;
  const toDeg = (r) => (r * 180) / Math.PI;

  const [lng1, lat1] = start;
  const [lng2, lat2] = end;

  const φ1 = toRad(lat1);
  const φ2 = toRad(lat2);
  const Δλ = toRad(lng2 - lng1);

  const y = Math.sin(Δλ) * Math.cos(φ2);
  const x =
    Math.cos(φ1) * Math.sin(φ2) - Math.sin(φ1) * Math.cos(φ2) * Math.cos(Δλ);

  const θ = Math.atan2(y, x);
  return (toDeg(θ) + 360) % 360;
};

export default {
  components: {
    MapLegend,
    TimeSlider,
  },

  setup() {
    const mapContainer = ref(null);
    const activeHour = ref(DEFAULT_HOUR);

    let map = null;
    let aqMap = null;
    let baseGeojson = null;

    // Borough popup
    let popup = null;
    let popupApp = null;

    // Route popup（包含 ECharts）
    let routePopup = null;
    let routePopupApp = null;
    let routePopupCloseHandler = null; // 存 close 回调，方便卸载

    // Map<normalizedBoroughName, { center, bearing, bounds, coords }>
    let busiestStreetByBorough = null;

    let highlightedBorough = null;

    // 视角状态
    const isStreetViewActive = ref(false);
    const previousViewState = ref(null);
    let londonBounds = null;

    const resetScroll = () => {
      if (typeof window !== "undefined") {
        window.scrollTo({ top: 0, left: 0, behavior: "auto" });
      }
      if (typeof document !== "undefined") {
        if (document.documentElement) {
          document.documentElement.scrollTop = 0;
          document.documentElement.scrollLeft = 0;
        }
        if (document.body) {
          document.body.scrollTop = 0;
          document.body.scrollLeft = 0;
        }
      }
    };

    // --- Route popup 生命周期管理 ---------------------------------------

    const onRoutePopupClose = () => {
      // 来自用户点 X 或外部点击的 close 事件，此时 Mapbox 已经 remove 了 DOM
      if (routePopupApp) {
        try {
          routePopupApp.unmount();
        } catch (e) {
          console.warn("RoutePopup unmount warning:", e);
        }
        routePopupApp = null;
      }
      routePopup = null;
      routePopupCloseHandler = null;
    };

    const closeRoutePopup = () => {
      // 主动关闭时：先解绑 close 监听，再 remove，避免重复触发
      if (routePopup) {
        if (routePopupCloseHandler) {
          routePopup.off("close", routePopupCloseHandler);
          routePopupCloseHandler = null;
        }
        routePopup.remove();
        routePopup = null;
      }
      if (routePopupApp) {
        try {
          routePopupApp.unmount();
        } catch (e) {
          console.warn("RoutePopup unmount warning:", e);
        }
        routePopupApp = null;
      }
    };

    // ---------------------------------------------------------------------

    const fetchBackendDataForHour = async (hour) => {
      if (!baseGeojson) return;

      const normalizedNames = new Set();

      baseGeojson.features.forEach((f) => {
        const rawName =
          f.properties.NAME ||
          f.properties.BOROUGH ||
          f.properties.LAD13NM ||
          f.properties.name;

        if (!rawName) return;
        const norm = normalizeForBackend(rawName);
        if (norm) normalizedNames.add(norm);
      });

      const results = await Promise.all(
        Array.from(normalizedNames).map(async (normName) => {
          const backendKey = `${normName}_${hour}`;
          const url = `${API_BASE_URL}/api/hourly?borough=${encodeURIComponent(
            backendKey
          )}`;

          try {
            const res = await fetch(url);
            if (!res.ok) {
              console.warn(
                `Failed to fetch data for ${normName}: ${res.status}`
              );
              return null;
            }

            const data = await res.json();

            return {
              normName,
              pm25: data.pm25_g_per_h,
              co: data.co_g_per_h,
              co2: data.co2_kg_per_h,
              vehicles: data.vehicles_per_hour,
            };
          } catch (err) {
            console.error("Error fetching data for", normName, err);
            return null;
          }
        })
      );

      aqMap = new Map();
      results.forEach((item) => {
        if (!item) return;
        aqMap.set(item.normName, {
          pm25: item.pm25,
          co: item.co,
          co2: item.co2,
          vehicles: item.vehicles,
        });
      });
    };

    const buildStyledGeojson = () => {
      if (!baseGeojson || !aqMap) return null;

      return {
        type: "FeatureCollection",
        features: baseGeojson.features.map((f) => {
          const rawName =
            f.properties.NAME ||
            f.properties.BOROUGH ||
            f.properties.LAD13NM ||
            f.properties.name;

          const normName = rawName ? normalizeForBackend(rawName) : null;
          const metrics = normName ? aqMap.get(normName) || {} : {};

          const pm25 = metrics.pm25 ?? null;
          const co = metrics.co ?? null;
          const co2 = metrics.co2 ?? null;
          const vehicles = metrics.vehicles ?? null;

          return {
            ...f,
            properties: {
              ...f.properties,
              BoroughName: rawName || normName || "Unknown",
              NormName: normName || "",
              pm25,
              co,
              co2,
              vehicles,
            },
          };
        }),
      };
    };

    const addBoroughLayers = async () => {
      try {
        if (!baseGeojson) {
          const geoRes = await fetch("/london_boroughs.geojson");
          baseGeojson = await geoRes.json();
        }

        await fetchBackendDataForHour(activeHour.value);

        const styledGeojson = buildStyledGeojson();
        if (!styledGeojson) {
          console.error("No styledGeojson generated");
          return;
        }

        if (!map.getSource(BOROUGH_SOURCE_ID)) {
          map.addSource(BOROUGH_SOURCE_ID, {
            type: "geojson",
            data: styledGeojson,
          });
        } else {
          map.getSource(BOROUGH_SOURCE_ID).setData(styledGeojson);
        }

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
                200,
                "#84cc16",
                260,
                "#eab308",
                340,
                "#f97316",
                430,
                "#ef4444",
                550,
                "#7f1d1d",
              ],
              "fill-opacity": 0.78,
            },
          });
        }

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

        // 保存伦敦总 bounds
        const bounds = new mapboxgl.LngLatBounds();
        styledGeojson.features.forEach((f) => {
          const geom = f.geometry;
          const coords =
            geom.type === "Polygon"
              ? geom.coordinates.flat()
              : geom.coordinates.flat(2);
          coords.forEach(([lng, lat]) => bounds.extend([lng, lat]));
        });
        londonBounds = bounds;
        map.fitBounds(bounds, { padding: 40, maxZoom: 9.2 });

        map.on("mouseenter", BOROUGH_FILL_ID, () => {
          map.getCanvas().style.cursor = "pointer";
        });
        map.on("mouseleave", () => {
          map.getCanvas().style.cursor = "";
        });

        map.on("click", BOROUGH_FILL_ID, (e) => {
          if (!e.features || !e.features.length) return;

          const feature = e.features[0];
          const props = feature.properties || {};

          const rawName =
            props.BoroughName ||
            props.NAME ||
            props.BOROUGH ||
            props.LAD13NM ||
            props.name ||
            "Unknown borough";

          const normName = normalizeForBackend(rawName);
          const metrics = (aqMap && aqMap.get(normName)) || {};

          const pm25 =
            props.pm25 !== undefined && props.pm25 !== null
              ? Number(props.pm25)
              : metrics.pm25 ?? null;
          const co =
            props.co !== undefined && props.co !== null
              ? Number(props.co)
              : metrics.co ?? null;
          const co2 =
            props.co2 !== undefined && props.co2 !== null
              ? Number(props.co2)
              : metrics.co2 ?? null;
          const vehicles =
            props.vehicles !== undefined && props.vehicles !== null
              ? Number(props.vehicles)
              : metrics.vehicles ?? null;

          if (popup) {
            popup.remove();
            popup = null;
          }
          if (popupApp) {
            popupApp.unmount();
            popupApp = null;
          }

          const hasBusiest =
            !!busiestStreetByBorough && !!busiestStreetByBorough.get(normName);

          const container = document.createElement("div");
          popupApp = createApp(BoroughPopup, {
            name: rawName,
            hour: activeHour.value,
            pm25,
            co,
            co2,
            vehicles,
            hasBusiestStreet: hasBusiest,
            onViewStreet: () => {
              // 关闭 borough popup，进入路线视角
              if (popup) {
                popup.remove();
                popup = null;
              }
              if (popupApp) {
                popupApp.unmount();
                popupApp = null;
              }
              flyToBusiestStreet(normName);
            },
          });
          popupApp.mount(container);

          popup = new mapboxgl.Popup({
            closeButton: true,
            closeOnClick: false,
            offset: 16,
          })
            .setLngLat(e.lngLat)
            .setDOMContent(container)
            .addTo(map);

          popup.on("close", () => {
            if (popupApp) {
              popupApp.unmount();
              popupApp = null;
            }
            popup = null;

            if (
              map &&
              map.getLayer(BOROUGH_FILL_ID) &&
              !isStreetViewActive.value
            ) {
              map.setPaintProperty(BOROUGH_FILL_ID, "fill-opacity", 0.78);
              highlightedBorough = null;
            }
          });
        });
      } catch (err) {
        console.error("Failed to load map or backend data", err);
      }
    };

    // busiest_streets.geojson
    const addBusiestStreetsLayer = async () => {
      if (!map) return;

      try {
        const res = await fetch("/busiest_streets.geojson");
        if (!res.ok) {
          console.warn(
            "busiest_streets.geojson not found or failed to load:",
            res.status
          );
          return;
        }
        const geojson = await res.json();

        busiestStreetByBorough = new Map();

        (geojson.features || []).forEach((f) => {
          const props = f.properties || {};
          const geom = f.geometry;
          if (!geom) return;

          const rawBorough =
            props.borough || props.BoroughName || props.borough_name || "";

          const norm = normalizeForBackend(rawBorough);
          if (!norm) return;

          let coords = null;
          if (geom.type === "LineString") {
            coords = geom.coordinates;
          } else if (geom.type === "MultiLineString") {
            coords = geom.coordinates.flat();
          }
          if (!coords || coords.length < 2) return;

          const bounds = coords.reduce(
            (b, c) => b.extend(c),
            new mapboxgl.LngLatBounds(coords[0], coords[0])
          );
          const center = bounds.getCenter();
          const bearing = computeBearing(coords[0], coords[coords.length - 1]);

          busiestStreetByBorough.set(norm, {
            center: [center.lng, center.lat],
            bearing,
            bounds,
            coords,
          });
        });

        if (!map.getSource(BUSIEST_SOURCE_ID)) {
          map.addSource(BUSIEST_SOURCE_ID, {
            type: "geojson",
            data: geojson,
          });
        } else {
          map.getSource(BUSIEST_SOURCE_ID).setData(geojson);
        }

        if (!map.getLayer(BUSIEST_LINE_ID)) {
          map.addLayer({
            id: BUSIEST_LINE_ID,
            type: "line",
            source: BUSIEST_SOURCE_ID,
            layout: {
              "line-cap": "round",
              "line-join": "round",
            },
            paint: {
              "line-color": "#ff3b30",
              "line-width": 6,
              "line-opacity": 0.95,
              "line-blur": 1.2,
            },
          });
        }
      } catch (err) {
        console.error("Error adding busiest streets layer", err);
      }
    };

    // 显示路线 popup（放在路的尽头，一直可重复打开）
    const showRoutePopup = async (normBoroughName) => {
      if (!map || !busiestStreetByBorough) return;

      const info = busiestStreetByBorough.get(normBoroughName);
      if (!info) return;

      const boroughKey = `${normBoroughName}_${activeHour.value}`;
      const url = `${API_BASE_URL}/api/solution?borough=${encodeURIComponent(
        boroughKey
      )}`;

      let data;
      try {
        const res = await fetch(url);
        if (!res.ok) {
          console.warn("Failed to fetch solution data:", res.status);
          return;
        }
        data = await res.json();
      } catch (err) {
        console.error("Error fetching solution data", err);
        return;
      }

      const streetName = data.busiest_street || "Recommended route";
      const carTime = Number.isFinite(Number(data.crossing_time_min))
        ? Number(data.crossing_time_min)
        : null;
      const bikeTime = Number.isFinite(Number(data.bicycle_crossing_min))
        ? Number(data.bicycle_crossing_min)
        : null;
      const carCo2 = Number.isFinite(Number(data.per_car_co2_kg_per_h))
        ? Number(data.per_car_co2_kg_per_h)
        : null;
      const carCo = Number.isFinite(Number(data.per_car_co_g_per_h))
        ? Number(data.per_car_co_g_per_h)
        : null;
      const carPm25 = Number.isFinite(Number(data.per_car_pm25_g_per_h))
        ? Number(data.per_car_pm25_g_per_h)
        : null;

      // 确保每次是干净状态
      closeRoutePopup();

      const container = document.createElement("div");
      routePopupApp = createApp(RoutePopup, {
        street: streetName,
        hour: activeHour.value,
        carTime,
        bikeTime,
        carCo2,
        carCo,
        carPm25,
      });
      routePopupApp.mount(container);

      // 路的尽头
      let anchorLngLat;
      if (info.coords && info.coords.length > 0) {
        const last = info.coords[info.coords.length - 1];
        anchorLngLat = { lng: last[0], lat: last[1] };
      } else if (info.bounds) {
        anchorLngLat = info.bounds.getNorthEast();
      } else if (info.center) {
        anchorLngLat = { lng: info.center[0], lat: info.center[1] };
      } else {
        anchorLngLat = map.getCenter();
      }

      routePopup = new mapboxgl.Popup({
        closeButton: true,
        closeOnClick: false,
        offset: [18, -12],
        anchor: "left",
        className: "solution-popup",
      })
        .setLngLat([anchorLngLat.lng, anchorLngLat.lat])
        .setDOMContent(container)
        .addTo(map);

      // 绑定一次 close 事件
      routePopupCloseHandler = onRoutePopupClose;
      routePopup.on("close", routePopupCloseHandler);
    };

    // 飞到 busiest street + 打开 route popup
    const flyToBusiestStreet = async (normBoroughName) => {
      if (!map || !busiestStreetByBorough) return;
      const info = busiestStreetByBorough.get(normBoroughName);
      if (!info) return;

      const { center, bearing, bounds } = info;

      // 记录进入之前的视角（只记录第一次进入）
      if (!isStreetViewActive.value) {
        previousViewState.value = {
          center: map.getCenter(),
          zoom: map.getZoom(),
          bearing: map.getBearing(),
          pitch: map.getPitch(),
        };
      }

      if (map.getLayer(BOROUGH_FILL_ID)) {
        map.setPaintProperty(BOROUGH_FILL_ID, "fill-opacity", [
          "case",
          ["==", ["get", "NormName"], normBoroughName],
          0.15,
          0.78,
        ]);
        highlightedBorough = normBoroughName;
      }

      if (bounds) {
        map.fitBounds(bounds, {
          padding: 40,
          maxZoom: 16.5,
          bearing,
          pitch: 60,
          duration: 1800,
        });
      } else if (center) {
        map.flyTo({
          center,
          zoom: 16.5,
          bearing,
          pitch: 60,
          duration: 1800,
        });
      }

      isStreetViewActive.value = true;
      await showRoutePopup(normBoroughName);
    };

    const updateMapForHour = async (hour) => {
      if (!baseGeojson || !map) return;

      await fetchBackendDataForHour(hour);

      const styledGeojson = buildStyledGeojson();
      if (!styledGeojson) return;

      const source = map.getSource(BOROUGH_SOURCE_ID);
      if (source) source.setData(styledGeojson);

      if (popup) {
        popup.remove();
        popup = null;
      }
      if (popupApp) {
        popupApp.unmount();
        popupApp = null;
      }
      closeRoutePopup();

      if (map.getLayer(BOROUGH_FILL_ID)) {
        map.setPaintProperty(BOROUGH_FILL_ID, "fill-opacity", 0.78);
      }

      highlightedBorough = null;
      isStreetViewActive.value = false;
      previousViewState.value = null;
    };

    const onHourChange = async (newHour) => {
      if (newHour === activeHour.value) return;
      activeHour.value = newHour;
      await updateMapForHour(newHour);
    };

    const exitStreetView = () => {
      if (!map) return;

      if (previousViewState.value) {
        const { center, zoom } = previousViewState.value;
        map.flyTo({
          center,
          zoom,
          bearing: 0,
          pitch: 0,
          duration: 1500,
        });
      } else if (londonBounds) {
        map.fitBounds(londonBounds, {
          padding: 40,
          maxZoom: 9.2,
          pitch: 0,
          bearing: 0,
          duration: 1500,
        });
      }

      if (map.getLayer(BOROUGH_FILL_ID)) {
        map.setPaintProperty(BOROUGH_FILL_ID, "fill-opacity", 0.78);
      }

      highlightedBorough = null;
      isStreetViewActive.value = false;
      previousViewState.value = null;
      closeRoutePopup();
    };

    const initMap = () => {
      if (map || !mapContainer.value) return;

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

      map.on("load", async () => {
        await addBoroughLayers();
        await addBusiestStreetsLayer();
      });
    };

    onMounted(() => {
      resetScroll();
      initMap();
    });

    onActivated(() => {
      resetScroll();
    });

    onBeforeUnmount(() => {
      if (popup) {
        popup.remove();
        popup = null;
      }
      if (popupApp) {
        popupApp.unmount();
        popupApp = null;
      }
      closeRoutePopup();
      if (map) {
        map.remove();
        map = null;
      }
      isStreetViewActive.value = false;
      previousViewState.value = null;
      highlightedBorough = null;
    });

    return {
      mapContainer,
      activeHour,
      onHourChange,
      isStreetViewActive,
      exitStreetView,
    };
  },
};
</script>
