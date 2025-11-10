<template>
  <div class="time-slider-container">
    <div class="time-slider-header">
      <span class="time-label">TIME SCALE</span>
      <span class="time-value">{{ timeRangeDisplay }}</span>
    </div>

    <div class="color-bar"></div>

    <div class="tick-row">
      <button
        v-for="tick in ticks"
        :key="tick"
        class="tick"
        :class="{
          'is-major': tick % 6 === 0,
          'is-active': tick === localHour,
        }"
        @click="onSelect(tick)"
      >
        {{ tick }}
      </button>
    </div>
  </div>
</template>

<script setup>
import { ref, watch, computed } from "vue";

const props = defineProps({
  modelValue: {
    type: Number,
    default: 0, // 0–23
  },
});

const emit = defineEmits(["update:modelValue"]);

const localHour = ref(props.modelValue);
const ticks = Array.from({ length: 24 }, (_, i) => i);

watch(
  () => props.modelValue,
  (val) => {
    if (val !== localHour.value) {
      localHour.value = val;
    }
  }
);

const pad = (n) => String(n).padStart(2, "0");

const timeRangeDisplay = computed(() => {
  const start = localHour.value;
  const end = localHour.value + 1;
  return `${pad(start)}.00–${pad(end)}.00`;
});

const onSelect = (tick) => {
  if (tick === localHour.value) return;
  localHour.value = tick;
  emit("update:modelValue", localHour.value);
};
</script>

<style scoped>
.time-slider-container {
  position: absolute;
  top: 20px;
  left: 50%;
  transform: translateX(-50%);
  z-index: 15;
  width: 760px;
  max-width: 94vw;
  padding: 16px 26px 20px;
  border-radius: 22px;
  background: rgba(18, 21, 34, 0.92);
  border: 1px solid rgba(139, 92, 246, 0.6);
  backdrop-filter: blur(16px);
  box-shadow: 0 8px 32px rgba(0, 0, 0, 0.45);
  transition: all 0.25s ease;
}

.time-slider-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 10px;
  padding: 0 4px;
}

.time-label {
  font-size: 13px;
  letter-spacing: 0.16em;
  color: #9ca3af;
  text-transform: uppercase;
}

.time-value {
  font-size: 24px;
  font-weight: 700;
  color: #f9fafb;
}

.color-bar {
  width: 100%;
  height: 8px;
  border-radius: 999px;
  background: linear-gradient(
    to right,
    #22c55e 0%,
    #84cc16 20%,
    #eab308 45%,
    #f97316 70%,
    #ef4444 100%
  );
  opacity: 0.95;
  margin-bottom: 14px;
  box-shadow: 0 0 10px rgba(139, 92, 246, 0.25);
}

.tick-row {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.tick {
  background: transparent;
  border: none;
  padding: 4px 0;
  margin: 0;
  font-size: 13px;
  color: #9ca3af;
  font-weight: 500;
  opacity: 0.75;
  cursor: pointer;
  transition: all 0.2s ease;
  user-select: none;
  border-radius: 6px;
}

.tick.is-major {
  font-size: 14px;
  color: #d1d5db;
  font-weight: 600;
}

.tick.is-active {
  color: #f9fafb;
  font-weight: 700;
  background: rgba(139, 92, 246, 0.25);
  box-shadow: 0 0 14px rgba(139, 92, 246, 0.5);
  transform: translateY(-2px) scale(1.15);
}

.tick:hover {
  color: #ffffff;
  opacity: 1;
  transform: translateY(-1px) scale(1.08);
  background: rgba(139, 92, 246, 0.18);
}

.time-slider-container:hover {
  border-color: rgba(196, 181, 253, 0.9);
  box-shadow: 0 0 26px rgba(129, 140, 248, 0.35);
}

@media (max-width: 600px) {
  .time-slider-container {
    max-width: 96vw;
    padding: 14px 18px 16px;
  }

  .time-value {
    font-size: 20px;
  }

  .tick {
    font-size: 11px;
  }
}
</style>
