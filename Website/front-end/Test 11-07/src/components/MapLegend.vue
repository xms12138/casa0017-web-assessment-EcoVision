<template>
  <div class="map-legend">
    <div class="legend-title">{{ title }}</div>
    <div v-for="(item, index) in levels" :key="index" class="legend-row">
      <span class="legend-color" :style="{ background: item.color }"></span>
      <span>{{ item.label }}</span>
    </div>
    <div class="legend-note">{{ note }}</div>
  </div>
</template>

<script setup>
defineProps({
  title: {
    type: String,
    default: "PM2.5 Emissions (g/hour)",
  },
  note: {
    type: String,
    default: "* Based on backend hourly emissions",
  },
  levels: {
    type: Array,
    default: () => [
      { color: "#22c55e", label: "0 – 200 Very Low" },
      { color: "#84cc16", label: "200 – 260 Low" },
      { color: "#eab308", label: "260 – 340 Moderate" },
      { color: "#f97316", label: "340 – 430 High" },
      { color: "#ef4444", label: "430 – 550 Very High" },
      { color: "#7f1d1d", label: "> 550 Extreme" },
    ],
  },
});
</script>

<style scoped>
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
</style>
