
#include <Homie.h>
#include <HomieFSBrowser.h>

#define TRIG D1
#define ECHO D2

#define UPDATE_INTERVAL 60

#define MAX_VALID_DISTANCE 500

#define FW_NAME "tankdepth"
#define FW_VERSION "2.0.2"
/* Magic sequence for Autodetectable Binary Upload */
const char *__FLAGGED_FW_NAME = "\xbf\x84\xe4\x13\x54" FW_NAME "\x93\x44\x6b\xa7\x75";
const char *__FLAGGED_FW_VERSION = "\x6a\x3f\x3e\x0e\xe1" FW_VERSION "\xb0\x30\x48\xd4\x1a";
/* End of magic sequence for Autodetectable Binary Upload */

HomieNode levelNode("distance", "distance");

unsigned long lastUpdate = 0 ;

void setup() {
  Serial.begin(115200);
  Homie.setLoopFunction(loopHandler);
  Homie.setFirmware(FW_NAME, FW_VERSION);
  
  Homie.setNodeProperty(levelNode, "unit", "m", true);

  Homie.registerNode(levelNode);  

  Homie.setup();

  fsSetup();
}

void loopHandler() {
  if (millis() - lastUpdate >= UPDATE_INTERVAL * 1000UL || lastUpdate == 0) {
    float distance = getDistance();
    Homie.setNodeProperty(levelNode, "distance", String(distance), true);
    lastUpdate = millis();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Homie.loop();
  fsLoop();
}

float getDistance() {
  float results[5];
  int i = 0;
  float reading;
  float avg;

  // Try 10 times
  for (int l = 0; l<10; l++) {
    reading = pingDistance();
    delay(1000); // Let things settle down
    if (reading < 0) {
      continue;
    }
    if (reading > MAX_VALID_DISTANCE) {
      continue;
    }
    results[i] = reading;
    avg += reading;
    i++;
    if (i == 5) {
      break;
    }
  }

  if (i < 3) {
    // Make sure we have a few good measurements.
    return -1;
  }
  
  avg = avg / i;

  Serial.print("getDistance avg distance measured: ");
  Serial.println(avg);
  
  return avg;
}


float pingDistance() {
  unsigned long startt, endt;
  unsigned long _max_time = micros() + 10000;
  float distance;
  char buffer[50];

  pinMode(TRIG, OUTPUT);
  Serial.println("Looping...");
  digitalWrite(TRIG, LOW);   // Set the trigger pin low, should already be low, but this will make sure it is.
  delayMicroseconds(4);             // Wait for pin to go low.
  digitalWrite(TRIG, HIGH);  // Set trigger pin high, this tells the sensor to send out a ping.
  delayMicroseconds(10);            // Wait long enough for the sensor to realize the trigger pin is high. Sensor specs say to wait 10uS.
  digitalWrite(TRIG, LOW);   // Set trigger pin back to low.
  
  Serial.println("Sent...");

  pinMode(ECHO, INPUT);
  while (!digitalRead(ECHO)) {               // Wait for ping to start.
    if (micros() > _max_time) {             // Took too long to start, abort.
        Serial.println("Abort wait...");
        return -1;
    }
  }
  startt = micros();
  _max_time = micros() + 100000;
  
  while (digitalRead(ECHO)) {                // Wait for ping to start.
    if (micros() > _max_time) {             // Took too long to start, abort.
        Serial.println("Abort send...");
        return -1;
    }
  }
  endt = micros();
  Serial.print("Time took:");
  startt = endt - startt;
  Serial.println(startt);
  distance = startt / 57.0;
  Serial.print(distance);
  Serial.println("cm");
  yield();
  return distance;
}

