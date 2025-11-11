# EcoVision Synergy: Mapping London’s Air Quality and Vehicle Emissions  

<p align="center">
  <img src="./assets/logos/ecovision-logo.png" alt="EcoVision Team Logo" width="220"/>
</p>

---

## Abstract  

**EcoVision Synergy** is an interactive web-based visualisation platform that explores the relationship between vehicle-related CO₂/NO₂ emissions and traffic patterns across London’s 33 boroughs.  

The project transforms complex environmental data into an accessible, engaging, and actionable experience for the public. Through intuitive design, users can visualise hourly air-quality fluctuations and understand how urban mobility influences pollution levels.  

By merging open data, spatial mapping, and interactive storytelling, EcoVision empowers Londoners to make informed travel choices, advocate for sustainable policies, and appreciate how technology can translate data into environmental insight.  

---

## Introduction  

### Background and Context  
Air pollution remains one of London’s most pressing environmental and public health challenges. According to the **London Atmospheric Emissions Inventory (GLA, 2023)**, road transport contributes nearly half of the city’s nitrogen oxide emissions. Despite vast open datasets from **DEFRA**, **TfL**, and the **London Datastore**, these are often fragmented or inaccessible to the general public.  

**EcoVision Synergy** was created to bridge this gap — transforming complex emissions data into an interactive story that visualises how CO₂, NO₂, and PM₂.₅ fluctuate across boroughs throughout a 24-hour cycle.  

### Project Aim  
EcoVision’s mission is to create a dynamic, data-driven platform that helps users:  

- Visualise hourly CO₂ and NO₂ emission levels across boroughs  
- Understand how traffic density correlates with air quality  
- Explore how time of day impacts pollution levels  
- Discover practical solutions for individuals, communities, and policymakers  

---

## Key Features  

🗺️ **Interactive London Borough Map:** Clickable polygons representing each borough with embedded air-quality and traffic data.  
⏱️ **Time Slider Functionality:** Explore hourly pollution trends across a 24-hour period.  
🌈 **Colour-Coded Emissions:** Red, yellow, and green gradients show pollution intensity.  
💡 **Embedded Solutions:** Offers tailored recommendations for cleaner travel, behavioural change, and community action.  
🌍 **3D Globe Mode:** Uses **Globe.GL** to visualise global emissions contextually.  

---

## Methodology  

Data integration was achieved using open-source environmental and mobility datasets from:  

- **London Air Quality Network (LAQN)**  
- **Transport for London (TfL)** – traffic and congestion statistics  
- **National Atmospheric Emissions Inventory (NAEI)**  
- **DEFRA** and **Google Maps**  

Data was cleaned and transformed using **Python**, with visualisation layers developed in **JavaScript** and **Vue 3**.  

**Core Technologies:**  
- Leaflet.js – Interactive mapping  
- D3.js – Data visualisation and scaling  
- Globe.GL – 3D visualisation  
- MySQL – Backend data management  
- Node.js + Express – API connections  
- HTML5, CSS3, JavaScript (ES6) – Frontend design  
- GitHub Pages – Version control and hosting  

Our design prioritised **clarity, accessibility, and engagement**, ensuring the site communicates both data accuracy and storytelling impact.  

---

## Design Process and Roles  

### Team Structure  

#### **Madina Diallo – Project Manager & Data Integration Lead**  
Madina led the project from concept to delivery, ensuring alignment with scope, schedule, and quality. She coordinated sprints, GitHub version control, and documentation, verifying data accuracy and cohesion between backend and frontend teams. Her leadership ensured smooth collaboration and a coherent final product.  

#### **Matilda Nelson – Data & Content Lead**  
Matilda sourced and processed environmental datasets from DEFRA, TfL, and the London Datastore. She designed the “Solutions” section, transforming static references into actionable, data-driven insights linking emissions data to user-friendly recommendations.  

#### **Yidan Gao – UX & Visualisation Lead**  
Yidan designed the user journey and interface, creating prototypes with **Miro** and **Canva** before implementing them in **Vue 3**. She developed a clear navigation flow from homepage to map exploration and ensured accessibility (WCAG 2.1 compliance) across devices.  

