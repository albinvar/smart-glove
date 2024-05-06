#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "Wexron 2";
const char* password = "";
const char* serverIP = "192.168.28.207";
const int serverPort = 1234;
const int tiltPin = 5; // Tilt sensor pin
bool ledState = false;
const int LED_PIN = 2;     // GPIO pin number for the built-in LED

WiFiClient client;
MPU6050 mpu;

bool lastWaveState = false;
bool lastFlipState = false;
bool lastTiltState = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize MPU6050
  Wire.begin(21, 22); // Specify SDA and SCL pins
  mpu.initialize();

  // Set up tilt sensor pin
  pinMode(tiltPin, INPUT);

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
  // Read accelerometer data from MPU6050
  int16_t ax, ay, az;

  // Establish connection with the server
  if (!client.connected()) {
    Serial.print("Connecting to server...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connected to server");
    } else {
      Serial.println("Connection failed");
      delay(5000); // Retry after 5 seconds
      return;
    }
  }

  // Wait for a response from the server (optional)
  while (client.connected()) {
    mpu.getAcceleration(&ax, &ay, &az);

    // Detect gestures and send commands accordingly
    if (waveDetected(ax, ay, az) && !lastWaveState) {
      client.println("WAVE_DETECTED");
      lastWaveState = true;
       digitalWrite(LED_PIN, HIGH);
       delay(100);
       digitalWrite(LED_PIN, LOW);
    } else if (!waveDetected(ax, ay, az)) {
      lastWaveState = false;
    }

    if (flipDetected(ax, ay, az) && !lastFlipState) {
      client.println("FLIP_DETECTED");
      lastFlipState = true;
    } else if (!flipDetected(ax, ay, az)) {
      lastFlipState = false;
    }

    if (tiltDetected(ax, ay, az) && !lastTiltState) {
      client.println("TILT_DETECTED");
      lastTiltState = true;
    } else if (!tiltDetected(ax, ay, az)) {
      lastTiltState = false;
    }

    delay(500); // Wait for 0.5 seconds before checking again
  }

  // Close the connection
  client.stop();

  // Delay before sending next command
  delay(100); // Delay for 1 second
}

// Function to detect wave gesture based on accelerometer readings
bool waveDetected(int16_t ax, int16_t ay, int16_t az) {
  // Check if ax, ay, and az are negative at the same time
  if (ax < 5000 && ay < 0 && az < 0) {
    return true;
  } else {
    return false;
  }
}

// Function to detect flip gesture based on accelerometer readings
bool flipDetected(int16_t ax, int16_t ay, int16_t az) {
  // Check if the device is flipped upside down
  if (az < -8000) {
    return true;
  } else {
    return false;
  }
}

// Function to detect tilt gesture based on accelerometer readings
bool tiltDetected(int16_t ax, int16_t ay, int16_t az) {
  // Check the tilt sensor pin
  if (!digitalRead(tiltPin) == HIGH) {
    return true;
  } else {
    return false;
  }
}
