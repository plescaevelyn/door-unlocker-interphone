/***********************************************************/
/*           Measurements and Actuators Project            */
/*                                                         */
/*                     Contributors:                       */
/*                     Bejenariu Adina                     */
/*                     Lupu Dima Daria                     */
/*                     Pleșca Evelyn-Iulia                 */
/*                                                         */
/************************************************************/
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

#define redLed_pin 4
#define greenLed_pin 2
#define echoPin 7
#define trigPin 5

const int enterCode[] = { 113, 124, 95, 8 };
long duration;
int distance; 

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID 
byte nuidPICC[4];

Servo lacatUsa;

void setup() {

    Serial.begin(9600);
    SPI.begin(); // Init SPI bus
    rfid.PCD_Init(); // Init MFRC522 

    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    pinMode(6, OUTPUT);
    pinMode(redLed_pin, OUTPUT);
    pinMode(greenLed_pin, OUTPUT);
    digitalWrite(redLed_pin, HIGH);

    lacatUsa.attach(3);

    Serial.println("Initializare...");
    lacatUsa.write(50);
    
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
    pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
    Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed

    lcd.init();
    lcd.backlight();
    lcd.clear();------------------------------------------------------
    lcd.setCursor(4,0);
    lcd.print("Hackster");

}

void loop() {
    
    // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <=30){
          lcd.clear();
          lcd.print("Bine ati venit!");
          lcd.setCursor(0,1);
          lcd.print("Prezentati cardul va rog!");

        if (!rfid.PICC_IsNewCardPresent()) {
            return;
        }
        
        
        if (!rfid.PICC_ReadCardSerial())
            return;
    
        
        MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
       
    
        // Check is the PICC of Classic MIFARE type
        if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
            piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
            piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
            Serial.println(F("Your tag is not of type MIFARE Classic."));
            return;
        }
    
        
        Serial.println(F("Card detectat"));
        int enter = 0;
        
        for (byte i = 0; i < 4; i++) {
            nuidPICC[i] = rfid.uid.uidByte[i];
            
            if (nuidPICC[i] == enterCode[i]) {
                enter++;
                Serial.println(nuidPICC[i]);
            }
        }
        if (enter == 4) {
            
            Serial.println("Acces permis");
            lcd.clear();
            lcd.print("Acces permis!");
            lcd.setCursor(0,1);
            lcd.print("Va uram o zi buna!");
            openDoor();
            delay(400);
        }
        else {
            lcd.clear();
            lcd.print("Acces respins!");
            lcd.setCursor(0,1);
            lcd.print("Hai pa!");
            Serial.println("Acces nepermis");
            delay(1000);
        }
        Serial.println();
        //Serial.println("Apropiati cardul pentru citire");
        
    
        // Halt PICC
        rfid.PICC_HaltA();
    
        // Stop encryption on PCD
        rfid.PCD_StopCrypto1();
        }
        if (distance >30)
        lcd.clear();
}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void printHex(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}

void openDoor() {
    digitalWrite(redLed_pin, LOW);
    digitalWrite(greenLed_pin, HIGH);
    lacatUsa.write(100);
    tone(6, 1000);
    delay(2000);
    noTone(6);
    digitalWrite(redLed_pin, HIGH);
    digitalWrite(greenLed_pin, LOW);
    lacatUsa.write(10);
}
