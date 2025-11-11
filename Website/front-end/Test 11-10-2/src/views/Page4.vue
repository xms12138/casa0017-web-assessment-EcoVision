<template>
  <div class="team-page">
    <!-- Navigation -->
    <nav>
      <div class="logo">ECOVISION</div>
      <ul class="nav-links">
        <li><router-link to="/">HOME</router-link></li>
        <li><router-link to="/page2">ABOUT</router-link></li>
        <li><router-link to="/map">MAP</router-link></li>
        <li><router-link to="/page4" class="active">TEAM</router-link></li>
      </ul>
    </nav>

    <div class="success-message" :class="{ show: showSuccess }">
      {{ successMsg }}
    </div>

    <div class="modal" :class="{ active: showModal }">
      <div class="modal-content">
        <h3 class="modal-title">ADMIN LOGIN</h3>
        <p style="color: #888; font-size: 0.9rem; margin-bottom: 1rem">
          Enter password to access admin mode
        </p>
        <input
          type="password"
          class="modal-input"
          v-model="password"
          @keyup.enter="login"
          placeholder="Enter admin password"
        />
        <div class="modal-buttons">
          <button class="modal-btn cancel" @click="showModal = false">
            CANCEL
          </button>
          <button class="modal-btn confirm" @click="login">LOGIN</button>
        </div>
      </div>
    </div>

    <div class="content">
      <!-- Hero Section -->
      <div class="hero">
        <h1 class="title">OUR TEAM</h1>
        <p class="subtitle">Making a Difference Through Data and Action</p>
      </div>

      <!-- Team Members Introduction Section -->
      <div class="section team-intro-section">
        <div class="box">
          <h2>Meet Our Team Members</h2>
          <p>
            Our dedicated team brings together expertise in data science,
            design, development, and research to create EcoVision London.
          </p>

          <div class="team-members-grid">
            <div class="member-card">
              <div class="member-avatar">
                <span class="avatar-emoji">🧑‍🎓</span>
              </div>
              <h3 class="member-name">Madina Diallo</h3>
              <p class="member-title">Project & Data Integration Lead</p>
              <p class="member-description">
                Leading project coordination and managing data integration from
                multiple London air quality and transport sources.
              </p>
            </div>

            <div class="member-card">
              <div class="member-avatar">
                <span class="avatar-emoji">🧑‍💻</span>
              </div>
              <h3 class="member-name">Matilda Nelson</h3>
              <p class="member-title">Research & Web Content Lead</p>
              <p class="member-description">
                Conducting research on London's air quality challenges and
                developing user-focused content strategies.
              </p>
            </div>

            <div class="member-card">
              <div class="member-avatar">
                <span class="avatar-emoji">🎨</span>
              </div>
              <h3 class="member-name">Vidan Gao</h3>
              <p class="member-title">Design & Visualisation Lead</p>
              <p class="member-description">
                Creating intuitive visual designs and interactive data
                visualizations for the London borough mapping system.
              </p>
            </div>

            <div class="member-card">
              <div class="member-avatar">
                <span class="avatar-emoji">📱</span>
              </div>
              <h3 class="member-name">Zihang He</h3>
              <p class="member-title">Technical & Web Development Lead</p>
              <p class="member-description">
                Developing the technical infrastructure and web platform for
                real-time air quality data visualization.
              </p>
            </div>

            <div class="member-card">
              <div class="member-avatar">
                <span class="avatar-emoji">▶️</span>
              </div>
              <h3 class="member-name">Qingshan Luo</h3>
              <p class="member-title">Testing & Editing Lead</p>
              <p class="member-description">
                Ensuring quality assurance, user testing, and content editing
                for optimal user experience and accuracy.
              </p>
            </div>
          </div>
        </div>
      </div>

      <!-- Team Section -->
      <div class="section">
        <div class="box">
          <h2>Our Team Structure</h2>
          <p>
            Our diverse team of experts combines environmental science, data
            analytics, and community engagement to create meaningful change.
          </p>

          <div class="team-grid">
            <div class="team-member">
              <div class="member-role">🔬</div>
              <h3>Research Team</h3>
              <p>Environmental scientists and data analysts</p>
            </div>
            <div class="team-member">
              <div class="member-role">💻</div>
              <h3>Tech Team</h3>
              <p>Software engineers and UX designers</p>
            </div>
            <div class="team-member">
              <div class="member-role">📢</div>
              <h3>Community Team</h3>
              <p>Outreach coordinators and educators</p>
            </div>
          </div>
        </div>
      </div>

      <!-- Impact Section -->
      <div class="section">
        <div class="box">
          <h2>Community Impact</h2>
          <p>
            EcoVision empowers Londoners to make informed decisions about their
            transport choices, contributing to a cleaner, healthier city for
            everyone.
          </p>

          <div class="impact-stats">
            <div class="impact-card">
              <div class="impact-icon">👥</div>
              <div class="impact-value">10,000+</div>
              <div class="impact-label">Active Users</div>
            </div>
            <div class="impact-card">
              <div class="impact-icon">🚴</div>
              <div class="impact-value">25%</div>
              <div class="impact-label">Increase in Cycling</div>
            </div>
            <div class="impact-card">
              <div class="impact-icon">🌍</div>
              <div class="impact-value">30T</div>
              <div class="impact-label">CO₂ Reduced (Annual)</div>
            </div>
          </div>
        </div>
      </div>

      <!-- Survey Section -->
      <div class="section survey-section">
        <div class="survey-container">
          <div class="survey-header">
            <h1 class="survey-title">SUPPORT OUR RESEARCH</h1>
            <p class="survey-subtitle">
              Help us understand London's transport preferences. Your feedback
              will inform policy recommendations to reduce vehicle emissions and
              improve air quality.
            </p>
          </div>

          <form class="survey-form" @submit.prevent="submitForm">
            <div class="form-group">
              <label class="form-label"
                >Do you prefer cycling over driving in London?</label
              >
              <select class="form-select" v-model="form.q1" required>
                <option value="">Select your preference</option>
                <option value="Yes, always">Yes, always prefer cycling</option>
                <option value="Yes, mostly">Yes, mostly prefer cycling</option>
                <option value="Depends">It depends on the situation</option>
                <option value="No, mostly">No, mostly prefer driving</option>
                <option value="No, always">No, always prefer driving</option>
              </select>
            </div>

            <div class="form-group">
              <label class="form-label"
                >How often do you cycle in London?</label
              >
              <select class="form-select" v-model="form.q2" required>
                <option value="">Select frequency</option>
                <option value="Daily">Daily</option>
                <option value="Several times a week">
                  Several times a week
                </option>
                <option value="Once a week">Once a week</option>
                <option value="Once a month">Once a month</option>
                <option value="Rarely">Rarely</option>
                <option value="Never">Never</option>
              </select>
            </div>

            <div class="form-group">
              <label class="form-label"
                >What's your main reason for your transport choice?</label
              >
              <select class="form-select" v-model="form.q3" required>
                <option value="">Select main reason</option>
                <option value="Environmental concerns">
                  Environmental concerns
                </option>
                <option value="Health and fitness">Health and fitness</option>
                <option value="Cost savings">Cost savings</option>
                <option value="Convenience">Convenience</option>
                <option value="Speed">Speed</option>
                <option value="Safety">Safety</option>
                <option value="No alternative">No alternative available</option>
              </select>
            </div>

            <div class="form-group">
              <label class="form-label"
                >What are the main barriers to cycling more in London? (Select
                all that apply)</label
              >
              <div class="checkbox-group">
                <div class="checkbox-item">
                  <input
                    type="checkbox"
                    id="b1"
                    value="Safety concerns with traffic"
                    v-model="form.barriers"
                  />
                  <label for="b1">Safety concerns with traffic</label>
                </div>
                <div class="checkbox-item">
                  <input
                    type="checkbox"
                    id="b2"
                    value="Weather conditions"
                    v-model="form.barriers"
                  />
                  <label for="b2">Weather conditions</label>
                </div>
                <div class="checkbox-item">
                  <input
                    type="checkbox"
                    id="b3"
                    value="Poor cycling infrastructure"
                    v-model="form.barriers"
                  />
                  <label for="b3">Poor cycling infrastructure</label>
                </div>
                <div class="checkbox-item">
                  <input
                    type="checkbox"
                    id="b4"
                    value="Distance of journeys"
                    v-model="form.barriers"
                  />
                  <label for="b4">Distance of journeys</label>
                </div>
                <div class="checkbox-item">
                  <input
                    type="checkbox"
                    id="b5"
                    value="Bike storage issues"
                    v-model="form.barriers"
                  />
                  <label for="b5">Bike storage issues</label>
                </div>
                <div class="checkbox-item">
                  <input
                    type="checkbox"
                    id="b6"
                    value="Air quality concerns"
                    v-model="form.barriers"
                  />
                  <label for="b6">Air quality concerns</label>
                </div>
              </div>
            </div>

            <div class="form-group">
              <label class="form-label"
                >Which London borough do you live/work in most?</label
              >
              <select class="form-select" v-model="form.q5" required>
                <option value="">Select your borough</option>
                <option value="City of London">City of London</option>
                <option value="Westminster">Westminster</option>
                <option value="Camden">Camden</option>
                <option value="Islington">Islington</option>
                <option value="Hackney">Hackney</option>
                <option value="Tower Hamlets">Tower Hamlets</option>
                <option value="Greenwich">Greenwich</option>
                <option value="Southwark">Southwark</option>
                <option value="Lambeth">Lambeth</option>
                <option value="Wandsworth">Wandsworth</option>
                <option value="Hammersmith and Fulham">
                  Hammersmith and Fulham
                </option>
                <option value="Kensington and Chelsea">
                  Kensington and Chelsea
                </option>
                <option value="Other">Other borough</option>
              </select>
            </div>

            <div class="form-group">
              <label class="form-label"
                >Additional comments or suggestions for improving London's
                transport and air quality:</label
              >
              <textarea
                class="form-textarea"
                v-model="form.comments"
                placeholder="Share your thoughts on how London can reduce vehicle emissions and improve cycling infrastructure..."
              ></textarea>
            </div>

            <button type="submit" class="submit-btn">Submit Feedback</button>

            <div class="form-footer">
              Your responses help inform policy recommendations for cleaner
              transport
            </div>
          </form>

          <div class="help-text">
            <strong>💡 Instructions:</strong><br />
            • After submitting, your feedback will be displayed in "COMMUNITY
            RESPONSES" section below<br />
            • <strong>User mode</strong>: View only, cannot delete<br />
            • <strong>Admin mode</strong>: Can delete inappropriate comments<br />
            • Click <strong>ADMIN MODE</strong> button to switch to admin<br />
            • Admin password: <strong>ecovision2024</strong>
          </div>

          <div class="comments-section" :class="{ 'admin-mode': isAdmin }">
            <div class="comments-header">
              <h2 class="comments-title">
                COMMUNITY RESPONSES ({{ comments.length }})
              </h2>
              <div class="mode-indicator">
                <span class="role-badge" :style="badgeStyle">{{
                  isAdmin ? "ADMIN MODE" : "USER MODE"
                }}</span>
                <button
                  class="admin-btn"
                  :class="{ active: isAdmin }"
                  @click="toggleAdmin"
                >
                  {{ isAdmin ? "EXIT ADMIN" : "ADMIN MODE" }}
                </button>
              </div>
            </div>

            <div v-if="comments.length === 0" class="no-comments">
              No responses yet. Be the first to share your feedback!
            </div>

            <div
              v-for="(comment, index) in comments"
              :key="comment.id"
              class="comment"
            >
              <div class="comment-header">
                <div>
                  <div class="comment-author">
                    Response #{{ comments.length - index }}
                  </div>
                  <div class="comment-date">{{ formatDate(comment.date) }}</div>
                </div>
                <div class="comment-meta">
                  <button class="delete-btn" @click="deleteComment(index)">
                    DELETE
                  </button>
                </div>
              </div>
              <div class="comment-text">
                <strong>Preference:</strong> {{ comment.q1 }}<br />
                <strong>Frequency:</strong> {{ comment.q2 }}<br />
                <strong>Main reason:</strong> {{ comment.q3 }}<br />
                <strong>Barriers:</strong> {{ comment.barriers }}<br />
                <strong>Borough:</strong> {{ comment.q5 }}
                <template v-if="comment.comments"
                  ><br /><strong>Comments:</strong>
                  {{ comment.comments }}</template
                >
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, computed, onMounted, onActivated } from "vue";

