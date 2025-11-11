<template>
  <div class="bp-card">
    <!-- Header -->
    <div class="bp-header">
      <div class="bp-label">AIR QUALITY SNAPSHOT</div>
      <div class="bp-title">{{ name }}</div>
      <div class="bp-subtitle">Hour {{ hour }}:00 · Backend hourly data</div>
    </div>

    <div class="bp-metrics">
      <div class="bp-metric highlight">
        <div class="bp-metric-label">PM2.5</div>
        <div class="bp-metric-value">
          {{ format(pm25) }}
          <span class="unit">g/hour</span>
        </div>
      </div>

      <div class="bp-metric">
        <div class="bp-metric-label">CO</div>
        <div class="bp-metric-value">
          {{ format(co) }}
          <span class="unit">g/hour</span>
        </div>
      </div>

      <div class="bp-metric">
        <div class="bp-metric-label">CO₂</div>
        <div class="bp-metric-value">
          {{ format(co2) }}
          <span class="unit">kg/hour</span>
        </div>
      </div>

      <div class="bp-metric traffic">
        <div class="bp-metric-label">Traffic Volume</div>
        <div class="bp-metric-value">
          {{ formatInt(vehicles) }}
          <span class="unit">vehicles/hour</span>
        </div>
      </div>
    </div>

    <!-- Divider -->
    <div class="bp-divider"></div>

    <button
      v-if="hasBusiestStreet"
      class="bp-route-btn"
      @click="emit('view-street')"
    >
      View recommended route
    </button>

    <!-- Chart -->
    <div class="bp-chart-header">
      <span>24h PM2.5 trend (g/hour)</span>
      <span class="bp-chart-hint"> live from backend · lower is better </span>
    </div>
    <div class="bp-chart-wrap">
      <canvas ref="chartCanvas"></canvas>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount, watch } from "vue";
import { Chart } from "chart.js/auto";

const API_BASE_URL = "http://10.129.111.25:3000";

const props = defineProps({
  name: { type: String, required: true },
  hour: { type: Number, required: true },
  pm25: { type: Number, default: null },
  co: { type: Number, default: null },
  co2: { type: Number, default: null },
  vehicles: { type: Number, default: null },

  hasBusiestStreet: { type: Boolean, default: false },
});

const emit = defineEmits(["view-street"]);

const chartCanvas = ref(null);
let chartInstance = null;

const hourlyPm25 = ref(Array(24).fill(null));

const format = (v) => {
  if (v === null || v === undefined || Number.isNaN(v)) return "N/A";
  return Number(v).toFixed(1);
};

const formatInt = (v) => {
  if (v === null || v === undefined || Number.isNaN(v)) return "N/A";
  return Math.round(Number(v)).toLocaleString();
};

const normalizeForBackend = (name) => {
  const lower = name.toLowerCase().trim();

  if (lower === "city of london") {
    return "city of london";
  }

  return lower
    .replace(/london borough of /g, "")
    .replace(/royal borough of /g, "")
    .replace(/city of /g, "")
    .replace(/&/g, "and")
    .replace(/\./g, "")
    .replace(/\s+/g, " ")
    .trim();
};

const fetchPm25Trend = async () => {
  const norm = normalizeForBackend(props.name);
  if (!norm) return;

  const values = [];

  for (let h = 0; h < 24; h++) {
    const key = `${norm}_${h}`;
    const url = `${API_BASE_URL}/api/hourly?borough=${encodeURIComponent(key)}`;

    try {
      const res = await fetch(url);
      if (!res.ok) {
        values.push(null);
        continue;
      }
      const data = await res.json();
      values.push(
        data && data.pm25_g_per_h !== undefined
          ? Number(data.pm25_g_per_h)
          : null
      );
    } catch (err) {
      console.error("Failed to fetch hourly pm25 for", key, err);
      values.push(null);
    }
  }

  hourlyPm25.value = values;
};

