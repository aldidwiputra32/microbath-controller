// ─── State ───────────────────────────────────────────────
var isConnected = false;
var uptimeSeconds = 0;
var uptimeTimer = null;

// ─── update status ────────────────────────────────────────
document.addEventListener("DOMContentLoaded", loadStatus);

async function loadStatus() {
  const res = await fetch("/status");
  const data = await res.json();

  // update status network
  if (data.state_connected === "1") {
    document.getElementById("statusDot").classList.add("online");
    document.getElementById("statusText").textContent = "Online";
    document.getElementById("offlineWarning").classList.add("hidden");
  } else {
    document.getElementById("statusDot").classList.remove("online");
    document.getElementById("statusText").textContent = "Offline";
    document.getElementById("offlineWarning").classList.remove("hidden");
  }
  // info card
  document.getElementById("statIP").textContent = data.ip;
  document.getElementById("statRSSI").textContent = data.rssi + " dBm";
  document.getElementById("infoSSID").textContent = data.station_connected;
  document.getElementById("infoName").textContent = data.device_name;
  document.getElementById("infoMAC").textContent = data.mac;
  document.getElementById("macAddr").innerHTML = data.mac;
  document.getElementById("infoFW").textContent = data.firmware;
  document.getElementById("fwVersion").innerHTML = data.firmware;
  // parameters settings
  document.getElementById("fastInterval").value = data.fastInterval;
  document.getElementById("slowInterval").value = data.slowInterval;
  document.getElementById("retry").value = data.retryCount;
  document.getElementById("authToken").value = data.authToken;

  // document.getElementById("infoInterval").textContent = data.up_interval;
  // document.getElementById("infoLastUpload").textContent = data.up_last;
}

// ─── Tab Switching ────────────────────────────────────────
function switchTab(name, btn) {
  document.querySelectorAll(".tab-pane").forEach(function (p) {
    p.classList.remove("active");
  });
  document.querySelectorAll(".tab-btn").forEach(function (b) {
    b.classList.remove("active");
  });
  document.getElementById("tab-" + name).classList.add("active");
  btn.classList.add("active");
}

// ─── Password Toggle ──────────────────────────────────────
function togglePassword() {
  var inp = document.getElementById("wifiPass");
  var icon = document.getElementById("eyeIcon");
  if (inp.type === "password") {
    inp.type = "text";
    icon.innerHTML =
      '<path d="M17.94 17.94A10.07 10.07 0 0 1 12 20c-7 0-11-8-11-8a18.45 18.45 0 0 1 5.06-5.94"/><path d="M9.9 4.24A9.12 9.12 0 0 1 12 4c7 0 11 8 11 8a18.5 18.5 0 0 1-2.16 3.19"/><line x1="1" y1="1" x2="23" y2="23"/>';
  } else {
    inp.type = "password";
    icon.innerHTML =
      '<path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/>';
  }
}

// ─── WiFi Connect ─────────────────────────────────────────
async function connectWifi(e) {
  e.preventDefault();

  const ssid = document.getElementById("wifiSsid").value.trim();
  const pass = document.getElementById("wifiPass").value.trim();

  if (!ssid) return;

  const btn = document.getElementById("connectBtn");
  btn.disabled = true;
  btn.innerHTML = '<span class="spinner"></span> Connecting...';

  try {
    const res = await fetch("/wifi/connect", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ ssid, pass }),
    });

    const data = await res.json();

    if (data.success) {
      document.getElementById("wifiSuccess").classList.remove("hidden");
      document.getElementById("offlineWarning").classList.add("hidden");

      document.getElementById("infoSSID").textContent = ssid;
      document.getElementById("statIP").textContent = data.ip;
      document.getElementById("statRSSI").textContent = data.rssi + " dBm";

      // update status
      document.getElementById("statusDot").classList.add("online");
      document.getElementById("statusText").textContent = "Online";
    } else {
      alert(data.msg || "Connection failed");
    }
  } catch (err) {
    alert("Network error");
  }

  btn.disabled = false;
  btn.innerHTML = "Update";
}

