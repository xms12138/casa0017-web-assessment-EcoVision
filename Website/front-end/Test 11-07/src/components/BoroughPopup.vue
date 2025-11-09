<template>
  <div class="bp-card">
    <!-- Header -->
    <div class="bp-header">
      <div class="bp-label">AIR QUALITY SNAPSHOT</div>
      <div class="bp-title">{{ name }}</div>
      <div class="bp-subtitle">Hour {{ hour }}:00 · Sample data</div>
    </div>

    <!-- Metrics row -->
    <div class="bp-metrics">
      <div class="bp-metric highlight">
        <div class="bp-metric-label">PM2.5</div>
        <div class="bp-metric-value">
          {{ format(pm25) }}
          <span class="unit">µg/m³</span>
        </div>
      </div>
      <div class="bp-metric">
        <div class="bp-metric-label">CO</div>
        <div class="bp-metric-value">
          {{ format(co) }} <span class="unit">ppm</span>
        </div>
      </div>
      <div class="bp-metric">
        <div class="bp-metric-label">CO₂</div>
        <div class="bp-metric-value">
          {{ format(co2) }} <span class="unit">ppm</span>
        </div>
      </div>
    </div>

    <!-- Divider -->
    <div class="bp-divider"></div>

    <!-- Chart -->
    <div class="bp-chart-header">
      <span>24h PM2.5 trend</span>
      <span class="bp-chart-hint">mock data · lower is better</span>
    </div>
    <div class="bp-chart-wrap">
      <canvas ref="chartCanvas"></canvas>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount, watch } from "vue";
import { Chart } from "chart.js/auto";

const props = defineProps({
  name: { type: String, required: true },
  hour: { type: Number, required: true },
  pm25: { type: Number, default: null },
  co: { type: Number, default: null },
  co2: { type: Number, default: null },
  // { pm25: number[], co: number[], co2: number[] }
  hourly: {
    type: Object,
    default: () => ({ pm25: [], co: [], co2: [] }),
  },
});

const chartCanvas = ref(null);
let chartInstance = null;

const format = (v) => {
  if (v === null || v === undefined || Number.isNaN(v)) return "N/A";
  return Number(v).toFixed(1);
};

const buildChart = () => {
  const el = chartCanvas.value;
  if (!el) return;

  const src = Array.isArray(props.hourly.pm25) ? props.hourly.pm25 : [];
  const labels = Array.from({ length: 24 }, (_, i) => i);

  // 保证有 24 个点，不足补 null，多了截断
  const data = labels.map((i) => {
    const raw = src[i];
    if (raw === null || raw === undefined || Number.isNaN(raw)) return null;
    return Number(raw);
  });

  // 如果全是 null，就不画，避免难看一条线
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
            label: (ctx) => `PM2.5: ${format(ctx.parsed.y)} µg/m³`,
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

onMounted(buildChart);

watch(
  () => props.hourly,
  () => buildChart(),
  { deep: true }
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
  width: 300px;
  height: 250px;
  padding: 10px 12px 9px;
  background: radial-gradient(
      circle at top,
      rgba(168, 85, 247, 0.16),
      transparent
    ),
    rgba(5, 5, 12, 0.98);
  border-radius: 14px;
  border: 1px solid rgba(168, 85, 247, 0.9);
  box-shadow: 0 0 24px rgba(168, 85, 247, 0.45);
  backdrop-filter: blur(8px);
  color: #e5e7eb;
  box-sizing: border-box;
}

.bp-header {
  margin-bottom: 4px;
}

.bp-label {
  display: inline-flex;
  padding: 2px 6px;
  border-radius: 999px;
  border: 1px solid rgba(168, 85, 247, 0.6);
  font-size: 7px;
  letter-spacing: 0.12em;
  color: #a855f7;
}

.bp-title {
  font-family: "Space Grotesk", sans-serif;
  font-weight: 700;
  font-size: 14px;
  letter-spacing: 0.5px;
  color: #f3f4f6;
}

.bp-subtitle {
  margin-top: 1px;
  font-size: 8px;
  color: #9ca3af;
}

.bp-metrics {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 6px;
  margin-top: 6px;
}

.bp-metric {
  padding: 4px 4px 3px;
  border-radius: 6px;
  background: rgba(10, 10, 18, 0.98);
  border: 1px solid rgba(148, 163, 253, 0.14);
}

.bp-metric.highlight {
  border-color: #22c55e;
  box-shadow: 0 0 8px rgba(34, 197, 94, 0.35);
}

.bp-metric-label {
  font-size: 7px;
  color: #9ca3af;
}

.bp-metric-value {
  font-size: 10px;
  font-weight: 600;
  line-height: 1.15;
}

.bp-metric-value .unit {
  margin-left: 2px;
  font-size: 7px;
  color: #9ca3af;
}

.bp-divider {
  margin: 6px 0 4px;
  height: 1px;
  background: linear-gradient(
    to right,
    transparent,
    rgba(168, 85, 247, 0.7),
    transparent
  );
}

.bp-chart-header {
  display: flex;
  justify-content: space-between;
  align-items: baseline;
  font-size: 7px;
  color: #9ca3af;
  margin-bottom: 2px;
}

.bp-chart-hint {
  font-size: 6px;
  color: #6b7280;
}

.bp-chart-wrap {
  width: 100%;
  height: 60px;
}
</style>
