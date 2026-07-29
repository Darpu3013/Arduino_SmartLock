#include <Servo.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


const byte hallPin = 2;
const byte servoPin = 9;


// Nokia 5110 LCD - CLK, DIN, DC, CS, RST
Adafruit_PCD8544 display(13, 11, 5, 7, 6);


// Backlight control
const byte lcdLedPin = 12;


// Servo Settings and Stuff

const int magnetOnPosition  = 0;
const int magnetOffPosition = 180;


// Time holding each position (in ms ofc)
const unsigned long positionHoldTime = 10000;


// Servo movement speed
const unsigned long servoStepInterval = 20;


// Hall Sensor

const unsigned long sensorFilterTime = 200;

int lastHallReading = HIGH;
int stableHallState = HIGH;

unsigned long hallChangeTime = 0;



// Servo

Servo magnetServo;

int currentServoPosition = magnetOnPosition;
int targetServoPosition = magnetOnPosition;

bool servoMoving = false;

unsigned long lastServoStepTime = 0;
unsigned long positionReachedTime = 0;



// LCD Msg

void showLCD(String line1, String line2)
{
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(BLACK);

  display.setCursor(0,0);
  display.println(line1);

  display.setCursor(0,15);
  display.println(line2);

  display.display();
}



// Setup

void setup()
{

  Serial.begin(9600);


  pinMode(hallPin, INPUT_PULLUP);


  // LCD
  pinMode(lcdLedPin, OUTPUT);
  digitalWrite(lcdLedPin, HIGH);

  display.begin();

  display.setContrast(50);

  showLCD("SYSTEM START", "Initializing");


  delay(1000);



  // Servo

  magnetServo.attach(servoPin);

  currentServoPosition = magnetOnPosition;
  targetServoPosition = magnetOnPosition;

  magnetServo.write(currentServoPosition);

  positionReachedTime = millis();
  hallChangeTime = millis();



  showLCD("READY",
          "Magnet Position");


  Serial.println("System started");
}



// Main

void loop()
{

  unsigned long currentTime = millis();


  readHallSensor(currentTime);


  controlServo(currentTime);

}



// Hall Sensor

void readHallSensor(unsigned long currentTime)
{

  int hallReading = digitalRead(hallPin);



  if (hallReading != lastHallReading)
  {

    lastHallReading = hallReading;

    hallChangeTime = currentTime;

  }



  if ((currentTime - hallChangeTime) >= sensorFilterTime)
  {


    if (stableHallState != lastHallReading)
    {


      stableHallState = lastHallReading;



      if (stableHallState == LOW)
      {

        showLCD("MAGNET:",
                "DETECTED");


        Serial.println("Magnet detected");


      }
      else
      {

        showLCD("MAGNET:",
                "NOT DETECTED");


        Serial.println("Magnet not detected");

      }

    }

  }

}



// Servo Control

void controlServo(unsigned long currentTime)
{


  // Wait

  if (!servoMoving &&
      currentTime - positionReachedTime >= positionHoldTime)
  {


    if (targetServoPosition == magnetOnPosition)
    {

      targetServoPosition = magnetOffPosition;

      showLCD("SERVO:",
              "MOVING AWAY");

      Serial.println("Moving magnet away");

    }

    else
    {

      targetServoPosition = magnetOnPosition;

      showLCD("SERVO:",
              "RETURNING");

      Serial.println("Returning magnet");

    }


    servoMoving = true;

  }


  // Move

  if (servoMoving &&
      currentTime - lastServoStepTime >= servoStepInterval)
  {


    lastServoStepTime = currentTime;



    if (currentServoPosition < targetServoPosition)
    {

      currentServoPosition++;

      magnetServo.write(currentServoPosition);

    }


    else if (currentServoPosition > targetServoPosition)
    {

      currentServoPosition--;

      magnetServo.write(currentServoPosition);

    }



    if (currentServoPosition == targetServoPosition)
    {

      servoMoving = false;

      positionReachedTime = currentTime;



      if (targetServoPosition == magnetOnPosition)
      {

        showLCD("POSITION:",
                "MAGNET ON");

      }

      else
      {

        showLCD("POSITION:",
                "MAGNET OFF");

      }

    }

  }

}