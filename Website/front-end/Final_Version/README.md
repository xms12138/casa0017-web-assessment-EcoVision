1. Enhanced TEAM Page (Page4.vue)

The TEAM page has been updated with a detailed introduction for all five team members:

Madina Diallo – Project & Data Integration Lead
Leads project coordination and manages integration of air quality and transport datasets.

Matilda Nelson – Research & Web Content Lead
Conducts research on London’s air quality challenges and creates user-focused written content.

Vidan Gao – Design & Visualisation Lead
Designs the website’s visual style and develops intuitive, interactive visualisations.

Zihang He – Technical & Web Development Lead
Builds and maintains the technical infrastructure and interactive map for real-time data visualisation.

Qingshan Luo – Testing & Editing Lead
Responsible for quality assurance, user testing, and proofreading to ensure clarity and accuracy.

Design features:

Individual member cards with emoji avatar, name, role, and description

Hover effect: subtle lift and shadow for better interactivity

Purple–pink gradient accent to match the overall site theme

Responsive grid layout for different screen sizes

2. Updated “Submit Feedback” Button Style

The Submit Feedback button has been restyled to align with the global visual theme:

Default background: #a855f7 (purple)

Hover background: #9333ea (darker purple)

Slight 3D-style shadow for a more clickable look

Uses a pixel-style font where available for a playful UI effect

3. Other Pages & Features

HOME, ABOUT (Page2.vue), MAP (Map.vue) and core logic remain unchanged.

All existing functionality continues to work as before.

Only Page4.vue (TEAM page) and the feedback button styling were modified.

Project Structure (Current)
FINAL_VERSION/
├── dist/ # Production build output
├── node_modules/
├── public/
│ ├── borough_air_quality.json
│ ├── busiest_streets.geojson
│ ├── london_boroughs.geojson
│ └── test.json
├── src/
│ ├── assets/
│ │ ├── css/
│ │ └── styles/
│ ├── components/
│ │ ├── BoroughPopup.vue
│ │ ├── MapLegend.vue
│ │ ├── RoutePopup.vue
│ │ └── TimeSlider.vue
│ ├── router/
│ │ └── index.js
│ ├── views/
│ │ ├── Home.vue
│ │ ├── Map.vue
│ │ ├── Page2.vue
│ │ ├── Page4.vue # TEAM page (modified)
│ │ └── Test.vue
│ ├── App.vue
│ └── main.js
├── .env
├── .gitignore
├── CHANGES.md
├── index.html
├── package-lock.json
├── package.json
├── PROJECT_OVERVIEW.md
├── QUICKSTART.md
├── README.md
└── vite.config.js

Installation & Usage

1. Install Dependencies
   cd FINAL_VERSION
   npm install

2. Run Development Server
   npm run dev

The app will be available at: http://localhost:3000

3. Build for Production
   npm run build

4. Preview Production Build
   npm run preview

Tech Stack

Vue 3 – UI framework

Vue Router – Client-side routing

Vite – Fast dev server & bundler

Mapbox GL JS – Interactive London borough map & visualisation

CSS3 – Layout, animations, and custom styling

Browser Support

Chrome (recommended)

Firefox

Safari

Edge

Notes

Requires Node.js 16+.

Run npm install before first use.

Public JSON/GeoJSON files provide London borough boundaries, busiest streets, and sample air quality data for the map.

TEAM page includes an Admin mode password: ecovision2024 (for in-page admin features if enabled in code).

Modification Details
Updated Files

src/views/Page4.vue

Added team introduction section and member cards.

Applied hover and gradient styles for the TEAM section.

Updated .submit-btn styles to purple theme.

Other files and routes remain unchanged.