#### **Zihang He – Frontend & Interactivity Lead**  
Zihang implemented the interactive map and 3D globe using **Vue 3**, **Vite**, and **Mapbox GL JS**. He engineered modular components (TimeSlider, MapLegend, BoroughPopup) and optimised performance for smooth visual transitions and real-time responsiveness.  

#### **Qingshan Luo – Backend & Data Engineering Lead**  
Qingshan developed two RESTful APIs powering real-time borough-level visualisations. He structured emissions and traffic datasets in **MySQL**, created efficient **Node.js/Express** routes, and connected Python-processed emissions data with live frontend requests.  

---

## Workflow  

The project followed a **four-week Agile sprint** structure:  

| Week | Key Activities | Deliverables |
|------|----------------|--------------|
| **1** | Research, data discovery, and storyboarding | Storyboard and initial data sources |
| **2** | Backend setup and UI wireframes | Cleaned datasets and prototype |
| **3** | Development of interactivity and API testing | Interactive map and time slider |
| **4** | Final testing, accessibility review, and deployment | Live website and report |

Collaboration tools included **Microsoft Teams**, **GitHub**, and **Google Drive**, ensuring version control, sprint transparency, and rapid iteration.  

---

## Results  

The final platform delivers:  
- Real-time API-driven data visualisation  
- Hourly CO₂, NO₂, and PM₂.₅ mapping across 33 boroughs  
- Dynamic 3D global context view  
- Actionable insights for sustainable urban mobility  

**User testing** confirmed the map’s intuitiveness and praised the clarity of its time slider and visual cues. Participants appreciated how EcoVision translated complex data into meaningful environmental insight.  

---

## Reflection  

The EcoVision project showcased the power of **collaborative, interdisciplinary work** — blending environmental research, data engineering, and UX design.  

### Key Learnings  
- Aligning multi-source datasets required creative problem-solving  
- API optimisation was essential for smooth visual interaction  
- Visual storytelling proved as critical as technical accuracy  

Ultimately, **EcoVision** demonstrated how data-driven design can inspire behavioural change and civic awareness.  

---

## Conclusion  

EcoVision Synergy exemplifies how open data and web technology can drive environmental understanding and advocacy.  

### Future Extensions  
- Integrate **real-time IoT sensor data** for live monitoring  
- Develop **borough comparison dashboards**  
- Implement **community-driven data contributions**  
- Explore **predictive analytics** for emissions forecasting  

EcoVision represents not only a website but a **prototype for citizen engagement**, turning invisible pollution into visible knowledge and inspiring collective action for a cleaner London.  

---

## Bibliography  

- Batty, M. (2018). *Inventing Future Cities*. MIT Press.  
- Cox, M., Longley, P., & Shao, Z. (2022). *Interactive Geovisualisation for Environmental Awareness*. *Environment and Planning B.*  
- DEFRA (2023). *Air Quality Data API.* https://uk-air.defra.gov.uk/data  
- Greater London Authority (GLA) (2023). *London Atmospheric Emissions Inventory.* https://data.london.gov.uk  
- London Air Quality Network (LAQN) (2024). *NO₂ and PM₂.₅ Data Portal.* https://www.londonair.org.uk  
- Transport for London (TfL) (2024). *Traffic and Congestion Statistics.* https://tfl.gov.uk/info-for/open-data-users/  

---

## Declaration of Authorship  

We, **Madina Diallo, Matilda Nelson, Yidan Gao, Zihang He, and Qingshan Luo**, confirm that the work presented in this project is our own.  
Where information has been derived from other sources, it has been clearly indicated.  

**Digitally Signed:**  
**EcoVision Team**  
*November 2025*  

---

### 🔗 Project Links  
- **Live Website:** [EcoVision Interactive Map](https://xms12138.github.io/casa0017-web-assessment-EcoVision/)  
- **GitHub Repository:** [EcoVision on GitHub](https://github.com/xms12138/casa0017-web-assessment-EcoVision)  
