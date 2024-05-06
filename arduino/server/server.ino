#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Wexron 2";
const char* password = "";
const int serverPort = 1234;

WiFiServer server(serverPort);
bool ledState = false;

const int LED_PIN = 2;     // GPIO pin number for the built-in LED
const int BUZZER_PIN = 18; // GPIO pin number for the buzzer
const int BUZZER_DURATION = 1000; // Duration to play the buzzer in milliseconds (2 seconds)

// Base URL of your API
const char* apiBaseUrl = "http://127.0.0.1:8000/api/";

// Function to send HTTP request
void sendRequest(const char* endpoint) {
  HTTPClient http;
  String url = String(apiBaseUrl) + String(endpoint);
  Serial.println("Sending request to: " + url);
  if (http.begin(url)) {
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("HTTP response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("HTTP request failed: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  } else {
    Serial.println("Failed to connect to server");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Set LED and buzzer pins as outputs
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

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
        // Remove the newline character at the end
        command.trim();

        // Process the command
        if (command == "WAVE_DETECTED") {
          digitalWrite(LED_PIN, HIGH);
          ledState = true;
          sendRequest("appliance1/toggle"); // Send request to toggle Appliance 1
        } else if (command == "TILT_DETECTED") {
          digitalWrite(LED_PIN, LOW);
          ledState = false;
          sendRequest("appliance2/toggle"); // Send request to toggle Appliance 2
        } else if (command == "FLIP_DETECTED") {
          digitalWrite(LED_PIN, LOW);
          ledState = false;
          sendRequest("reset"); // Send request to reset all appliances
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
