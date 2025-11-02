import "./style.css";
import { Map } from "maplibre-gl";
import "maplibre-gl/dist/maplibre-gl.css";

import { MapboxOverlay } from "@deck.gl/mapbox";
import { ScatterplotLayer } from "@deck.gl/layers";

const MAPTILER_KEY = "LOsRZZhtrk16JU81NWOw"; // ← 换成你的 key

// 1) 初始化 MapLibre + MapTiler 底图
const map = new Map({
  container: "map",
  style: `https://api.maptiler.com/maps/basic-v2/style.json?key=${MAPTILER_KEY}`,
  interactive: true,
  center: [-0.12262486445294093, 51.50756471490389],
  zoom: 12,
});

// 2) 等样式加载完成（确保能 addSource / addLayer）
await new Promise((resolve) => map.once("load", resolve));

// 3) 复用样式里已有的 openmaptiles 源；若没有则新增（避免重名报错）
const styleSources = map.getStyle().sources || {};
const sourceId = styleSources.openmaptiles ? "openmaptiles" : "omt3d";
if (sourceId === "omt3d") {
  map.addSource("omt3d", {
    url: `https://api.maptiler.com/tiles/v3/tiles.json?key=${MAPTILER_KEY}`,
    type: "vector",
  });
}

// 4) 找到第一个文本 symbol 层，把 3D 建筑插到其下方，避免盖住标注
const firstSymbolId = map
  .getStyle()
  .layers.find(
    (l) => l.type === "symbol" && l.layout && l.layout["text-field"]
  )?.id;

// 5) 添加 3D 建筑层（按教程；修正 zoom 表达式 & 兼容属性名）
map.addLayer(
  {
    id: "3d-buildings",
    source: sourceId,
    "source-layer": "building",
    type: "fill-extrusion",
    minzoom: 10,
    // 教程中的 hide_3d 过滤先保留；如看不到可临时注释掉此行
    filter: ["!=", ["get", "hide_3d"], true],
    paint: {
      "fill-extrusion-color": [
        "interpolate",
        ["linear"],
        // 兼容不同数据集：优先 render_height，其次 height
        ["coalesce", ["get", "render_height"], ["get", "height"], 0],
        0,
        "lightgray",
        200,
        "royalblue",
        400,
        "lightblue",
      ],
      // 关键修正：不要在小倍率把高度插成 0，直接使用高度属性
      "fill-extrusion-height": [
        "coalesce",
        ["get", "render_height"],
        ["get", "height"],
        0,
      ],
      "fill-extrusion-base": [
        "coalesce",
        ["get", "render_min_height"],
        ["get", "min_height"],
        0,
      ],
      "fill-extrusion-opacity": 0.9,
    },
  },
  firstSymbolId
);

// 6) 倾斜视角更容易看出 3D；也可放大一点更直观
map.easeTo({ pitch: 60, bearing: -20, duration: 600 });

// 7) 你的 deck.gl 圆点（可交互示例）
const layerCircle = new ScatterplotLayer({
  id: "deckgl-circle",
  data: [{ position: [-0.1226, 51.5075], name: "London Center", value: 42 }],
  getPosition: (d) => d.position,
  getFillColor: [242, 133, 0, 200],
  getRadius: 1000,
  pickable: true,
  onClick: (info) => {
    if (info.object) {
      alert(`You clicked: ${info.object.name}\nValue: ${info.object.value}`);
    }
  },
  onHover: (info) => {
    document.body.style.cursor = info.object ? "pointer" : "default";
  },
});

// 8) 叠加 deck.gl 到 MapLibre
const deckOverlay = new MapboxOverlay({
  interleaved: true,
  layers: [layerCircle],
});
map.addControl(deckOverlay);
