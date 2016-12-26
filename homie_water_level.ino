
#include <Homie.h>

#define TRIG D1
#define ECHO D2

#define UPDATE_INTERVAL 60
#define TANK_DEPTH 200  // Depth of the tank in cm

HomieNode levelNode("depth", "depth");

unsigned long lastUpdate = 0 ;

void setup() {
  Serial.begin(115200);
  Homie.setLoopFunction(loopHandler);
  Homie.setFirmware("tankdepth", "1.0.0");
  
  Homie.setNodeProperty(levelNode, "unit", "m", true);

  Homie.registerNode(levelNode);  

  Homie.setup();
}

void loopHandler() {
  if (millis() - lastUpdate >= UPDATE_INTERVAL * 1000UL || lastUpdate == 0) {
    float depth = getDepth();
    Homie.setNodeProperty(levelNode, "depth", String(depth), true);
    lastUpdate = millis();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Homie.loop();
}

float getDepth() {
  float results[5];
  int i = 0;
  float reading;
  float avg;

  // Try 10 times
  for (int l = 0; l<10; l++) {
    reading = pingDistance();
    delay(100); // Let things settle down
    if (reading < 0) {
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

  Serial.print("getDepth avg distance measured: ");
  Serial.println(avg);
  
  avg = TANK_DEPTH - avg;

  if (avg < 1) {
    Serial.println("avg < 1!");
    avg = 1;
  }

  Serial.print("getDepth tank depth measured: ");
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