const resetScroll = () => {
  window.scrollTo(0, 0);
  document.body.scrollTop = 0;
  document.documentElement.scrollTop = 0;
  const app = document.getElementById("app");
  if (app) app.scrollTop = 0;
};

onMounted(() => {
  resetScroll();
});
onActivated(() => {
  resetScroll();
});

const form = reactive({
  q1: "",
  q2: "",
  q3: "",
  barriers: [],
  q5: "",
  comments: "",
});

const comments = ref([]);
const isAdmin = ref(false);
const showModal = ref(false);
const password = ref("");
const showSuccess = ref(false);
const successMsg = ref("");

const badgeStyle = computed(() => ({
  background: isAdmin.value ? "#10b981" : "#888",
  color: "#fff",
  padding: "0.5rem 1rem",
  borderRadius: "4px",
  fontSize: "0.7rem",
  fontFamily: "'Press Start 2P', cursive",
}));

const formatDate = (date) => {
  const d = new Date(date);
  return d.toLocaleString("en-GB", {
    day: "2-digit",
    month: "short",
    year: "numeric",
    hour: "2-digit",
    minute: "2-digit",
  });
};

const submitForm = () => {
  const newComment = {
    id: Date.now(),
    date: new Date().toISOString(),
    q1: form.q1,
    q2: form.q2,
    q3: form.q3,
    barriers: form.barriers.join(", ") || "None selected",
    q5: form.q5,
    comments: form.comments,
  };

  comments.value.unshift(newComment);

  // Reset form
  Object.assign(form, {
    q1: "",
    q2: "",
    q3: "",
    barriers: [],
    q5: "",
    comments: "",
  });

  // Show success message
  successMsg.value = "FEEDBACK SUBMITTED!";
  showSuccess.value = true;
  setTimeout(() => {
    showSuccess.value = false;
  }, 3000);

  // Scroll to comments
  setTimeout(() => {
    document.querySelector(".comments-section").scrollIntoView({
      behavior: "smooth",
      block: "start",
    });
  }, 500);
};

