---
title: "CASA0017: Web Architecture Final Assessment"
author: "EcoVision Team – Madina Diallo, Matilda Nelson, Qingshan Luo, Yidan Gao, Zihang He"
date: "November 2025"
---

# 🌿 EcoVision Synergy: Mapping London’s Air Quality and Vehicle Emissions  

## Abstract  

**EcoVision Synergy** is an interactive web-based visualisation platform that explores the relationship between **vehicle-related CO₂/NO₂ emissions** and **traffic patterns** across London’s 33 boroughs.  

The project transforms complex environmental data into an **accessible, engaging, and actionable experience** for the public. Through intuitive design, users can visualise hourly air-quality fluctuations and understand how urban mobility influences pollution levels.  

By merging **open data, spatial mapping, and interactive storytelling**, EcoVision empowers Londoners to make informed travel choices, advocate for sustainable policies, and appreciate how technology can translate data into environmental insight.  

---

## Introduction  

### Background and Context  
Air pollution remains one of London’s most serious environmental and public health challenges. With rising urban density, heavy vehicle usage, and unequal green-space distribution, CO₂ and NO₂ levels continue to threaten both the city’s ecology and human wellbeing.  

Despite the abundance of open environmental data, much of it exists in **technical dashboards or static reports** inaccessible to most citizens. Our project reimagines this data through a **visual narrative**, allowing anyone to explore and interpret London’s air quality interactively.  

### Project Aim  
EcoVision Synergy’s mission is to create a **dynamic, data-driven platform** that helps users:  
- Visualise hourly CO₂ and NO₂ emission levels across boroughs  
- Understand how traffic density correlates with air quality  
- Explore how time of day impacts emissions  
- Discover practical solutions for individuals, communities, and policymakers  

### Key Features  
- 🗺️ **Interactive London Borough Map:** Clickable polygons representing each borough with embedded air-quality data.  
- ⏱️ **Time Slider Functionality:** Allows exploration of hourly pollution trends over a 24-hour cycle.  
- 🌈 **Colour-Coded Emissions:** Red, yellow, and green gradients show pollution intensity.  
- 💡 **Embedded Solutions:** Offers tailored recommendations encouraging cleaner travel, behavioural change, and community action.  
- 🌍 **3D Globe Mode:** A supplementary view that visualises data globally, built using Globe.GL.  

### Methodology  
We integrated **open-source environmental and mobility datasets** from:  
- London Air Quality Network (LAQN)  
- Transport for London (TfL) traffic and congestion statistics  
- National Atmospheric Emissions Inventory (NAEI)  
- Supplementary Google Maps and DEFRA data  

Data cleaning and transformation were performed using **Python**, with visualisation layers built in **JavaScript**.  

**Core technologies used:**  
- **Leaflet.js** – interactive mapping  
- **D3.js** – data visualisation and scaling  
- **Globe.GL** – 3D representation  
- **MySQL** – backend data storage  
- **HTML5, CSS3, JavaScript (ES6)** – front-end design and functionality  
- **Node.js** – server logic and API connections  
- **GitHub Pages** – version control and hosting  

Our design philosophy focused on **clarity, accessibility, and engagement**, ensuring the site communicates both data accuracy and visual storytelling.  

---

## Design Process and Roles  

### Team Structure  

**Meet the Team**  

- **Madina Diallo – Project Manager & Data Integration Lead**  
Madina brings the vision to delivery. She ensures the project stays aligned with scope, timeline, and quality, facilitating weekly sprints, coordinating GitHub workflows, and keeping the team synced. Her focus is on ensuring every dataset, visualisation, and line of code serves the broader mission. She also integrated datasets, maintained narrative cohesion, and oversaw presentation and repository documentation.  

- **Matilda Nelson – Data & Content Lead**  
Matilda is our data detective and storyteller. She sourced emissions, traffic, and mode-use datasets from the London Datastore, DEFRA, and TfL, documented how each dataset feeds the platform, and guided early sketching workshops to align team vision. She ensures the **“Solutions”** page is meaningful — not just a list of resources but a data-driven call to action.  

- **Yidan Gao – UX & Visualisation Lead**  
Yidan designed the user journey — from homepage flow to map interaction and solution discovery. Using **Miro** and wireframes, she mapped how users explore the interface, visualising how data transitions through the slider and pop-ups. Her accessibility-first mindset ensures inclusivity across devices and browsers.  

