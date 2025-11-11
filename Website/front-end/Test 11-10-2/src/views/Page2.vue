<template>
  <div class="page2">
    <nav>
      <div class="logo">ECOVISION</div>
      <ul class="nav-links">
        <li><router-link to="/">HOME</router-link></li>
        <li><router-link to="/page2" class="active">ABOUT</router-link></li>
        <li><router-link to="/map">MAP</router-link></li>
        <li><router-link to="/page4">TEAM</router-link></li>
      </ul>
    </nav>

    <div class="pixel-decoration top-left"></div>
    <div class="pixel-decoration bottom-right"></div>

    <section class="hero">
      <div class="hero-content">
        <h1>Ecovision London</h1>
        <p>
          Mapping London's vehicle emissions to air quality - Empowering
          citizens with real-time data for cleaner transport decisions
        </p>
      </div>
    </section>

    <section class="challenge-section">
      <h2 class="section-title">The Challenge We're Solving</h2>
      <div class="challenge-grid">
        <div class="challenge-card">
          <span class="card-icon">🏥</span>
          <h3 class="card-title">Health Impact</h3>
          <p class="card-text">
            Vehicle emissions directly affect public health and urban
            sustainability across London's 33 boroughs
          </p>
        </div>
        <div class="challenge-card">
          <span class="card-icon">📊</span>
          <h3 class="card-title">Data Complexity</h3>
          <p class="card-text">
            Complex air quality reports are difficult for citizens to interpret
            and use for daily decisions
          </p>
        </div>
        <div class="challenge-card">
          <span class="card-icon">💡</span>
          <h3 class="card-title">Our Solution</h3>
          <p class="card-text">
            Interactive visual mapping with hourly data to help citizens make
            informed transport choices
          </p>
        </div>
      </div>
    </section>

    <section class="features-section">
      <h2 class="section-title">Key Features</h2>
      <div class="features-list">
        <div class="feature-item">
          <span class="feature-icon">🗺️</span>
          <div class="feature-content">
            <h3>Real-time Borough Mapping</h3>
            <p>
              Interactive map of all 33 London boroughs with hourly air quality
              data visualization
            </p>
          </div>
        </div>
        <div class="feature-item">
          <span class="feature-icon">⏰</span>
          <div class="feature-content">
            <h3>24-Hour Time Analysis</h3>
            <p>
              Track pollution patterns throughout the day with interactive time
              sliders
            </p>
          </div>
        </div>
        <div class="feature-item">
          <span class="feature-icon">🚴</span>
          <div class="feature-content">
            <h3>Cycling vs Car Comparison</h3>
            <p>
              Compare journey times and environmental impact between cycling and
              driving
            </p>
          </div>
        </div>
      </div>
    </section>

    <section class="cta-section">
      <div class="cta-box">
        <h2>Explore the Interactive Map</h2>
        <p style="color: #ccc; margin-bottom: 2rem; line-height: 1.8">
          Coming soon - Full interactive experience with historical data from
          previous month
        </p>
        <div class="cta-buttons">
          <router-link to="/map" class="pixel-button primary"
            >View Map</router-link
          >
          <router-link to="/page4" class="pixel-button secondary"
            >Join Us</router-link
          >
        </div>
      </div>
    </section>
  </div>
</template>

<script setup>
import { onMounted, onActivated } from "vue";

function resetScroll() {
  window.scrollTo(0, 0);
  document.body.scrollTop = 0;
  document.documentElement.scrollTop = 0;

  const app = document.getElementById("app");
  if (app) app.scrollTop = 0;
}

onMounted(() => {
  resetScroll();

  const observerOptions = { threshold: 0.1, rootMargin: "0px 0px -100px 0px" };
  const observer = new IntersectionObserver((entries) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        entry.target.style.opacity = "1";
        entry.target.style.transform = "translateY(0)";
      }
    });
  }, observerOptions);

  document.querySelectorAll(".challenge-card, .feature-item").forEach((el) => {
    el.style.opacity = "0";
    el.style.transform = "translateY(50px)";
    el.style.transition = "all 0.6s ease";
    observer.observe(el);
  });
});

