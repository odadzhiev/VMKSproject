const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname)));

let latestData = {};

app.post("/data", (req, res) => {
    latestData = req.body;
    console.log("Received:", latestData);
    res.send("OK");
});

app.get("/data", (req, res) => {
    res.json(latestData);
});

app.listen(3000, () => {
    console.log("Server running on port 3000");
});