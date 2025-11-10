<template>
  <div class="route-card">
    <div class="route-header">
      <div class="route-title">{{ street }}</div>
      <div class="route-subtitle">Hour {{ hour }}:00 · Car vs Bicycle</div>
    </div>

    <div class="route-grid">
      <div class="route-col">
        <div class="label label-car">BY CAR</div>
        <div class="time">
          {{ display(carTime) }}<span v-if="carTime !== null"> min</span>
        </div>
        <div class="metric">CO₂: {{ fixed(carCo2) }} kg</div>
        <div class="metric">CO: {{ fixed(carCo) }} g</div>
        <div class="metric">PM2.5: {{ fixed(carPm25) }} g</div>
      </div>

      <div class="route-col">
        <div class="label label-bike">BY BICYCLE</div>
        <div class="time">
          {{ display(bikeTime) }}<span v-if="bikeTime !== null"> min</span>
        </div>
        <div class="metric">CO₂: 0 kg</div>
        <div class="metric">CO: 0 g</div>
        <div class="metric">PM2.5: 0 g</div>
      </div>
    </div>

    <div class="route-chart-title">
      Visualised comparison
      <span>(time & emissions)</span>
    </div>
    <div class="route-chart" ref="chartRef"></div>

    <div class="route-note" v-if="note">
      {{ note }}
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount, watch, nextTick } from "vue";
import * as echarts from "echarts";

const props = defineProps({
  street: { type: String, required: true },
  hour: { type: Number, required: true },
  carTime: { type: Number, default: null },
  bikeTime: { type: Number, default: null },
  carCo2: { type: Number, default: null },
  carCo: { type: Number, default: null },
  carPm25: { type: Number, default: null },
});

const chartRef = ref(null);
let chart = null;

const display = (v) =>
  v === null || Number.isNaN(v) ? "N/A" : Number(v).toFixed(0);

const fixed = (v) =>
  v === null || Number.isNaN(v) ? "N/A" : Number(v).toFixed(2);

const buildNote = () => {
  const c = props.carTime;
  const b = props.bikeTime;
  if (c == null || b == null || Number.isNaN(c) || Number.isNaN(b)) return "";
  const diff = c - b;
  if (diff > 0) return `Cycling is ${diff} min faster on this route.`;
  if (diff < 0)
    return `Driving is ${Math.abs(
      diff
    )} min faster, but with significantly higher emissions.`;
  return "Both modes take similar time, but cycling avoids tailpipe emissions.";
};
const note = buildNote();

const renderChart = () => {
  if (!chartRef.value) return;

  if (chart) {
    chart.dispose();
  }

  const el = chartRef.value;
  if (!el.offsetWidth || !el.offsetHeight) {
    // 容器还没铺开，晚一点再画
    setTimeout(renderChart, 30);
    return;
  }

  chart = echarts.init(el);

  const carTime = props.carTime ?? 0;
  const bikeTime = props.bikeTime ?? 0;
  const carCo2 = props.carCo2 ?? 0;
  const carPm25 = props.carPm25 ?? 0;

  const option = {
    animation: true,
    tooltip: {
      trigger: "axis",
      axisPointer: { type: "shadow" },
      textStyle: { fontSize: 9 },
    },
    legend: {
      data: ["Car", "Bicycle"],
      top: 0,
      itemWidth: 10,
      itemHeight: 6,
      textStyle: {
        color: "#e5e7eb",
        fontSize: 8,
      },
    },
    grid: {
      left: 32,
      right: 6,
      top: 20,
      bottom: 16,
    },
    xAxis: {
      type: "category",
      data: ["Time", "CO₂", "PM2.5"],
      axisLabel: {
        color: "#9ca3af",
        fontSize: 8,
      },
      axisLine: {
        lineStyle: { color: "rgba(148,163,253,0.4)" },
      },
    },
    yAxis: {
      type: "value",
      axisLabel: {
        color: "#6b7280",
        fontSize: 7,
      },
      splitLine: {
        lineStyle: { color: "rgba(148,163,253,0.16)" },
      },
    },
    series: [
      {
        name: "Car",
        type: "bar",
        data: [carTime, carCo2, carPm25],
        barWidth: 9,
      },
      {
        name: "Bicycle",
        type: "bar",
        data: [bikeTime, 0, 0],
        barWidth: 9,
      },
    ],
  };

  chart.setOption(option);
  chart.resize();
};

onMounted(async () => {
  await nextTick();
  renderChart();
});

watch(
  () => [
    props.carTime,
    props.bikeTime,
    props.carCo2,
    props.carPm25,
    props.street,
  ],
  async () => {
    await nextTick();
    renderChart();
  }
);

onBeforeUnmount(() => {
  if (chart) {
    chart.dispose();
    chart = null;
  }
});
</script>

<style scoped>
.route-card {
  width: 340px; /* ✅ 比之前小一圈 */
  padding: 10px 12px 10px;
  background: radial-gradient(
      circle at top,
      rgba(255, 0, 136, 0.22),
      transparent
    ),
    rgba(0, 0, 0, 0.98);
  border-radius: 14px;
  border: 2px solid #ff00ff;
  box-shadow: 0 0 26px rgba(255, 0, 255, 0.5);
  color: #e5e7eb;
  box-sizing: border-box;
  font-family: "Space Grotesk", sans-serif;
}

.route-header {
  margin-bottom: 4px;
}

.route-title {
  font-family: "Press Start 2P", cursive;
  font-size: 0.6rem;
  color: #ff00ff;
  text-shadow: 0 0 8px rgba(255, 0, 255, 0.9);
}

.route-subtitle {
  font-size: 0.5rem;
  color: #9ca3af;
  margin-top: 1px;
}

.route-grid {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 6px;
  margin-top: 4px;
}

.route-col {
  padding: 5px 6px;
  border-radius: 8px;
  background: rgba(7, 7, 12, 0.98);
  border: 1px solid rgba(148, 163, 253, 0.22);
}

.label {
  font-size: 0.48rem;
  margin-bottom: 1px;
}

.label-car {
  color: #fb923c;
}

.label-bike {
  color: #22c55e;
}

.time {
  font-size: 0.95rem;
  font-weight: 700;
  line-height: 1.1;
}

.time span {
  font-size: 0.48rem;
  margin-left: 2px;
  color: #9ca3af;
}

.metric {
  font-size: 0.48rem;
  color: #9ca3af;
  line-height: 1.25;
}

.route-chart-title {
  margin-top: 4px;
  font-size: 0.5rem;
  color: #9ca3af;
  display: flex;
  justify-content: space-between;
}

.route-chart-title span {
  font-size: 0.44rem;
  color: #6b7280;
}

.route-chart {
  margin-top: 2px;
  width: 100%;
  height: 64px; /* ✅ 图表更小巧 */
}

.route-note {
  margin-top: 3px;
  font-size: 0.48rem;
  color: #e5e7eb;
}
</style>
