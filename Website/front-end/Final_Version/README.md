## 💻 How to Run the Frontend Server

### By Zihang He



The EcoVision frontend is built with **Vue 3 + Vite**, and can be launched either in development or production mode.

Follow the steps below to set up and run the frontend server.



```bash

1️⃣ Clone the Repository

Clone the entire EcoVision repository and navigate to the final frontend version:
git clone https://github.com/xms12138/casa0017-web-assessment-EcoVision.git
cd casa0017-web-assessment-EcoVision/Website/front-end/Final_Version


2️⃣ Install Dependencies
Install all required Node.js packages:
npm install


3️⃣ Configure Environment Variables
Before running, create a file named .env.local inside Final_Version/:

VITE_MAPBOX_TOKEN=your_mapbox_access_token_here
VITE_API_BASE_URL=http://localhost:3000

Notes:

VITE_MAPBOX_TOKEN → Your personal Mapbox access token.

VITE_API_BASE_URL → The backend server address.

Use http://localhost:3000 if running locally.

Use http://10.129.111.18:3000 if connected to the UCL lab network.

4️⃣ Run the Development Server
Start the frontend with:
npm run dev

6️⃣ Frontend Structure Overview

Final_Version/
├─ public/             # Static files (icons, assets)
├─ src/
│  ├─ assets/          # Images and visual resources
│  ├─ components/      # Reusable UI components (sliders, panels, navbar)
│  ├─ views/           # Main pages
│  │  ├─ Home.vue      # Landing page with 3D globe
│  │  ├─ Map.vue       # London air quality visualization (Mapbox)
│  │  ├─ Page2.vue     # Project concept & background
│  │  ├─ Page4.vue     # Team and contribution info
│  │  └─ Test.vue      # Developer tool for Map Matching API
│  ├─ router/          # Page routing logic
│  ├─ App.vue          # Root application layout
│  └─ main.js          # Entry point for Vue
├─ index.html          # Root HTML template for Vite
├─ vite.config.js      # Vite configuration
└─ package.json        # Scripts and dependency list

7️⃣ Page Descriptions
File	Description
Home.vue	Landing page with a 3D rotating globe and global air quality visualization.
Map.vue	Main interactive map of London using Mapbox GL JS. Displays 33 boroughs, air quality levels, and allows time-based exploration.
Page2.vue	Project description and environmental motivation.
Page4.vue	Team presentation and acknowledgements.
Test.vue	Internal developer tool for drawing and snapping lines to real roads using Mapbox Map Matching API.

8️⃣ Example Access
When connected to the UCL lab network:
👉 http://10.129.111.18:3000/casa0017-web-assessment-EcoVision/

When running locally:
👉 http://localhost：3000/
```