const toggleAdmin = () => {
  if (isAdmin.value) {
    isAdmin.value = false;
  } else {
    showModal.value = true;
  }
};

const login = () => {
  if (password.value === "ecovision2024") {
    isAdmin.value = true;
    showModal.value = false;
    password.value = "";
    successMsg.value = "ADMIN MODE ACTIVATED";
    showSuccess.value = true;
    setTimeout(() => {
      showSuccess.value = false;
    }, 2000);
  } else {
    alert("Incorrect password");
  }
};

const deleteComment = (index) => {
  if (isAdmin.value) {
    if (confirm("Delete this response?")) {
      comments.value.splice(index, 1);
      successMsg.value = "RESPONSE DELETED";
      showSuccess.value = true;
      setTimeout(() => {
        showSuccess.value = false;
      }, 2000);
    }
  }
};
</script>

<style scoped>
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

.team-page {
  font-family: "Space Grotesk", sans-serif;
  background: #0a0a0f;
  color: #fff;
  overflow-x: hidden;
}

/* Navigation */
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

/* Content */
.content {
  padding-top: 80px;
}

/* Hero */
.hero {
  min-height: 50vh;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  padding: 4rem 2rem;
  background: linear-gradient(
    180deg,
    rgba(138, 43, 226, 0.2) 0%,
    transparent 100%
  );
}

