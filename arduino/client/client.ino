#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "Wexron 2";
const char* password = "";
const char* serverIP = "192.168.28.207";
const int serverPort = 1234;
const int tiltPin = 5; // Tilt sensor pin

WiFiClient client;
MPU6050 mpu;

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

    // Print out the accelerometer data
    Serial.print("aX = "); Serial.print(ax);
    Serial.print(" | aY = "); Serial.print(ay);
    Serial.print(" | aZ = "); Serial.println(az);

    // Detect gestures and send commands accordingly
    if (waveDetected(ax, ay, az)) {
      client.println("WAVE_DETECTED");
    } else if (flipDetected(ax, ay, az)) {
      client.println("FLIP_DETECTED");
    } else if (tiltDetected(ax, ay, az)) {
      client.println("TILT_DETECTED");
    }

    delay(500); // Wait for 3 seconds before sending next command

    if (client.available()) {
      String response = client.readStringUntil('\n');
      Serial.println("Server response: " + response);
      // Process the response from the server if needed
    }
  }

  // Close the connection
  client.stop();

  // Delay before sending next command
  delay(1000); // Delay for 1 second
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
  // check the tilt sensor pin
  if (!digitalRead(tiltPin) == HIGH) {
    return true;
  } else {
    return false;
  }
}
