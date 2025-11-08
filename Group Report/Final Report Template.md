---
title: "CASA0017: Web Architecture Final Assessment"
author: "EcoVision Team – Madina Diallo, Matilda Nelson, Qingshan Luo, Yidan Gao, Zihang He"
date: "November 2025"
---

# EcoVision Synergy: Mapping London’s Air Quality and Vehicle Emissions  

## Abstract  

EcoVision Synergy is an interactive web-based visualisation platform that explores the relationship between **vehicle-related CO₂/NO₂ emissions** and **green-space distribution** across London’s 33 boroughs.  

The project aims to make complex environmental data more **accessible and actionable** to the public, enabling users to visualise daily air-quality fluctuations and understand how traffic density and greenery interact to shape urban health. By combining time open datasets, spatial mapping, and intuitive web design, EcoVision empowers Londoners to make informed decisions about travel behaviour, exposure, and sustainability.  

Our team’s focus was on developing a **data-driven, user-friendly interface** that communicates environmental insight through interactivity, design clarity, and open-source transparency. The result is a website that bridges science, technology, and storytelling, helping turn raw data into an experience that informs and inspires change.  

---

## Introduction  

### Background and Context  
Air pollution remains one of London’s most pressing environmental challenges. With rising urban density, increasing private vehicle usage, and uneven green-space distribution, the city faces growing concerns over CO₂ and NO₂ concentrations and their impact on public health.  

Despite the availability of open environmental data, much of it exists in **technical reports and dashboards** that are inaccessible to non-expert audiences. Our project sought to reimagine how citizens engage with this information by transforming complex datasets into a **simple, visual story of London’s air**.  

### Project Aim  
EcoVision Synergy’s mission is to create a **dynamic, interactive web platform** that visualises:  
- Hourly CO₂ and NO₂ emission levels across London boroughs  
- The relationship between traffic congestion and air quality  
- The mitigating effect of urban green spaces  
- Suggested actions for individuals, communities, and policymakers  

### Key Features  
- **Interactive London Borough Map** : Each of the 33 boroughs is represented as a clickable polygon displaying historical data.  
- **Time Slider Functionality** : Users can explore air-quality changes over a 24-hour cycle, observing morning and evening traffic patterns.  
- **Colour-Coded Emissions Visuals** : Red, yellow, and green indicators communicate pollution severity intuitively.  
- **Embeded Solutions**: Suggests practical methods for governments, communities, and individuals to reduce emissions.  

### Methodology  
We combined **open datasets** from the London Air Quality Network, Transport for London (TfL) traffic counts, and satellite-derived green-space indices. The data was processed using **Python and JavaScript libraries** to ensure temporal alignment and geographical accuracy.  

The final interface was built with:  
- **Leaflet.js** – For mapping and geospatial interaction  
- **D3.js** – For data visualisation and dynamic scaling  
- **HTML, CSS, and Node.js** – For structure, styling, and server-side functionality  
- **GitHub Pages** – For project hosting and collaboration  

Our design prioritised **clarity and simplicity**, using familiar map metaphors to engage users while maintaining technical depth for those interested in data exploration.  

---

## Design Process and Roles  

### Team Structure  
- **Madina Diallo (Project Manager & Data Integration Lead):** Coordinated project milestones, integrated datasets, ensured narrative cohesion, and oversaw presentation and repository documentation.  
- **Matilda Nelson (Research & Content Lead):** Synthesised environmental data sources, contextualised pollution impacts, and authored textual content for the website.  
- **Qingshan Luo (Design & Resource Coordinator):** Produced visuals, layouts, and design assets, ensuring consistency across the web interface and report.  
- **Yidan Gao (Data Visualisation Developer):** Developed the time-slider, emission-to-colour mapping logic, and chart visualisations.  
- **Zihang He (Technical Developer):** Implemented JavaScript frameworks, borough interactivity, and responsive web structure.  

### Workflow  
The team followed a **four-week Agile structure**:  
1. **Week 1:** Problem definition, dataset selection, and concept sketching  
2. **Week 2:** Prototype design and basic map rendering  
3. **Week 3:** Data integration, time-slider function, and borough-level interaction  
4. **Week 4:** Testing, refinement, and deployment  

Progress was tracked through Teams , issue logs, and weekly check-ins to maintain collaboration transparency.  

---

## Results  

The final web platform provides:  
- A **smooth, interactive user experience**, allowing users to explore London’s pollution levels hour by hour.  
- A **clear correlation visualisation** between traffic volume and pollutant concentration.  
- An **intuitive overlay of green-space icons** that show the spatial balance (or imbalance) between pollution sources and natural buffers.  
- **Action-oriented insights**, encouraging different stakeholders to adopt sustainable choices.  

Through testing, we found that users quickly understood the relationship between emission levels and traffic times, often citing “visual storytelling” as a key strength of the project.  

---

## Reflection  

The development process revealed the importance of **data storytelling** in environmental communication. Working across datasets taught us the challenges of aligning time intervals, standardising borough codes, and optimising map performance.  

Collaboration was essential: by combining technical, analytical, and creative skills, we transformed open data into a coherent experience. Each team member’s expertise contributed to the platform’s multidimensional nature — **technical accuracy, visual appeal, and public accessibility.**  

We learned how to balance design ambition with scope and deadline constraints. The experience reinforced the importance of **iteration over perfection**, where early testing and simplicity led to better user experiences.  

Overall, EcoVision Synergy taught us how **web architecture can turn environmental complexity into clarity**, and how IoT, open data, and urban analytics can merge to empower everyday decision-making.  

---

## Conclusion  

EcoVision Synergy demonstrates how connected environmental data can be transformed into a **narrative of sustainability**. By visualising vehicle emissions, traffic density, and green-space distribution, the project highlights the need for holistic urban planning and citizen awareness.  

Our platform offers not only a visual exploration but also a **call to action** , encouraging individuals to walk, cycle, or use public transport during cleaner air hours and urging policymakers to invest in green corridors and low-emission zones.  

Beyond this module, the project could evolve into a **live dashboard** integrating IoT sensors, allowing near real-time air-quality monitoring and community-led data contributions.  

EcoVision represents the fusion of **technology, collaboration, and social purpose** , proving that digital tools can make invisible urban challenges visible, understandable, and actionable.  

---

## Bibliography  

1. Centre for London. (2023). *Air Quality in the Capital: Monitoring and Impact Report.* London: Centre for London. <https://www.centreforlondon.org/>  
2. Transport for London (TfL). (2024). *Traffic and Congestion Statistics.* London: TfL Open Data. <https://tfl.gov.uk/info-for/open-data-users/>  
3. London Air Quality Network. (2024). *NO₂ and PM₂.₅ Data Portal.* King’s College London. <https://www.londonair.org.uk/>
4. S., Gray, S., & Signorelli, V. (2024). *CASA0017 Teaching Materials: Web Architecture and Mapping.* UCL.   

---

## Declaration of Authorship  

We, **Madina Diallo, Matilda Nelson, Qingshan Luo, Yidan Gao, and Zihang He**, confirm that the work presented in this assessment is our own.  
Where information has been derived from other sources, we confirm that this has been indicated in the work.

**Digitally Signed:**  
Ecovision Team  
November 2025  

---

