#define TINY_GSM_MODEM_SIM800  // Define GSM modem model
#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <toneAC.h>

// I2C address for the LCD
#define I2C_ADDR 0x27

// Define the LCD display
LiquidCrystal_I2C lcd(I2C_ADDR, 16, 2);

String apn = "internet";                    // APN
String apn_u = "";                         // APN-Username
String apn_p = "";                         // APN-Password
String url = "http://bus.w3x.live/api/c"; // URL of Server

SoftwareSerial SWserial(2, 3); // RX, TX

MFRC522 mfrc522(10, 9);  // Define RFID reader pins (SS, RST)

void setup() {
  Serial.begin(115200);
  Serial.println("SIM800 AT CMD Test");
  SWserial.begin(9600);

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.backlight();

  // Initialize the RFID reader
  SPI.begin();
  mfrc522.PCD_Init();

  // Display Intro Message
  lcd.setCursor(0, 0);
  lcd.print("CBPVM System");

  delay(2000);
  // Display a cheerful message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Setting up GSM..");
  delay(2000);
  // Display a cheerful message
  lcd.setCursor(0, 0);
  lcd.print("Configuring GPRS...");

  delay(15000);
  while (SWserial.available()) {
    Serial.write(SWserial.read());
  }
  delay(2000);
  gsmConfigGPRS();
  lcd.setCursor(0, 1);
  lcd.print("Success...!!!");

  delay(2000);

  //clear the screen
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Please scan your");
  lcd.setCursor(0, 1);
  lcd.print("card here...");
}

void loop() {

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    tone(7, 1000, 250);  
    // Read RFID card UID
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    // Make HTTP request with UID as a parameter
    gsmHTTPPost("uid=" + uid);
    delay(1000);  // Adjust delay based on your requirements
    lcd.setCursor(0, 0);
    lcd.print("Please scan your");
    lcd.setCursor(0, 1);
    lcd.print("card here...");
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

void gsmHTTPPost(String postdata) {
  //clear the screen
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Please wait...");

  Serial.println(" --- Start GPRS & HTTP --- ");
  gsmSendSerial("AT+SAPBR=1,1");
  gsmSendSerial("AT+SAPBR=2,1");
  gsmSendSerial("AT+HTTPINIT");
  gsmSendSerial("AT+HTTPPARA=CID,1");
  gsmSendSerial("AT+HTTPPARA=URL," + url);
  gsmSendSerial("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  gsmSendSerial("AT+HTTPDATA=100,1000");
 
  lcd.setCursor(0, 0);
  lcd.print("Sending Request..");
  gsmSendSerial("AT+HTTPACTION=1");
  lcd.setCursor(0, 1);
  lcd.print("Done..!!!");
  delay(7000);  // Wait for HTTP response
  // Read HTTP status code
  String response = gsmReadResponse();
  int statusCode = response.substring(response.indexOf(",") + 1).toInt();

  if (statusCode == 200) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Verified...!!!");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Unauthorized...!!!");
    }

  Serial.println("HTTP Status Code: " + String(statusCode));
  // Read and print HTTP response
  delay(5000);
  response = gsmReadApiResponse();  // Read the actual HTTP response

  Serial.println("HTTP Response:");
  Serial.println(response);

  // Extract the JSON part and store it in a variable
  int jsonStart = response.indexOf("{");
  int jsonEnd = response.lastIndexOf("}");

  String jsonResponse = response.substring(jsonStart, jsonEnd + 1);
  Serial.println("Extracted JSON Response:");
  Serial.println(jsonResponse);

  gsmSendSerial("AT+HTTPTERM");
  gsmSendSerial("AT+SAPBR=0,1");

  //clear the screen
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Success...!");
}


void gsmConfigGPRS() {
  Serial.println(" --- CONFIG GPRS --- ");
  gsmSendSerial("AT+SAPBR=3,1,Contype,GPRS");
  gsmSendSerial("AT+SAPBR=3,1,APN," + apn);
  if (apn_u != "") {
    gsmSendSerial("AT+SAPBR=3,1,USER," + apn_u);
  }
  if (apn_p != "") {
    gsmSendSerial("AT+SAPBR=3,1,PWD," + apn_p);
  }
}

void gsmSendSerial(String command) {
  Serial.println("Send ->: " + command);
  SWserial.println(command);
  delay(1500); // Adjust delay as needed
  while (SWserial.available()) {
    Serial.write(SWserial.read());
  }
  Serial.println();
}

String gsmReadResponse() {
  String response = "";
 
  while (SWserial.available()) {
    char c = SWserial.read();
    response += c;
    delay(10);
  }
  return response;
}

String gsmReadApiResponse() {
  String response = "";
  Serial.println("Send ->: AT+HTTPREAD");
  SWserial.println("AT+HTTPREAD");
  delay(1000); // Adjust delay as needed, increased to 2000 milliseconds
  while (SWserial.available()) {
    char c = SWserial.read();
    response += c;
    // Check for the presence of "OK" indicating the end of the response
    if (response.endsWith("OK")) {
      break;
    }
    delay(10);
  }
  return response;
}