onActivated(() => {
  resetScroll();
});
</script>

<style scoped>
body {
  font-family: "Space Grotesk", sans-serif;
  background: #0a0a0f;
  color: #fff;
  overflow-x: hidden;
}

nav {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  background: rgba(0, 0, 0, 0.95);
  border-bottom: 2px solid #a855f7;
  padding: 1rem 2rem;
  z-index: 100;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.logo {
  font-family: "Press Start 2P", cursive;
  font-size: 0.8rem;
  color: #a855f7;
  text-shadow: 0 0 10px rgba(168, 85, 247, 0.8);
}

.nav-links {
  display: flex;
  gap: 2rem;
  list-style: none;
}

.nav-links a {
  color: #fff;
  text-decoration: none;
  font-family: "Press Start 2P", cursive;
  font-size: 0.6rem;
  transition: all 0.3s ease;
}

.nav-links a:hover {
  color: #a855f7;
  text-shadow: 0 0 10px rgba(168, 85, 247, 0.8);
}

.nav-links a.active {
  color: #ff0088;
}

.hero {
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: linear-gradient(rgba(0, 0, 0, 0.7), rgba(0, 0, 0, 0.7)),
    url('data:image/svg+xml,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1200 600"><rect fill="%23000" width="1200" height="600"/><g fill="%2300ff88" opacity="0.1"><circle cx="100" cy="100" r="50"/><circle cx="400" cy="200" r="80"/><circle cx="800" cy="300" r="60"/><circle cx="1000" cy="450" r="70"/></g></svg>');
  background-size: cover;
  background-attachment: fixed;
  position: relative;
  padding: 8rem 2rem 4rem;
}

.hero::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: repeating-linear-gradient(
    0deg,
    transparent,
    transparent 2px,
    rgba(0, 255, 136, 0.03) 2px,
    rgba(0, 255, 136, 0.03) 4px
  );
  pointer-events: none;
}

.hero-content {
  max-width: 900px;
  text-align: center;
  position: relative;
  z-index: 1;
}

.hero h1 {
  font-family: "Press Start 2P", cursive;
  font-size: clamp(1.5rem, 5vw, 3rem);
  color: #a855f7;
  margin-bottom: 2rem;
  text-shadow: 0 0 20px rgba(168, 85, 247, 0.8), 3px 3px 0 #000;
  animation: glow 2s ease-in-out infinite;
}

@keyframes glow {
  0%,
  100% {
    text-shadow: 0 0 20px rgba(168, 85, 247, 0.8), 3px 3px 0 #000;
  }
  50% {
    text-shadow: 0 0 40px rgba(168, 85, 247, 1), 3px 3px 0 #000;
  }
}

.hero p {
  font-size: 1.2rem;
  line-height: 1.8;
  color: #ccc;
  margin-bottom: 2rem;
}