- **Zihang He – Frontend & Interactivity Lead**  
Zihang built the bridge between data and experience. Using **HTML5, CSS3, JavaScript (ES6), Leaflet.js, and Globe.GL**, he developed the interactive map, the 3D globe, and smooth slider transitions. His code connects to live APIs, ensuring the experience remains responsive, dynamic, and visually rich.  

- **Qingshan Luo – Backend & Data Engineering Lead**  
Qingshan built the engine behind the platform. He gathered emissions data from the **NAEI**, estimated transport-based emissions via **Google Maps**, structured datasets in **MySQL**, and developed two RESTful APIs that allow real-time fetching of CO₂, NO₂, PM₂.₅, and travel-time comparisons. His robust backend enables seamless interaction between frontend visuals and live data sources.  

---

### Workflow  

The EcoVision team followed a **four-week Agile structure**:  
1. **Week 1:** Research, data selection, and conceptual sketching  
2. **Week 2:** Interface design and prototype development  
3. **Week 3:** Backend integration, API development, and data linking  
4. **Week 4:** Testing, refinement, accessibility review, and deployment  

We tracked progress via **Microsoft Teams**, shared documentation, and GitHub commits. Each sprint included review sessions to maintain scope control and ensure all visual and functional elements aligned with our vision.  

---

## Results  

The final platform offers:  
- A **fluid, interactive user experience**, allowing users to visualise pollution levels hour by hour.  
- **API-driven live updates** connecting traffic and air-quality data in real time.  
- A **3D global visualisation** that situates London’s emissions within a broader environmental context.  
- **Action-oriented recommendations**, encouraging individuals, communities, and policymakers to take steps toward cleaner transport and urban living.  

User testing confirmed that participants found the **time-slider and colour-coding** intuitive and appreciated how the map turned abstract data into tangible insights.  

---

## Reflection  

Building EcoVision Synergy taught the team the power of **collaboration across disciplines** — blending coding, data science, and design to turn information into meaning.  

Key lessons included:  
- The challenge of aligning multi-source datasets and time intervals.  
- The importance of **API optimisation** for smooth user interaction.  
- The value of **simplicity over complexity** in communicating data stories.  

This project reinforced how **iteration, collaboration, and storytelling** are as vital to data communication as the code itself.  

Each team member’s unique skill set transformed EcoVision into a **multi-dimensional platform** — technical, creative, and socially impactful.  

---

## Conclusion  

EcoVision Synergy demonstrates how connected data can be transformed into a **narrative of sustainability**. By visualising the links between emissions, traffic, and environmental health, it encourages both **individual awareness and systemic change**.  

Future extensions could include:  
- Integration with **IoT-based air-quality sensors** for real-time updates  
- Borough-level comparisons and predictive analytics  
- Community-driven data contributions  

Ultimately, EcoVision shows how **digital architecture and environmental insight** can converge to empower citizens and policymakers alike — turning invisible pollution into visible knowledge.  

---

## Bibliography  

1. Centre for London. (2023). *Air Quality in the Capital: Monitoring and Impact Report.* London: Centre for London. <https://www.centreforlondon.org/>  
2. Transport for London (TfL). (2024). *Traffic and Congestion Statistics.* London: TfL Open Data. <https://tfl.gov.uk/info-for/open-data-users/>  
3. London Air Quality Network (LAQN). (2024). *NO₂ and PM₂.₅ Data Portal.* King’s College London. <https://www.londonair.org.uk/>  
4. National Atmospheric Emissions Inventory (NAEI). (2024). *UK Atmospheric Emissions Data.* Department for Environment, Food & Rural Affairs. <https://naei.beis.gov.uk/>  
5. Gray, S., & Signorelli, V. (2024). *CASA0017 Teaching Materials: Web Architecture and Mapping.* UCL.  

---

## Declaration of Authorship  

We, **Madina Diallo, Matilda Nelson, Qingshan Luo, Yidan Gao, and Zihang He**, confirm that the work presented in this assessment is our own.  
Where information has been derived from other sources, this has been clearly indicated in the work.  

**Digitally Signed:**  
EcoVision Team  
November 2025  
