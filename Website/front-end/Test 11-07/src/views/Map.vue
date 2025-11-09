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
import { ref, onMounted, onBeforeUnmount, onActivated, createApp } from "vue";

import "../assets/styles/map.css";
import MapLegend from "../components/MapLegend.vue";
import BoroughPopup from "../components/BoroughPopup.vue";

mapboxgl.accessToken = import.meta.env.VITE_MAPBOX_TOKEN;

const BOROUGH_SOURCE_ID = "london-boroughs";
const BOROUGH_FILL_ID = "borough-fill";
const BOROUGH_BORDER_GLOW_ID = "borough-border-glow";
const LONDON_OUTER_GLOW_ID = "london-outer-glow";
const BOROUGH_LABEL_ID = "borough-labels";

// For now: fixed hour (0–23). Later you can make this reactive with a time slider.
const ACTIVE_HOUR = 12;

export default {
  components: { MapLegend },

  setup() {
    const mapContainer = ref(null);

    let map = null;
    let aqMap = null; // Map<boroughName, {pm25[], co[], co2[]}>
    let baseGeojson = null; // original borough boundaries
    let popup = null; // Mapbox Popup instance
    let popupApp = null; // Vue app instance for BoroughPopup

    const resetScroll = () => {
      window.scrollTo({ top: 0, left: 0, behavior: "instant" });
    };

    const getValueForHour = (arr) => {
      if (!Array.isArray(arr) || arr.length === 0) return null;
      return arr[ACTIVE_HOUR] ?? arr[0];
    };

    // Build FeatureCollection with pm25/co/co2 for the current hour
    const buildStyledGeojson = () => {
      if (!baseGeojson || !aqMap) return null;

      return {
        type: "FeatureCollection",
        features: baseGeojson.features.map((f) => {
          const name =
            f.properties.NAME ||
            f.properties.BOROUGH ||
            f.properties.LAD13NM ||
            f.properties.name;

          const hourly = aqMap.get(name) || {};
          const pm25 = getValueForHour(hourly.pm25);
          const co = getValueForHour(hourly.co);
          const co2 = getValueForHour(hourly.co2);

          return {
            ...f,
            properties: {
              ...f.properties,
              BoroughName: name,
              pm25,
              co,
              co2,
            },
          };
        }),
      };
    };

    const addBoroughLayers = async () => {
      try {
        // Load boundaries + test data from /public
        const [geoRes, aqRes] = await Promise.all([
          fetch("/london_boroughs.geojson"),
          fetch("/borough_air_quality.json"),
        ]);

        const geojson = await geoRes.json();
        const aq = await aqRes.json();

        // Map borough -> hourly data
        aqMap = new Map(aq.data.map((entry) => [entry.borough, entry.hourly]));
        baseGeojson = geojson;

        const styledGeojson = buildStyledGeojson();

        // Data source
        if (!map.getSource(BOROUGH_SOURCE_ID)) {
          map.addSource(BOROUGH_SOURCE_ID, {
            type: "geojson",
            data: styledGeojson,
          });
        } else {
          map.getSource(BOROUGH_SOURCE_ID).setData(styledGeojson);
        }

        // Fill layer (color by pm2.5)
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

        // Outer glow
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

        // Borough borders
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

        // Borough labels
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

        // Fit to all boroughs
        const bounds = new mapboxgl.LngLatBounds();
        styledGeojson.features.forEach((f) => {
          const geom = f.geometry;
          const coords =
            geom.type === "Polygon"
              ? geom.coordinates.flat()
              : geom.coordinates.flat(2);
          coords.forEach(([lng, lat]) => bounds.extend([lng, lat]));
        });

        map.fitBounds(bounds, { padding: 40, maxZoom: 9.2 });

        // Hover cursor
        map.on("mouseenter", BOROUGH_FILL_ID, () => {
          map.getCanvas().style.cursor = "pointer";
        });
        map.on("mouseleave", BOROUGH_FILL_ID, () => {
          map.getCanvas().style.cursor = "";
        });

        // Click: show Vue-based popup with 3 metrics + chart
        map.on("click", BOROUGH_FILL_ID, (e) => {
          if (!e.features || !e.features.length) return;

          const feature = e.features[0];
          const props = feature.properties || {};

          const name =
            props.BoroughName ||
            props.NAME ||
            props.BOROUGH ||
            props.LAD13NM ||
            props.name ||
            "Unknown borough";

          const hourly = (aqMap && aqMap.get(name)) || {
            pm25: [],
            co: [],
            co2: [],
          };

          const pm25 =
            props.pm25 !== undefined && props.pm25 !== null
              ? Number(props.pm25)
              : getValueForHour(hourly.pm25);
          const co =
            props.co !== undefined && props.co !== null
              ? Number(props.co)
              : getValueForHour(hourly.co);
          const co2 =
            props.co2 !== undefined && props.co2 !== null
              ? Number(props.co2)
              : getValueForHour(hourly.co2);

          // Clean up previous popup & app
          if (popup) {
            popup.remove();
            popup = null;
          }
          if (popupApp) {
            popupApp.unmount();
            popupApp = null;
          }

          // Mount BoroughPopup.vue into a DOM node
          const container = document.createElement("div");
          popupApp = createApp(BoroughPopup, {
            name,
            hour: ACTIVE_HOUR,
            pm25,
            co,
            co2,
            hourly,
          });
          popupApp.mount(container);

          popup = new mapboxgl.Popup({
            closeButton: true,
            closeOnClick: false,
            offset: 16,
            //maxWidth: "340px",
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
          });
        });
      } catch (err) {
        console.error("Failed to load map or air quality data", err);
      }
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

      map.on("load", () => {
        addBoroughLayers();
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
      if (map) {
        map.remove();
        map = null;
      }
    });

    return { mapContainer };
  },
};
</script>
