#include <WiFi.h>

const char* ssid = "Wexron 2";
const char* password = "";
const char* serverIP = "192.168.168.207";
const int serverPort = 1234;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Establish connection with the server
  if (!client.connected()) {
    Serial.print("Connecting to server...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connected to server");
    } else {
      Serial.println("Connection failed");
      return;
    }
  }

  // Send data to the server (e.g., sensor readings)
  String dataToSend = "Gesture detected";
  client.println(dataToSend);

  // Wait for a response from the server
  while (client.connected()) {
    if (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.println("Server response: " + response);
      // Process the response from the server
      // Implement your logic here
    }
  }

  // Close the connection
  client.stop();
}
