#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "Wexron 2";
const char* password = "";
const char* serverIP = "192.168.168.207";
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

  // print out the data
  Serial.print("aX = "); Serial.print(ax);
  Serial.print(" | aY = "); Serial.print(ay);
  Serial.print(" | aZ = "); Serial.println(az);

  // Detect tilt
  bool tilted = digitalRead(tiltPin) == HIGH; // Assuming HIGH state indicates tilt

  Serial.println(tilted);
  // Send command to the server based on tilt state
  if (tilted) {
    // Send command to turn on the server LED
    client.println("TURN_ON_LED");
  } else {
    // Send command to turn off the server LED
    client.println("TURN_OFF_LED");
  }

  delay(3000); // Wait for 3 seconds before sending next command

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
