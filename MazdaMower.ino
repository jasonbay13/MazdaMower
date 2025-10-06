int ledPin = 13;                    // pin to control the onboard LED
int pulsepin = 4;                   // read high until TDC then goes low
int pulsepout = 8;                  // pin to send signal to coil (mazda 4 wire coil pinout pooking into coil with clip at top: GND-Feedback-Signal-POS)
int maxrpm = 3600;                  // limit rpm spark - math in setup (the map function is hard-coded to avoid extra math during loop)
unsigned long pulsemaxrpm;          // rpm limit calculated in setup
unsigned long pulsedelay = 50000;   // decrease for advance timing (in microseconds) - gets recalculated every rotation
unsigned long microscount = 0;      // microseconds at last rotation
unsigned long microscountdur = 0;   // microseconds between rotations, averaged over the compression and intake strokes
unsigned long microscountdur2 = 0;  // microseconds between rotation 1 (alternating)
unsigned long microscountdur3 = 0;  // microseconds between rotation 2 (alternating)
unsigned long pulseodur = 5000;     // output pulse duration (dwell time) - gets recalculated every rotation
int everyother = 0;                 // gets set 0 or 1 every rotation and updates microscountdur2/3 accordingly.
unsigned long belay1 = 0;           // timer for advance timing (delay from detection and spark)
unsigned long belay2 = 0;           // timer for dwell time (charge time for coil, setting this too high may damage the coil)
int ppold = 0;                      // last detection state
int ppnew = 0;                      // current detection state (if changed from last, we have the leading edge of detection)
bool spark = false;
bool waitadv = false;
bool waitdwell = false;

void setup() {
  //Serial.begin(9600); // enable for debugging only - too slow for timing to remain accurate
  pinMode(ledPin, OUTPUT);
  pinMode(pulsepin, INPUT);
  pinMode(pulsepout, OUTPUT);
  pulsemaxrpm = 1 * 60 * 1000000 / maxrpm;
}

void loop() {
  ppnew = digitalRead(pulsepin);  // read from MOC7811 or equivalent (or hall effect)
  if (ppnew != ppold) {           // this is needed so we don't get any double detections and while loops block micros()
    ppold = ppnew;
    if (ppnew == HIGH) {  // just transitioned from low to high - means we need a spark!
      spark = true;
      if (everyother == 0) {
        microscountdur2 = micros() - microscount;  // save rpm value in microseconds
        everyother = 1;
      } else {
        microscountdur3 = micros() - microscount;  // save rpm value in microseconds
        everyother = 0;
      }
      microscountdur = (microscountdur2 + microscountdur3) / 2;  // save averaged rpm value in microseconds
      microscount = micros();                                    // reset rpm counter for next detection
    }
    //Serial.print(pulsedelay);
    //Serial.print(" ");
    //Serial.println(microscountdur);
  }
  if (spark) {                   //advance timing here
    if (waitadv || waitdwell) {  // means we have a spark waiting for the correct time
      if (waitadv) {             // check to see if we have waited long enough to start the spark
        if (micros() - belay1 >= pulsedelay) {
          digitalWrite(ledPin, HIGH);
          digitalWrite(pulsepout, HIGH);
          belay2 = micros();  // start timer for pulseodur
          waitdwell = true;   // start timer for pulseodur
          waitadv = false;    // stop timer for pulsedelay as we just completed the start of the dwell-time
        }
      }
      if (waitdwell) {  // check to see if we have waited long enough to end the dwell-time and allow the coil to produce it's spark
        if (micros() - belay2 >= pulseodur) {
          digitalWrite(pulsepout, LOW);
          digitalWrite(ledPin, LOW);
          waitdwell = false;  // stop timer for pulseodur as we just completed the spark cycle
          spark = false;      // start all over again and wait for the next valid revolution
        }
      }
    } else {                                                                 // spark enabled but not waiting for set timing based on rpm - set timing here
      if (microscountdur >= pulsemaxrpm && microscountdur <= 150000) {       // under 3600 rpm and over 400 rpm
        pulsedelay = map(microscountdur / 10, 1666, 10000, 330, 2200) * 10;  // map delay for spark advance - adjust according to position of sensor on crank
        pulseodur = map(microscountdur / 10, 1666, 15000, 200, 600) * 10;    // map dwell time 2ms at 3600rpm to 6ms at 400rpm
        belay1 = micros();                                                   // start timer for pulsedelay
        waitadv = true;                                                      // start timer for pulsedelay
      } else {                                                               // out of rpm range, do nothing (disable spark this rotation)
        spark = false;
      }
    }
  }
}