.title {
  font-family: "Press Start 2P", cursive;
  font-size: clamp(1.5rem, 4vw, 2.5rem);
  color: #a855f7;
  margin-bottom: 1rem;
  text-shadow: 0 0 20px rgba(168, 85, 247, 0.8);
  animation: pulse 2s ease-in-out infinite;
}

@keyframes pulse {
  0%,
  100% {
    text-shadow: 0 0 20px rgba(168, 85, 247, 0.8);
  }
  50% {
    text-shadow: 0 0 40px rgba(168, 85, 247, 1);
  }
}

.subtitle {
  font-size: 1.3rem;
  color: #ccc;
}

.section {
  min-height: 50vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 4rem 2rem;
}

.box {
  max-width: 1000px;
  width: 100%;
  background: rgba(0, 0, 0, 0.95);
  border: 3px solid #a855f7;
  padding: 3rem;
  box-shadow: 0 0 30px rgba(168, 85, 247, 0.5);
}

.box h2 {
  font-family: "Press Start 2P", cursive;
  color: #a855f7;
  font-size: 1.5rem;
  margin-bottom: 2rem;
  line-height: 1.8;
}

.box p {
  font-size: 1.1rem;
  line-height: 1.8;
  color: #e0e0e0;
  margin-bottom: 2rem;
}

