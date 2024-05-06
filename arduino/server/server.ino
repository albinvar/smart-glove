#include <WiFi.h>
#include <ESP32Servo.h> // Include the ESP32Servo library

const char* ssid = "Wexron 2";
const char* password = "";
const int serverPort = 1234;

WiFiServer server(serverPort);
bool ledState = false;

const int LED_PIN = 2;     // GPIO pin number for the built-in LED
const int BUZZER_PIN = 18; // GPIO pin number for the buzzer
const int SERVO_PIN = 5;  // GPIO pin number for the servo motor
const int BUZZER_DURATION = 2000; // Duration to play the buzzer in milliseconds (2 seconds)

Servo myServo; // Create a servo object

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

  // Attach servo to the servo pin
  myServo.attach(SERVO_PIN);
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
          // Play the buzzer for BUZZER_DURATION milliseconds
          tone(BUZZER_PIN, 2000); // Activate the buzzer at 2000 Hz
          delay(BUZZER_DURATION); // Sound the buzzer for BUZZER_DURATION milliseconds
          noTone(BUZZER_PIN); // Turn off the buzzer
          // Rotate the servo motor to a certain angle (e.g., 90 degrees)
          myServo.write(60); // Adjust the angle as needed
        } else if (command == "TILT_DETECTED") {
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
