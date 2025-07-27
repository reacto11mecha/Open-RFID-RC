#include <NetworkManager.h>

// TODO: Pindahkan ssid dan password ke environment variable
const char *ssid = "TestESPAPDOOR";
const char *password = "1234567891110";

NetworkManager::NetworkManager() : server(80)
{
    this->clicked = false;
}

void NetworkManager::begin()
{
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();

    server.begin();
}

void NetworkManager::renderWebServer(WiFiClient *client, bool isOpenRequest)
{
    client->println("HTTP/1.1 200 OK");
    client->println("Content-type:text/html");
    client->println("Connection: close");
    client->println();

    client->println("<!DOCTYPE html><html>");
    client->println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client->println("<meta http-equiv=\"refresh\" content=\"3;url=/\">"); // Auto refresh ke halaman utama
    client->println("<link rel=\"icon\" href=\"data:,\">");

    client->println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client->println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
    client->println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");

    client->println("<body><h1>Pintu Ke Mana Saja</h1>");

    if (isOpenRequest)
    {
        client->println("<p>Pintu sedang dibuka...</p>");
        client->println("<p>Otomatis kembali ke halaman utama dalam 3 detik</p>");
    }
    else if (!clicked)
    {
        client->println("<p><a href=\"/open\"><button class=\"button\">BUKA PINTU</button></a></p>");
    }
    else
    {
        client->println("<p>Permintaan sedang diproses...</p>");
    }

    client->println("</body></html>");
    client->println();
}