#include <WiFi.h>

const char* ssid = "Wexron 2";
const char* password = "";
const int serverPort = 1234;

WiFiServer server(serverPort);
bool ledState = false;

const int LED_PIN = 2; // GPIO pin number for the built-in LED

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

        // Process the command
        if (command == "TURN_ON_LED") {
          digitalWrite(LED_PIN, HIGH);
          ledState = true;
        } else if (command == "TURN_OFF_LED") {
          digitalWrite(LED_PIN, LOW);
          ledState = false;
        }

        // Respond to the client
        client.println("Command processed");
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected");

    // Print LED state (optional)
    Serial.print("LED state: ");
    Serial.println(ledState);
  }
}
