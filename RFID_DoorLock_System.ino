// RFID Door Lock System (Arduino) - Customized Version
// Inspired by SriTu Hobby's tutorial
// Author: Rayon Dhali

#include <Servo.h>                  // Library to control Servo motors
#include <LiquidCrystal_I2C.h>      // Library for I2C LCD display
#include <SPI.h>                    // SPI communication library for RFID
#include <MFRC522.h>                // RFID reader library

#define SS_PIN 10                   // Slave Select pin for RFID module
#define RST_PIN 9                   // Reset pin for RFID module
#define SERVO_PIN 3                 // PWM pin to control Servo motor

String authorizedUID = "A4 1F 23 9B"; // The UID of the authorized RFID card/tag (change this to match your card)
bool lockState = true;                // true = Door Locked, false = Door Unlocked

Servo lockServo;                     // Servo object to control the lock
LiquidCrystal_I2C lcd(0x27, 16, 2);   // LCD object with I2C address 0x27, 16 columns x 2 rows
MFRC522 rfid(SS_PIN, RST_PIN);       // Create MFRC522 RFID object

void setup() {
  Serial.begin(9600);                // Start Serial Monitor for debugging
  SPI.begin();                       // Initialize SPI bus for RFID communication
  rfid.PCD_Init();                   // Initialize RFID module
  lcd.init();                        // Initialize LCD
  lcd.backlight();                   // Turn on LCD backlight
  lockServo.attach(SERVO_PIN);       // Attach servo to designated pin

  lockDoor();                        // Ensure the door starts in locked position
  displayWelcomeMessage();           // Show initial welcome message on LCD
}

void loop() {
  // Wait until a new RFID card is detected
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  // Read the UID of the scanned card
  String scannedUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    scannedUID.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
    scannedUID.concat(String(rfid.uid.uidByte[i], HEX));
  }
  scannedUID.toUpperCase();         // Convert UID string to uppercase for comparison

  Serial.print("Scanned UID: ");     // Print scanned UID in Serial Monitor
  Serial.println(scannedUID);

  lcd.clear();                      // Clear LCD before showing results

  // Check if scanned UID matches the authorized UID
  if (scannedUID == authorizedUID) {
    if (lockState) {                // If currently locked, unlock
      unlockDoor();
      lcd.setCursor(0, 0); lcd.print("Access Granted");
      lcd.setCursor(0, 1); lcd.print("Door Unlocked");
    } else {                        // If already unlocked, lock again
      lockDoor();
      lcd.setCursor(0, 0); lcd.print("Door Locked");
    }
  } else {
    // If UID is invalid, deny access
    lcd.setCursor(0, 0); lcd.print("Access Denied");
  }

  delay(2000);                      // Wait for 2 seconds to show the result
  displayWelcomeMessage();          // Return to welcome screen

  // Halt the current card communication
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// Function to lock the door by moving the servo to locked position
void lockDoor() {
  lockServo.write(70);              // Rotate servo to 70 degrees (adjustable based on hardware)
  lockState = true;                 // Update lock state
}

// Function to unlock the door by moving the servo to unlocked position
void unlockDoor() {
  lockServo.write(160);             // Rotate servo to 160 degrees (adjustable based on hardware)
  lockState = false;                // Update lock state
}

// Function to display the default welcome message on the LCD
void displayWelcomeMessage() {
  lcd.clear();
  lcd.setCursor(3, 0);              // Position cursor at column 3, row 0
  lcd.print("Welcome!");            // Display "Welcome!"
  lcd.setCursor(1, 1);              // Position cursor at column 1, row 1
  lcd.print("Scan your card");      // Prompt user to scan their card
}