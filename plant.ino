unsigned long lastWater = 0;
unsigned long lastAdjust = 0;
const unsigned long HALFWEEK = 1000UL*43200*7;
//10000UL;
int waterMode = 0; // no watering until an interval is chosen
int interval = 0;
int intervals[5] = {0, 1, 2, 4, 6};
bool tested = false;

#define PUMP 0
#define LIGHT 1

void setup() {
  pinMode(PUMP, OUTPUT);
  pinMode(LIGHT, OUTPUT);
  pinMode(3, INPUT_PULLUP); // pins 3 and 4 are connected to ground for me to interact with the chip
  pinMode(4, INPUT_PULLUP);
  for (int i = 0; i <= 40; i++) {
    analogWrite(LIGHT, i*5);
    delay(30);
  }
  delay(800);
  for (int i = 20; i > 0; i--) {
    analogWrite(LIGHT, i*10);
    delay(10);
  }
  digitalWrite(LIGHT, LOW);
  delay(300);
}

bool timestamp(unsigned long stamp, unsigned long wait) {
  // checks whether or not a number of milliseconds have passed since a timestamp
  bool pastStamp = (millis()-stamp >= wait);
  return pastStamp;
}

void water() {
  // waters the plant and stores the time of watering
  digitalWrite(PUMP, HIGH);
  delay(8000);
  digitalWrite(PUMP, LOW);
  lastWater = millis()-8000;
}
void setInterval(int dm) {
  // sets the interval at which the plant should be watered and displays the new interval on the LED
  waterMode = (waterMode+dm)%5;
  interval = intervals[waterMode];
  if (waterMode == 0) {
    for (int i = 20; i >= 0; i--) {
      analogWrite(LIGHT, i*10);
      delay(30);
    }
  } else if(waterMode == 1) {
    digitalWrite(LIGHT, HIGH);
    delay(250);
    digitalWrite(LIGHT, LOW);
  }
  else {
    for (int i = 0; i < (waterMode-1); i++) {
      digitalWrite(LIGHT, HIGH);
      delay(900);
      digitalWrite(LIGHT, LOW);
      delay(100);
    }
  }
  lastAdjust = millis(); // stores adjustment time so the plant won't be watered immediately
}

void loop() {
  if (tested) // once the three test pumps are done
  {
    if (timestamp(lastAdjust, 2000)) {
      if (!digitalRead(3) && waterMode < 4)
        setInterval(1); // can water less frequently
      else if (!digitalRead(4) && waterMode > 0)
        setInterval(-1); // or more frequently
    }
    if (waterMode != 0
     && timestamp(lastWater, interval*HALFWEEK)
     && timestamp(lastAdjust, 60000)) {
          water();
    }
  } else {
    analogWrite(LIGHT, 90*sin(millis()*0.001)+100);
    if (!digitalRead(3)) { // only start test when I connect pin 3 to ground
      digitalWrite(LIGHT, HIGH);
      for (int i = 0; i < 3; i++) {
        // send three test water pumps to make sure the tube is at the right height
        digitalWrite(PUMP, HIGH);
        delay(2000);
        digitalWrite(PUMP, LOW);
        delay(4000);
      }
      digitalWrite(LIGHT, LOW);
      lastWater = millis()-4000;
      tested = true;
    }
  }
}
