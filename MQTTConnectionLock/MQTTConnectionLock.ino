#include <WiFiS3.h>
#include <PubSubClient.h>


// Wifi

char ssid[] = "DarpiU";
char pass[] = "Nirma@13302922";


// MQTT

const char* mqttServer = "10.0.0.227";
const int mqttPort = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


// Pins

const byte hallPin = 2;
const byte ledPin = 4;


// Hall Filter (plus glitch prevention stuff)

const unsigned long sensorFilterTime = 200;

int lastHallReading = HIGH;
int stableHallState = HIGH;

unsigned long hallChangeTime = 0;


// Status of Magnet
bool magnetDetected = false;


// MQTT Reconnect (Incase fails)

void reconnectMQTT() {

  while (!mqttClient.connected()) {

    Serial.print("Connecting to MQTT... ");

    if (mqttClient.connect(
      "SmartLockArduino001",
      "Username",
      "Password"
    )) {

      Serial.println("Connected!");

      // Send current status when reconnecting
      if (magnetDetected) {
        mqttClient.publish(
          "smartlock/status",
          "LOCKED",
          true
        );
      }
      else {
        mqttClient.publish(
          "smartlock/status",
          "OPEN",
          true
        );
      }

    }

    else {

      Serial.print("Failed, rc=");
      Serial.println(mqttClient.state());

      delay(2000);
    }
  }
}


// Setup

void setup() {

  Serial.begin(9600);


  pinMode(hallPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);


  Serial.println("Connecting to WiFi...");


  WiFi.begin(ssid, pass);


  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);
    Serial.print(".");
  }


  delay(1000);


  Serial.println();
  Serial.println("WiFi Connected!");


  Serial.print("Arduino IP: ");
  Serial.println(WiFi.localIP());


  mqttClient.setServer(
    mqttServer,
    mqttPort
  );


  hallChangeTime = millis();


  Serial.println("Ready");

}



// Loop

void loop() {


  if (!mqttClient.connected()) {

    reconnectMQTT();

  }


  mqttClient.loop();


  unsigned long currentTime = millis();


  readHallSensor(currentTime);


  updateLED();

}



// Hall Sensor

void readHallSensor(unsigned long currentTime) {


  int hallReading = digitalRead(hallPin);



  if (hallReading != lastHallReading) {

    lastHallReading = hallReading;

    hallChangeTime = currentTime;

  }



  if ((currentTime - hallChangeTime) >= sensorFilterTime) {



    if (stableHallState != lastHallReading) {


      stableHallState = lastHallReading;



      if (stableHallState == LOW) {


        magnetDetected = true;


        Serial.println("LOCKED");


        mqttClient.publish(
          "smartlock/status",
          "LOCKED",
          true
        );


      }


      else {


        magnetDetected = false;


        Serial.println("OPEN");


        mqttClient.publish(
          "smartlock/status",
          "OPEN",
          true
        );

      }

    }

  }

}



// LED

void updateLED() {


  if (magnetDetected) {


    // Locked
    digitalWrite(
      ledPin,
      LOW
    );


  }

  else {


    // Open
    digitalWrite(
      ledPin,
      HIGH
    );


  }

}
