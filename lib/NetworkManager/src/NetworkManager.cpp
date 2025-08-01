#include <NetworkManager.h>

// TODO: Pindahkan ssid dan password ke environment variable
const char *host = "esp32"; // http://esp32.local
const char *ssid = "TestESPAPDOOR";
const char *password = "1234567891110";

bool clicked = false;
WebServer server(80);

const char indexHtml[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Dosen Teknik, or is it?</title>
    <style>
        html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
        button { 
            background-color: #4CAF50;
            border: none; color: white;
            padding: 16px 40px;
            text-decoration: none;
            font-size: 30px;
            margin: 2px;
            cursor: pointer;}
    </style>
</head><body>
<h1>Pintu Ke Mana Saja</h1>
<button>BUKA PINTU</button>

<script>
const btn = document.querySelector("button");
btn.addEventListener("click", () => {
    btn.disabled = true;
    btn.style.opacity = "50%";

    fetch("/open", { method: "POST" }).then(() => {
        btn.remove();
        const div = document.createElement("div");
        div.innerHTML = '<p>Pintu sedang dibuka...</p><p>Otomatis kembali ke halaman utama dalam 3 detik</p>';
        document.body.appendChild(div);

        setTimeout(() => location.reload(), 3000);
    }).catch(() => {
        btn.remove();
        const div = document.createElement("div");
        div.innerHTML = '<p>Sabar oittt...</p><p>Lagi buka pintu</p>';
        document.body.appendChild(div);

        setTimeout(() => location.reload(), 3000);
    });
});
</script>
</body></html>)rawliteral";

const char OTAUpdate[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Dosen Teknik, or is it?</title>
    <style>
        html { font-family: sans-serif; display: inline-block; margin: 0px auto; text-align: center; background-color: #f0f2f5; }
        body { padding: 20px; }
        h1 { color: #333; margin-bottom: 20px; }
        .container {
            max-width: 500px;
            margin: 0 auto;
            background: #fff;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        form {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }
        input[type="file"] {
            padding: 10px;
            border: 2px solid #ccc;
            border-radius: 5px;
            background-color: #f8f8f8;
        }
        input[type="submit"] {
            background-color: #007bff;
            color: white;
            border: none;
            padding: 12px 20px;
            border-radius: 5px;
            cursor: pointer;
            font-size: 16px;
            transition: background-color 0.3s ease;
        }
        input[type="submit"]:hover {
            background-color: #0056b3;
        }
        #upload-status {
            margin-top: 20px;
            font-size: 1.1em;
            color: #555;
        }
        #progress-bar-container {
            width: 100%;
            background-color: #e0e0e0;
            border-radius: 5px;
            margin-top: 10px;
            overflow: hidden;
            height: 25px;
            display: none; /* Initially hidden */
        }
        #progress-bar {
            height: 100%;
            width: 0%;
            background-color: #28a745;
            text-align: center;
            line-height: 25px;
            color: white;
            transition: width 0.3s ease;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Perbarui Firmware</h1>
        <p>Unggah file firmware (.bin) untuk memperbarui perangkat.</p>
        <form id="upload_form">
            <input type="file" name="update" id="fileInput" required>
            <input type="submit" value="Mulai Perbarui">
        </form>
        <div id="upload-status"></div>
        <div id="progress-bar-container">
            <div id="progress-bar">0%</div>
        </div>
    </div>

<script>
document.addEventListener('DOMContentLoaded', () => {
    const uploadForm = document.getElementById('upload_form');
    const fileInput = document.getElementById('fileInput');
    const statusDiv = document.getElementById('upload-status');
    const progressBarContainer = document.getElementById('progress-bar-container');
    const progressBar = document.getElementById('progress-bar');
    const submitButton = uploadForm.querySelector('input[type="submit"]');

    uploadForm.addEventListener('submit', (e) => {
        e.preventDefault();

        // Check if a file is selected
        if (fileInput.files.length === 0) {
            statusDiv.textContent = 'Pilih file terlebih dahulu!';
            statusDiv.style.color = '#dc3545';
            return;
        }
        
        const file = fileInput.files[0];
        const formData = new FormData();
        formData.append('update', file);

        // Disable button and show status
        submitButton.disabled = true;
        submitButton.style.opacity = "0.5";
        statusDiv.textContent = 'Mengunggah...';
        statusDiv.style.color = '#007bff';

        // Show and reset progress bar
        progressBarContainer.style.display = 'block';
        progressBar.style.width = '0%';
        progressBar.textContent = '0%';

        const xhr = new XMLHttpRequest();

        // Progress event listener
        xhr.upload.addEventListener('progress', (evt) => {
            if (evt.lengthComputable) {
                const percentComplete = Math.round((evt.loaded / evt.total) * 100);
                progressBar.style.width = percentComplete + '%';
                progressBar.textContent = percentComplete + '%';
            }
        }, false);

        // State change listener for success/error
        xhr.onreadystatechange = function() {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                submitButton.disabled = false;
                submitButton.style.opacity = "1";
                if (xhr.status === 200) {
                    statusDiv.textContent = 'Pembaruan berhasil! Perangkat akan restart.';
                    statusDiv.style.color = '#28a745';
                    progressBar.style.width = '100%';
                    progressBar.textContent = '100%';
                    // Optional: add a delay before reloading or showing a success message
                } else {
                    statusDiv.textContent = `Pembaruan gagal. Kode status: ${xhr.status}`;
                    statusDiv.style.color = '#dc3545';
                    progressBar.style.width = '0%';
                    progressBar.textContent = '0%';
                }
            }
        };

        // Open and send the request
        xhr.open('POST', '/update');
        xhr.send(formData);
    });
});
</script>
</body></html>)rawliteral";

NetworkManager::NetworkManager()
{
}

void NetworkManager::begin(MachineState &ms, MachineCore &mc, SemaphoreHandle_t &sm, QueueHandle_t &rq, uint32_t &standbyCount)
{
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(ssid, password);

    if (!MDNS.begin(host))
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
            ;
    }

    server.on("/", HTTP_GET, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", indexHtml); });

    server.on("/open", HTTP_POST, [&ms, &mc, &sm, &rq, &standbyCount]()
              {
        if (clicked)
        {
            server.sendHeader("Connection", "close");
            server.send(400, "text/plain", "0");
        }
        else
        {
            clicked = true;

            // Operasi buka pintu dilakukan DI LUAR loop client
            if (xSemaphoreTake(sm, portMAX_DELAY) == pdTRUE)
            {
                mc.unlockDoor();
                mc.buzzerShort();

                RenderCommand renderCmd = CMD_DRAW_OPEN_BY_NON_CARD_READ;
                xQueueSend(rq, &renderCmd, portMAX_DELAY);

                server.sendHeader("Connection", "close");
                server.send(200, "text/plain", "1");

                uint32_t unlockStart = millis();

                while (millis() - unlockStart < 4500)
                {
                    vTaskDelay(pdMS_TO_TICKS(100));
                }

                mc.lockDoor();
                standbyCount = millis();
                ms = STANDBY_MODE;
                xSemaphoreGive(sm);

                clicked = false;
                }    } });

    server.on("/ota", HTTP_GET, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", OTAUpdate); });

    server.on("/update", HTTP_POST, []()
              {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart(); }, []()
              {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    } });

    server.begin();
}

void NetworkManager::loopWebServer()
{
    server.handleClient();
}