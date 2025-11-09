import { createRouter, createWebHistory } from "vue-router";
import Home from "../views/Home.vue";
import Page2 from "../views/Page2.vue";
import Map from "../views/Map.vue";
import Page4 from "../views/Page4.vue";

const routes = [
  { path: "/", name: "Home", component: Home },
  { path: "/page2", name: "Page2", component: Page2 },
  { path: "/map", name: "Map", component: Map },
  { path: "/page4", name: "Page4", component: Page4 },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
  scrollBehavior(to, from, savedPosition) {
    if (savedPosition) {
      return savedPosition;
    }
    return { left: 0, top: 0 };
  },
});

export default router;
