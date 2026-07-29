#include <WiFiS3.h>


// Wifi

char ssid[] = "XXX";
char pass[] = "YYY";

WiFiServer server(80);


// Pins

const byte hallPin = 2;
const byte ledPin = 4;


// Hall Filter

const unsigned long sensorFilterTime = 200;

int lastHallReading = HIGH;
int stableHallState = HIGH;

unsigned long hallChangeTime = 0;


//Status

bool magnetDetected = false;



// Setup

void setup() {

  Serial.begin(9600);

  pinMode(hallPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);


  Serial.println("Connecting to WiFi...");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }


  Serial.println();
  Serial.println("Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  server.begin();


  hallChangeTime = millis();

}



// Loop

void loop() {

  unsigned long currentTime = millis();


  readHallSensor(currentTime);


  updateLED();


  webpage();

}



// Hall Sensor Filter (So doesn't Glitch)

void readHallSensor(unsigned long currentTime) {


  int hallReading = digitalRead(hallPin);



  // Detect change

  if (hallReading != lastHallReading) {

    lastHallReading = hallReading;

    hallChangeTime = currentTime;

  }



  // Accept only stable readings

  if ((currentTime - hallChangeTime) >= sensorFilterTime) {


    if (stableHallState != lastHallReading) {


      stableHallState = lastHallReading;



      if (stableHallState == LOW) {

        magnetDetected = true;

        Serial.println("Magnet detected");

      }

      else {

        magnetDetected = false;

        Serial.println("Magnet not detected");

      }

    }

  }

}



// LED

void updateLED() {


  if (magnetDetected) {

    // Locked
    digitalWrite(ledPin, LOW);

  }

  else {

    // Open
    digitalWrite(ledPin, HIGH);

  }

}



// UI / Webpg

void webpage() {


  WiFiClient client = server.available();


  if (client) {


    while(client.connected()) {


      if(client.available()) {


        client.read();


        String status;
        String color;
        String icon;



        if (magnetDetected) {

          status = "LOCKED";
          color = "#22c55e";
          icon = "🔒";

        }

        else {

          status = "LOCK OPEN";
          color = "#ef4444";
          icon = "🚪";

        }



        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();



        client.println("<html>");
        client.println("<head>");
        client.println("<meta http-equiv='refresh' content='1'>");


        client.println("<style>");

        client.println("body{");
        client.println("background:#0f172a;");
        client.println("color:white;");
        client.println("font-family:Arial;");
        client.println("text-align:center;");
        client.println("margin-top:80px;");
        client.println("}");

        client.println(".card{");
        client.println("background:#1e293b;");
        client.println("padding:40px;");
        client.println("border-radius:25px;");
        client.println("width:300px;");
        client.println("margin:auto;");
        client.println("}");

        client.println(".icon{font-size:70px;}");

        client.println(".status{");
        client.println("font-size:35px;");
        client.println("font-weight:bold;");
        client.print("color:");
        client.print(color);
        client.println(";}");

        client.println("</style>");

        client.println("</head>");



        client.println("<body>");

        client.println("<div class='card'>");

        client.print("<div class='icon'>");
        client.print(icon);
        client.println("</div>");

        client.println("<h1>Smart Lock</h1>");

        client.print("<div class='status'>");
        client.print(status);
        client.println("</div>");

        client.println("</div>");

        client.println("</body>");

        client.println("</html>");



        break;

      }

    }


    delay(1);
    client.stop();

  }

}