/* Team Members Grid - New Section */
.team-intro-section {
  background: linear-gradient(
    180deg,
    transparent 0%,
    rgba(138, 43, 226, 0.1) 100%
  );
}

.team-members-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(280px, 1fr));
  gap: 2rem;
  margin-top: 3rem;
}

.member-card {
  background: rgba(0, 0, 0, 0.7);
  border: 2px solid #a855f7;
  padding: 2rem;
  text-align: center;
  transition: all 0.3s ease;
  position: relative;
  overflow: hidden;
}

.member-card::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: linear-gradient(135deg, rgba(168, 85, 247, 0.1), transparent);
  opacity: 0;
  transition: opacity 0.3s ease;
}

.member-card:hover {
  transform: translateY(-10px);
  box-shadow: 0 20px 40px rgba(168, 85, 247, 0.4);
  border-color: #ec4899;
}

.member-card:hover::before {
  opacity: 1;
}

.member-avatar {
  width: 120px;
  height: 120px;
  margin: 0 auto 1.5rem;
  background: linear-gradient(135deg, #a855f7, #ec4899);
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  border: 3px solid rgba(255, 255, 255, 0.2);
  box-shadow: 0 10px 30px rgba(168, 85, 247, 0.3);
}

.avatar-emoji {
  font-size: 3.5rem;
}

.member-name {
  font-family: "Press Start 2P", cursive;
  color: #a855f7;
  font-size: 0.9rem;
  margin-bottom: 0.5rem;
  line-height: 1.6;
}

.member-title {
  color: #ec4899;
  font-weight: 600;
  font-size: 0.85rem;
  margin-bottom: 1rem;
  line-height: 1.4;
}

.member-description {
  color: #ccc;
  font-size: 0.95rem;
  line-height: 1.6;
  margin: 0;
}

/* Team Section */
.team-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 2rem;
  margin-top: 2rem;
}

.team-member {
  padding: 2rem;
  background: rgba(168, 85, 247, 0.05);
  border: 2px solid #a855f7;
  text-align: center;
  transition: all 0.3s ease;
}

.team-member:hover {
  transform: translateY(-5px);
  box-shadow: 0 10px 30px rgba(168, 85, 247, 0.3);
  background: rgba(168, 85, 247, 0.1);
}

.member-role {
  font-size: 3rem;
  margin-bottom: 1rem;
}

.team-member h3 {
  color: #a855f7;
  margin-bottom: 0.5rem;
  font-size: 1.1rem;
}

.team-member p {
  color: #ccc;
  margin: 0;
  font-size: 0.95rem;
}

/* Impact Section */
.impact-stats {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 2rem;
  margin-top: 2rem;
}

.impact-card {
  text-align: center;
  padding: 2rem;
  background: rgba(168, 85, 247, 0.05);
  border: 2px solid #a855f7;
  transition: all 0.3s ease;
}

.impact-card:hover {
  transform: translateY(-5px);
  box-shadow: 0 10px 30px rgba(168, 85, 247, 0.3);
  background: rgba(168, 85, 247, 0.1);
}

.impact-icon {
  font-size: 3rem;
  margin-bottom: 1rem;
}

.impact-value {
  font-family: "Press Start 2P", cursive;
  font-size: 2rem;
  color: #a855f7;
  margin-bottom: 0.5rem;
}

.impact-label {
  color: #ccc;
  font-size: 0.9rem;
}