const buildChart = () => {
  const el = chartCanvas.value;
  if (!el) return;

  const src = Array.isArray(hourlyPm25.value) ? hourlyPm25.value : [];
  const labels = Array.from({ length: 24 }, (_, i) => i);

  const data = labels.map((i) => {
    const raw = src[i];
    if (raw === null || raw === undefined || Number.isNaN(raw)) {
      return null;
    }
    return Number(raw);
  });

  if (!data.some((v) => v !== null)) {
    if (chartInstance) {
      chartInstance.destroy();
      chartInstance = null;
    }
    return;
  }

  if (chartInstance) chartInstance.destroy();

  chartInstance = new Chart(el, {
    type: "line",
    data: {
      labels,
      datasets: [
        {
          data,
          fill: true,
          tension: 0.35,
          pointRadius: 0,
          borderWidth: 1.2,
        },
      ],
    },
    options: {
      responsive: true,
      maintainAspectRatio: false,
      plugins: {
        legend: { display: false },
        tooltip: {
          enabled: true,
          backgroundColor: "#050816",
          borderColor: "#a855f7",
          borderWidth: 1,
          titleFont: { size: 9 },
          bodyFont: { size: 9 },
          padding: 6,
          displayColors: false,
          callbacks: {
            title: (items) => `Hour ${items[0].label}:00`,
            label: (ctx) => `PM2.5: ${format(ctx.parsed.y)} g/hour`,
          },
        },
      },
      scales: {
        x: {
          display: true,
          grid: { display: false },
          ticks: {
            color: "#6b7280",
            font: { size: 7 },
            maxTicksLimit: 8,
          },
        },
        y: {
          display: true,
          grid: { color: "rgba(148,163,253,0.12)" },
          ticks: {
            color: "#6b7280",
            font: { size: 7 },
            maxTicksLimit: 4,
          },
        },
      },
    },
  });
};

onMounted(async () => {
  await fetchPm25Trend();
  buildChart();
});

watch(
  () => props.name,
  async () => {
    await fetchPm25Trend();
    buildChart();
  }
);

onBeforeUnmount(() => {
  if (chartInstance) {
    chartInstance.destroy();
    chartInstance = null;
  }
});
</script>

<style scoped>
.bp-card {
  width: 480px;
  height: 405px;
  padding: 15px 18px 14px;
  background: radial-gradient(
      circle at top,
      rgba(168, 85, 247, 0.16),
      transparent
    ),
    rgba(5, 5, 12, 0.98);
  border-radius: 21px;
  border: 1px solid rgba(168, 85, 247, 0.9);
  box-shadow: 0 0 36px rgba(168, 85, 247, 0.45);
  backdrop-filter: blur(10px);
  color: #e5e7eb;
  box-sizing: border-box;
}

.bp-header {
  margin-bottom: 6px;
}

.bp-label {
  display: inline-flex;
  padding: 3px 9px;
  border-radius: 999px;
  border: 1px solid rgba(168, 85, 247, 0.6);
  font-size: 10.5px;
  letter-spacing: 0.12em;
  color: #a855f7;
}

.bp-title {
  font-family: "Space Grotesk", sans-serif;
  font-weight: 700;
  font-size: 21px;
  letter-spacing: 0.5px;
  color: #f3f4f6;
}

.bp-subtitle {
  margin-top: 2px;
  font-size: 12px;
  color: #9ca3af;
}

.bp-metrics {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 9px;
  margin-top: 9px;
}

.bp-metric.traffic {
  grid-column: span 3;
}

.bp-metric {
  padding: 6px 6px 5px;
  border-radius: 9px;
  background: rgba(10, 10, 18, 0.98);
  border: 1px solid rgba(148, 163, 253, 0.14);
}

.bp-metric.highlight {
  border-color: #22c55e;
  box-shadow: 0 0 12px rgba(34, 197, 94, 0.35);
}

.bp-metric-label {
  font-size: 10.5px;
  color: #9ca3af;
}

.bp-metric-value {
  font-size: 15px;
  font-weight: 600;
  line-height: 1.2;
}

.bp-metric-value .unit {
  margin-left: 3px;
  font-size: 10.5px;
  color: #9ca3af;
}

.bp-divider {
  margin: 9px 0 6px;
  height: 1.5px;
  background: linear-gradient(
    to right,
    transparent,
    rgba(168, 85, 247, 0.7),
    transparent
  );
}

/* 新增按钮样式 */
.bp-route-btn {
  width: 100%;
  margin: 0 0 6px;
  padding: 6px 10px;
  border: none;
  border-radius: 9px;
  background: linear-gradient(90deg, #ff3b30, #fb923c);
  color: #ffffff;
  font-size: 11px;
  font-weight: 600;
  cursor: pointer;
  text-align: center;
}

.bp-route-btn:hover {
  opacity: 0.95;
}

.bp-chart-header {
  display: flex;
  justify-content: space-between;
  align-items: baseline;
  font-size: 10.5px;
  color: #9ca3af;
  margin-bottom: 3px;
}

.bp-chart-hint {
  font-size: 9px;
  color: #6b7280;
}

.bp-chart-wrap {
  width: 100%;
  height: 120px;
}
</style>
