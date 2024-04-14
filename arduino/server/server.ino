#include <WiFi.h>

const char* ssid = "Wexron 2";
const char* password = "";
const int serverPort = 1234;

WiFiServer server(serverPort);

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

  // Start the server
  server.begin();
  Serial.println("Server started");

  Serial.print("Server IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Wait for a client to connect
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Client connected");

    // Read data from the client
    while (client.connected()) {
      if (client.available()) {
        String command = client.readStringUntil('\n');
        Serial.println("Received command: " + command);

        // Process the command (e.g., control appliances)
        // Implement your logic here

        // Respond to the client
        client.println("Command received");
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}
