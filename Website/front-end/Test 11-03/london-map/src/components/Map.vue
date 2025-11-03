<template>
  <div ref="mapContainer" class="map-container"></div>
</template>

<script>
import mapboxgl from "mapbox-gl";
import "mapbox-gl/dist/mapbox-gl.css";

mapboxgl.accessToken =
  "pk.eyJ1IjoieG1zMTIxMzgiLCJhIjoiY21oaWw5cnRoMDIyazJpc2Q4azF3ZW81eCJ9.p5vgHrhM1hR6Bvl-vjmO7A";

export default {
  mounted() {
    const map = new mapboxgl.Map({
      container: this.$refs.mapContainer,
      style: "mapbox://styles/mapbox/standard",
    });

    // assign the map instance to this component's map property
    this.map = map;
  },

  // clean up the map instance when the component is unmounted
  unmounted() {
    this.map.remove();
    this.map = null;
  },
};
</script>

<style>
/* make the map container fill its parent */
.map-container {
  width: 100%;
  height: 100%;
}
</style>