// ─── Scan Networks ────────────────────────────────────────
async function scanNetworks() {
  const btn = document.getElementById("scanBtn");
  const list = document.getElementById("scanList");

  btn.disabled = true;
  btn.innerHTML = '<span class="spinner dark"></span> Scanning...';

  list.innerHTML = "";

  try {
    const res = await fetch("/wifi/scan");
    const data = await res.json();

    data.networks.forEach((net) => {
      const item = document.createElement("div");
      item.className = "scan-item";

      item.innerHTML = `
          <span class="scan-item-name">${net.ssid}</span>
          <span class="scan-item-rssi">${net.rssi} dBm</span>
      `;

      item.onclick = () => {
        document.getElementById("wifiSsid").value = net.ssid;
      };

      list.appendChild(item);
      document.getElementById("scanResults").classList.remove("hidden");
    });
  } catch (err) {
    list.innerHTML = "Scan failed";
  }

  btn.disabled = false;
  btn.innerHTML = "Scan WiFi";
}

// ─── Save Parameters ──────────────────────────────────────
async function saveParams(e) {
  e.preventDefault();

  const btn = document.getElementById("saveBtn");
  btn.disabled = true;
  btn.innerHTML = '<span class="spinner"></span> Saving...';

  const params = {
    fastInterval: parseInt(document.getElementById("fastInterval").value),
    slowInterval: parseInt(document.getElementById("slowInterval").value),
    retry: parseInt(document.getElementById("retry").value),
    authToken: document.getElementById("authToken").value.trim(),
  };

  try {
    const res = await fetch("/params", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(params),
    });

    const data = await res.json();

    if (data.success) {
      console.log("success");
      btn.classList.add("success");
      btn.innerHTML =
        '<svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="20 6 9 17 4 12"/></svg> Saved!';
    } else {
      alert("Save failed");
    }
  } catch (err) {
    alert("Network error");
  }

  setTimeout(() => {
    btn.disabled = false;
    btn.classList.remove("success");
    btn.innerHTML =
      '<svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/></svg> Save Settings';
  }, 2000);
}

// ─── Signal Bars ──────────────────────────────────────────
function updateSignalBars(rssi) {
  var bars = document.querySelectorAll("#signalBars span");
  var level = rssi > -55 ? 4 : rssi > -65 ? 3 : rssi > -75 ? 2 : 1;
  bars.forEach(function (b, i) {
    b.classList.toggle("active", i < level);
  });
}

// ─── Uptime Counter ───────────────────────────────────────
function startUptime() {
  if (uptimeTimer) return;
  uptimeTimer = setInterval(function () {
    uptimeSeconds++;
    var h = Math.floor(uptimeSeconds / 3600);
    var m = Math.floor((uptimeSeconds % 3600) / 60);
    var s = uptimeSeconds % 60;
    document.getElementById("statUptime").textContent =
      pad(h) + ":" + pad(m) + ":" + pad(s);
    // Simulate last upload
    if (
      uptimeSeconds %
        parseInt(document.getElementById("interval").value || 30) ===
      0
    ) {
      var now = new Date();
      document.getElementById("infoLastUpload").textContent =
        pad(now.getHours()) +
        ":" +
        pad(now.getMinutes()) +
        ":" +
        pad(now.getSeconds());
    }
  }, 1000);
}

function pad(n) {
  return n < 10 ? "0" + n : "" + n;
}

// ─── ESP32 Integration Notes ──────────────────────────────
// Replace simulation timeouts with actual fetch() calls:
//
// Connect WiFi:
//   fetch('/connect', { method: 'POST',
//     headers: {'Content-Type':'application/json'},
//     body: JSON.stringify({ssid: ssid, pass: pass}) })
//
// Scan Networks:
//   fetch('/scan').then(r => r.json()).then(networks => { ... })
//
// Save Parameters:
//   fetch('/params', { method: 'POST',
//     headers: {'Content-Type':'application/json'},
//     body: JSON.stringify(params) })
//
// Get Status (poll every N seconds):
//   fetch('/status').then(r => r.json()).then(data => {
//     data.ip, data.rssi, data.heap, data.uptime, data.ssid
//   })
