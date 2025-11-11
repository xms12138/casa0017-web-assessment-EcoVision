import express from "express";
import cors from "cors";
import dotenv from "dotenv";
import mysql from "mysql2/promise";

dotenv.config();

const app = express();
app.use(cors());
app.use(express.json());

const pool = mysql.createPool({
  host: process.env.DB_HOST || "127.0.0.1",
  port: Number(process.env.DB_PORT || 3306),
  user: process.env.DB_USER,
  password: process.env.DB_PASSWORD,
  database: process.env.DB_NAME,
  waitForConnections: true,
  connectionLimit: 10
});

app.get("/api/health", async (_req, res) => {
  try {
    const conn = await pool.getConnection();
    await conn.ping();
    conn.release();
    res.json({ ok: true });
  } catch (e) {
    console.error(e);
    res.status(500).json({ ok: false, error: e.message });
  }
});

app.get("/api/hourly", async (req, res) => {
  try {
    const { borough } = req.query;
    if (!borough) {
      return res.status(400).json({ error: "Missing parameter: borough" });
    }

    const [rows] = await pool.query(
      `SELECT borough,
              co2_kg_per_h,
              co_g_per_h,
              pm25_g_per_h,
              vehicles_per_hour
       FROM borough_hour
       WHERE borough = ?
       LIMIT 1`,
      [borough]
    );

    if (!rows[0]) {
      return res.status(404).json({ error: "No data found for this borough" });
    }

    res.json(rows[0]);
  } catch (e) {
    console.error("Error in /api/hourly:", e);
    res.status(500).json({ error: "Internal server error" });
  }
});

app.get("/api/solution", async (req, res) => {
  try {
    const { borough } = req.query;
    if (!borough) {
      return res.status(400).json({ error: "Missing parameter: borough" });
    }

    const [rows] = await pool.query(
      `SELECT borough,
              busiest_street,
              crossing_time_min,
              bicycle_crossing_min,
              per_car_co2_kg_per_h,
              per_car_co_g_per_h,
              per_car_pm25_g_per_h
       FROM busiest_road
       WHERE borough = ?
       LIMIT 1`,
      [borough]
    );

    if (!rows[0]) {
      return res.status(404).json({ error: "No solution data found for this borough" });
    }

    res.json(rows[0]);
  } catch (e) {
    console.error("Error in /api/solution:", e);
    res.status(500).json({ error: "Internal server error" });
  }
});

app.get("/api/boroughs", async (_req, res) => {
  try {
    const [rows] = await pool.query(
      `SELECT DISTINCT borough FROM borough_hour ORDER BY borough`
    );
    res.json(rows.map(r => r.borough));
  } catch (e) {
    console.error("Error in /api/boroughs:", e);
    res.status(500).json({ error: "Internal server error" });
  }
});

const port = Number(process.env.PORT || 3000);
app.listen(port, "0.0.0.0", () => {
  console.log(`Server running at http://0.0.0.0:${port}`);
});