.challenge-section {
  padding: 6rem 2rem;
  background: linear-gradient(180deg, #0a0a0f 0%, #1a0a2e 100%);
}

.section-title {
  text-align: center;
  font-family: "Press Start 2P", cursive;
  font-size: clamp(1.2rem, 3vw, 2rem);
  color: #ff0088;
  margin-bottom: 4rem;
  text-shadow: 0 0 20px rgba(255, 0, 136, 0.8);
}

.challenge-grid {
  max-width: 1200px;
  margin: 0 auto;
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
  gap: 3rem;
}

.challenge-card {
  background: rgba(0, 0, 0, 0.6);
  border: 2px solid #a855f7;
  padding: 2rem;
  position: relative;
  transition: all 0.3s ease;
}

.challenge-card::before {
  content: "";
  position: absolute;
  top: -2px;
  left: -2px;
  right: -2px;
  bottom: -2px;
  background: linear-gradient(45deg, #a855f7, #ff0088);
  z-index: -1;
  opacity: 0;
  transition: opacity 0.3s ease;
}

.challenge-card:hover::before {
  opacity: 0.3;
}

.challenge-card:hover {
  transform: translateY(-10px);
  box-shadow: 0 20px 40px rgba(168, 85, 247, 0.3);
}

.card-icon {
  font-size: 3rem;
  margin-bottom: 1rem;
  display: block;
  animation: float 3s ease-in-out infinite;
}

@keyframes float {
  0%,
  100% {
    transform: translateY(0);
  }
  50% {
    transform: translateY(-10px);
  }
}

.card-title {
  font-family: "Press Start 2P", cursive;
  font-size: 0.9rem;
  color: #a855f7;
  margin-bottom: 1rem;
  line-height: 1.6;
}

.card-text {
  color: #ccc;
  line-height: 1.8;
  font-size: 0.95rem;
}

.features-section {
  padding: 6rem 2rem;
  background: #0a0a0f;
}

.features-list {
  max-width: 1000px;
  margin: 0 auto;
}

.feature-item {
  display: flex;
  align-items: flex-start;
  gap: 2rem;
  margin-bottom: 3rem;
  padding: 2rem;
  background: rgba(168, 85, 247, 0.05);
  border-left: 4px solid #a855f7;
  transition: all 0.3s ease;
}

.feature-item:hover {
  background: rgba(168, 85, 247, 0.1);
  transform: translateX(10px);
}

.feature-icon {
  font-size: 3rem;
  flex-shrink: 0;
}

.feature-content h3 {
  font-family: "Press Start 2P", cursive;
  font-size: 0.9rem;
  color: #a855f7;
  margin-bottom: 1rem;
  line-height: 1.6;
}

.feature-content p {
  color: #ccc;
  line-height: 1.8;
}

.cta-section {
  padding: 6rem 2rem;
  background: linear-gradient(180deg, #1a0a2e 0%, #0a0a0f 100%);
  text-align: center;
}

.cta-box {
  max-width: 800px;
  margin: 0 auto;
  padding: 3rem;
  background: rgba(0, 0, 0, 0.6);
  border: 3px solid #ff0088;
  box-shadow: 0 0 40px rgba(255, 0, 136, 0.3);
}

.cta-box h2 {
  font-family: "Press Start 2P", cursive;
  font-size: clamp(1rem, 3vw, 1.5rem);
  color: #ff0088;
  margin-bottom: 2rem;
  line-height: 1.8;
}

.cta-buttons {
  display: flex;
  gap: 2rem;
  justify-content: center;
  flex-wrap: wrap;
}

.pixel-button {
  display: inline-block;
  padding: 1rem 2rem;
  font-family: "Press Start 2P", cursive;
  font-size: 0.7rem;
  text-decoration: none;
  border: 3px solid;
  box-shadow: 5px 5px 0 #000;
  transition: all 0.1s ease;
  cursor: pointer;
  text-transform: uppercase;
}

.pixel-button.primary {
  background: #a855f7;
  color: #fff;
  border-color: #a855f7;
}

.pixel-button.secondary {
  background: #ff0088;
  color: #fff;
  border-color: #ff0088;
}

.pixel-button:hover {
  transform: translate(2px, 2px);
  box-shadow: 3px 3px 0 #000;
}

.pixel-button:active {
  transform: translate(5px, 5px);
  box-shadow: none;
}

.pixel-decoration {
  position: fixed;
  pointer-events: none;
  z-index: 0;
  opacity: 0.1;
}

.pixel-decoration.top-left {
  top: 100px;
  left: 50px;
  width: 100px;
  height: 100px;
  background: #00ff88;
  clip-path: polygon(0 0, 100% 0, 50% 100%);
  animation: rotate 10s linear infinite;
}

.pixel-decoration.bottom-right {
  bottom: 100px;
  right: 50px;
  width: 150px;
  height: 150px;
  background: #ff0088;
  clip-path: polygon(50% 0, 100% 50%, 50% 100%, 0 50%);
  animation: rotate 15s linear infinite reverse;
}

@keyframes rotate {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}

@media (max-width: 768px) {
  nav {
    flex-direction: column;
    gap: 1rem;
  }
  .nav-links {
    gap: 1rem;
  }
  .nav-links a {
    font-size: 0.5rem;
  }
  .challenge-grid {
    grid-template-columns: 1fr;
  }
  .feature-item {
    flex-direction: column;
  }
  .cta-buttons {
    flex-direction: column;
  }
}
</style>