/* Survey Section */
.survey-section {
  background: linear-gradient(
    180deg,
    rgba(138, 43, 226, 0.1) 0%,
    transparent 100%
  );
  padding: 6rem 2rem;
}

.survey-container {
  max-width: 900px;
  width: 100%;
}

.survey-header {
  text-align: center;
  margin-bottom: 3rem;
}

.survey-title {
  font-family: "Press Start 2P", cursive;
  font-size: clamp(1rem, 3vw, 1.8rem);
  color: #a855f7;
  margin-bottom: 1.5rem;
  line-height: 1.8;
}

.survey-subtitle {
  font-size: 1.1rem;
  color: #ccc;
  line-height: 1.8;
}

.survey-form {
  background: rgba(0, 0, 0, 0.8);
  border: 3px solid #a855f7;
  padding: 3rem;
  box-shadow: 0 0 30px rgba(168, 85, 247, 0.3);
}

.form-group {
  margin-bottom: 2rem;
}

.form-label {
  display: block;
  color: #a855f7;
  font-weight: 600;
  margin-bottom: 0.5rem;
  font-size: 1rem;
}

.form-select,
.form-textarea {
  width: 100%;
  padding: 0.75rem;
  background: rgba(255, 255, 255, 0.05);
  border: 2px solid #666;
  color: #fff;
  font-family: "Space Grotesk", sans-serif;
  font-size: 0.95rem;
  border-radius: 4px;
  transition: border-color 0.3s;
}

.form-select:focus,
.form-textarea:focus {
  outline: none;
  border-color: #a855f7;
  background: rgba(255, 255, 255, 0.08);
}

.form-textarea {
  min-height: 120px;
  resize: vertical;
}

.checkbox-group {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 1rem;
  margin-top: 0.5rem;
}

.checkbox-item {
  display: flex;
  align-items: center;
  gap: 0.5rem;
}

.checkbox-item input[type="checkbox"] {
  width: 20px;
  height: 20px;
  cursor: pointer;
}

.checkbox-item label {
  color: #ccc;
  cursor: pointer;
  font-size: 0.95rem;
}

.submit-btn {
  width: auto;
  padding: 1rem 3rem;
  background: #a855f7;
  color: #fff;
  border: 3px solid #fff;
  font-family: "Press Start 2P", cursive;
  font-size: 0.8rem;
  font-weight: 600;
  cursor: pointer;
  display: block;
  margin: 2rem auto 0;
  transition: all 0.2s;
  box-shadow: 5px 5px 0 #000;
}

.submit-btn:hover {
  background: #9333ea;
  transform: translate(2px, 2px);
  box-shadow: 3px 3px 0 #000;
}

.submit-btn:active {
  transform: translate(5px, 5px);
  box-shadow: none;
}

.form-footer {
  text-align: center;
  color: #888;
  font-size: 0.9rem;
  margin-top: 1.5rem;
}

/* Comments Section */
.comments-section {
  background: rgba(168, 85, 247, 0.05);
  border: 2px solid #a855f7;
  padding: 2rem;
  border-radius: 8px;
  margin-top: 3rem;
}

.comments-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 2rem;
  flex-wrap: wrap;
  gap: 1rem;
}

.comments-title {
  font-family: "Press Start 2P", cursive;
  color: #a855f7;
  font-size: 0.9rem;
  line-height: 1.6;
}

.mode-indicator {
  display: flex;
  align-items: center;
  gap: 1rem;
}

.role-badge {
  font-family: "Press Start 2P", cursive;
  font-size: 0.6rem;
  color: #a855f7;
}

.admin-btn {
  background: #ec4899;
  color: #fff;
  padding: 0.5rem 1rem;
  border: 2px solid #fff;
  font-family: "Press Start 2P", cursive;
  font-size: 0.6rem;
  cursor: pointer;
  box-shadow: 3px 3px 0 #000;
  transition: all 0.1s;
}

.admin-btn:hover {
  transform: translate(1px, 1px);
  box-shadow: 2px 2px 0 #000;
}

.admin-btn.active {
  background: #10b981;
}

.comment {
  background: rgba(0, 0, 0, 0.5);
  border: 2px solid #a855f7;
  padding: 1.5rem;
  margin-bottom: 1.5rem;
  border-radius: 4px;
  position: relative;
}

