#include <SoftwareSerial.h>

// ULTRA SONIC SENSOR PINS
#define echoPin 2             // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3             //attach pin D3 Arduino to pin Trig of HC-SR04
long duration;                // variable for the duration of sound wave travel
int distance; 

// BUZZER PINS
#define buzzer 10              // Buzzer

// WATER SENSOR PINS
#define POWER_PIN  6          // White
#define SIGNAL_PIN A5         // Yellow 
int value = 0;                // variable to store the sensor value

SoftwareSerial mySerial(7, 8); //SIM900 Tx & Rx is connected to Arduino #7 & #8
#define power_on 9 // GSM POWER
// 7 GREEN UPPER
// 8 ORANGE LOWER
char incoming_char=0;
String newString = "";

// Lat and Log
String location_pos;
String lati;
String longi;

void setup() {
  pinMode(power_on, OUTPUT);
  digitalWrite(power_on,LOW);
  delay(1000);
  digitalWrite(power_on,HIGH);
  delay(2000);  
  digitalWrite(power_on,LOW);
  delay(1000); 
  Serial.begin(19200); //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  mySerial.begin(19200); //Begin serial communication with Arduino and SIM900
  // GSM POWER SOFTWARE
  Serial.println("Initializing..."); 
  delay(20000);
  startupSMS();
  // ULTRA SONIC PIN INPUT AND OUTPUT
  pinMode(trigPin, OUTPUT);                   // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);                    // Sets the echoPin as an INPUT
  // BUZZER PIN OUTPUT
  pinMode(buzzer, OUTPUT);                    // Sets the trigPin as an OUTPUT
  // WATER SENSOR PIN OUTPUT
  pinMode(POWER_PIN, OUTPUT);                 // configure D7 pin as an OUTPUT
  delSMS();
  delay(5000);
  mySerial.println(); 
  mySerial.println("AT"); //Handshaking with SIM900
  updateSerial();
  mySerial.println("AT+CMGF=1\r"); 
  delay(100);
  // Set module to send SMS data to serial out upon receipt 
  mySerial.println("AT+CNMI=2,2,0,0,0\r");
  delay(5000);
}

void loop() {
  if(mySerial.available() > 0){
    incoming_char = mySerial.read();
    newString += incoming_char;
    Serial.print(incoming_char);
    /*if(incoming_char == 'l'){
      sendGPS(); //- send GPS Function
    }*/
  }else {
    Serial.print("");
  // Clears the trigPin condition
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);                       // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);          // Reads the echoPin, returns the sound wave travel time in microseconds
    distance = duration * 0.034 / 2;            // Speed of sound wave divided by 2 (go and back)
    String printDistance = "Distance : ";
    String cm = " cm";
    String printing = printDistance + distance + cm;
    if(distance > 201){
      Serial.print("Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    }else if(distance <= 200 && distance >= 101){
      Serial.println(printing);
      digitalWrite(buzzer, HIGH);// HIGH
    }else{
     Serial.println(printing);
      digitalWrite(buzzer, HIGH); // HIGH
      delay(1000); 
      digitalWrite(buzzer, LOW);
      delay(1000);  
      digitalWrite(buzzer, HIGH); // HIGH
      delay(1000);  
      digitalWrite(buzzer, LOW);
      delay(1000);  
    }
      
    // WATER SENSOR LOOP CODE
    digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
    delay(10);                      // wait 10 milliseconds
    value = analogRead(SIGNAL_PIN); // read the analog value from sensor
    digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF
    Serial.print("Water Detector Value: ");
    Serial.println(value);
      
  }
  if (newString.endsWith("Loc")){
    sendGPS(); 
  }
  if (newString.endsWith("Location")){
    sendGPS(); 
  }
  if (newString.endsWith("loc")){
    sendGPS(); 
  }
  if (newString.endsWith("location")){
    sendGPS(); 
  }

}

void updateSerial()
{
  delay(500);
  while (Serial.available()) {
    mySerial.write(Serial.read());
  }
  while(mySerial.available()) {
    Serial.write(mySerial.read());
  }
}

void sendGPS(){

  lati = "13.44444";
  longi = "14.55555";
  location_pos = "https://www.google.com/maps/place/" + lati + "," + longi;
  mySerial.println("AT+CMGF=1"); 
  updateSerial();
  mySerial.println("AT+CMGS=\"+639297863943\"");
  updateSerial();
  mySerial.println(location_pos); 
  updateSerial();
  mySerial.write(26); 
  delay(10000);
  delSMS();
  delay(5000);
  mySerial.println("AT"); 
  updateSerial();
  mySerial.println("AT+CMGF=1\r"); 
  delay(100);

  mySerial.println("AT+CNMI=2,2,0,0,0\r");
  delay(5000);    
}

void startupSMS(){
  mySerial.println("AT"); 
  updateSerial();
  
  mySerial.println("AT+CMGF=1\r"); 
  delay(100);

  mySerial.println("AT+CMGS=\"+639297863943\"");
  updateSerial();
  
  mySerial.println("Booting Blind Stick Navigation"); 
  updateSerial();
  
  mySerial.write(26); 
  // Give module time to send SMS
  delay(10000);
}

void delSMS() {  // Delete All messages
  mySerial.print("AT+CMGDA=\""); 
  mySerial.println("DEL ALL\"");
  delay(500);
  Serial.println( "All Messages Deleted" );
}