.comment-header {
  display: flex;
  justify-content: space-between;
  align-items: start;
  margin-bottom: 1rem;
  flex-wrap: wrap;
  gap: 0.5rem;
}

.comment-author {
  color: #a855f7;
  font-weight: 600;
  font-size: 1.1rem;
}

.comment-meta {
  display: flex;
  gap: 1rem;
  align-items: center;
}

.comment-date {
  color: #888;
  font-size: 0.9rem;
}

.delete-btn {
  background: #ef4444;
  color: #fff;
  padding: 0.5rem 1rem;
  border: 2px solid #fff;
  font-family: "Press Start 2P", cursive;
  font-size: 0.5rem;
  cursor: pointer;
  box-shadow: 2px 2px 0 #000;
  transition: all 0.1s;
  display: none;
}

.delete-btn:hover {
  transform: translate(1px, 1px);
  box-shadow: 1px 1px 0 #000;
}

.comments-section.admin-mode .delete-btn {
  display: block;
}

.comment-text {
  color: #e0e0e0;
  line-height: 1.8;
  font-size: 0.95rem;
}

.no-comments {
  text-align: center;
  color: #888;
  padding: 3rem;
  font-style: italic;
}

/* Modal */
.modal {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: rgba(0, 0, 0, 0.9);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 200;
  opacity: 0;
  pointer-events: none;
  transition: opacity 0.3s;
}

.modal.active {
  opacity: 1;
  pointer-events: all;
}

.modal-content {
  background: #fff;
  border: 3px solid #a855f7;
  max-width: 500px;
  width: 90%;
  padding: 2rem;
  box-shadow: 0 0 50px rgba(168, 85, 247, 0.7);
}

.modal-title {
  background: #a855f7;
  color: #fff;
  padding: 1rem;
  font-family: "Press Start 2P", cursive;
  font-size: 0.8rem;
  text-align: center;
  margin: -2rem -2rem 1rem;
}

.modal-input {
  width: 100%;
  padding: 1rem;
  border: 2px solid #ddd;
  font-family: "Space Grotesk", sans-serif;
  font-size: 1rem;
  border-radius: 4px;
  margin-bottom: 1rem;
}

.modal-input:focus {
  outline: none;
  border-color: #a855f7;
}

.modal-buttons {
  display: flex;
  gap: 1rem;
}

.modal-btn {
  flex: 1;
  padding: 1rem;
  border: none;
  font-family: "Press Start 2P", cursive;
  font-size: 0.7rem;
  cursor: pointer;
  transition: all 0.2s;
}

.modal-btn.confirm {
  background: #a855f7;
  color: #fff;
}

.modal-btn.confirm:hover {
  background: #9333ea;
}

.modal-btn.cancel {
  background: #888;
  color: #fff;
}

.modal-btn.cancel:hover {
  background: #666;
}

/* Success Message */
.success-message {
  position: fixed;
  top: 100px;
  right: 2rem;
  background: #10b981;
  color: #fff;
  padding: 1rem 2rem;
  border: 2px solid #fff;
  font-family: "Press Start 2P", cursive;
  font-size: 0.7rem;
  box-shadow: 5px 5px 0 #000;
  z-index: 300;
  opacity: 0;
  transform: translateX(400px);
  transition: all 0.3s;
}

.success-message.show {
  opacity: 1;
  transform: translateX(0);
}

/* Help Text */
.help-text {
  background: rgba(168, 85, 247, 0.1);
  border-left: 3px solid #a855f7;
  padding: 1rem;
  margin-top: 2rem;
  font-size: 0.9rem;
  color: #a855f7;
  line-height: 1.6;
}

/* Responsive */
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

  .title {
    font-size: 1.5rem;
  }

  .box {
    padding: 2rem;
  }

  .survey-form {
    padding: 2rem 1.5rem;
  }

  .impact-stats {
    grid-template-columns: 1fr;
  }

  .team-grid {
    grid-template-columns: 1fr;
  }

  .team-members-grid {
    grid-template-columns: 1fr;
  }

  .checkbox-group {
    grid-template-columns: 1fr;
  }
}
</style>
