#include <Arduino.h>
//millis wordt gebruikt omdat delay niet kan
//alles gaat via serialmonitor omdat we nog geen displaycode hebben
//als iemand de code kan versimpelen is dat erg fijn




//pins deff
const int PIN_DOOD    = 35;
const int PIN_RELOAD  = 34;
const int PIN_TRIGGER = 2;
const int PIN_RESET   = 5;
const int PIN_HIT     = 4;
const int PIN_SETUP   = 21;
      int levens      = 0;
const int PIN_RDR     = 19;
const int PIN_PVP     = 18;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 250;
const unsigned long period2 = 1800;
const unsigned long period3 = 600;
const unsigned long period4 = 500;
// States
enum State {
  SETUP,
  ALIVE,
  HIT,
  DEAD,
  RELOADING,
  SHOOTING
};

State currentState = SETUP;

// Variabelen
int ammo = 0;
int magazineUit = 1;

// Voor edge-detectie
bool prevTrigger = LOW;
bool prevReload  = LOW;
bool prevDood    = LOW;
bool prevReset   = LOW;
bool prevHit     = LOW;
bool prevPVP     = HIGH;  // Initialize HIGH because INPUT_PULLUP starts HIGH
bool prevRDR     = HIGH;  // Initialize HIGH because INPUT_PULLUP starts HIGH

void setup() {
  Serial.begin(115200);
  delay(1000);  // Give Serial time to initialize
  Serial.println("System Starting...");
  
  pinMode(PIN_DOOD, INPUT_PULLUP);
  pinMode(PIN_RELOAD, INPUT_PULLUP);
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_HIT, INPUT_PULLUP);
  pinMode(PIN_SETUP, INPUT_PULLUP);
  pinMode(PIN_RDR, INPUT_PULLUP);
  pinMode(PIN_PVP, INPUT_PULLUP);
  
  Serial.println("Waiting for game mode selection...");
  startMillis = millis();
}

void loop() {
  // Update current time
  currentMillis = millis();
  
  // Lees inputs
  bool doodKnop   = digitalRead(PIN_DOOD);
  bool reloadKnop = digitalRead(PIN_RELOAD);
  bool trigger    = digitalRead(PIN_TRIGGER);
  bool resetKnop  = digitalRead(PIN_RESET);
  bool hitknop    = digitalRead(PIN_HIT);
  bool setupknop  = digitalRead(PIN_SETUP);
  bool rdr        = digitalRead(PIN_RDR);
  bool pvp        = digitalRead(PIN_PVP);
  
  // Detecteer rising edges (LOW -> HIGH)
  bool doodPressed   = (doodKnop == HIGH   && prevDood   == LOW);
  bool reloadPressed = (reloadKnop == HIGH && prevReload == LOW);
  bool triggerPressed= (trigger == HIGH    && prevTrigger== LOW);
  bool resetPressed  = (resetKnop == HIGH  && prevReset  == LOW);
  bool hitPressed    = (hitknop == HIGH    && prevHit    == LOW);
  bool pvpPressed    = (pvp == HIGH        && prevPVP    == LOW);
  bool rdrPressed    = (rdr == HIGH        && prevRDR    == LOW);

  // Update vorige waarden
  prevDood   = doodKnop;
  prevReload = reloadKnop;
  prevTrigger= trigger;
  prevReset  = resetKnop;
  prevHit    = hitknop;
  prevPVP    = pvp;
  prevRDR    = rdr;
  switch (currentState) {
    case SETUP:
      if (pvpPressed){
        levens = 3;
        currentState = ALIVE;
        Serial.println("PVP mode selected - 3 lives");
      } else if (rdrPressed){
        levens = 1;
        currentState = ALIVE;
        Serial.println("RDR mode selected - 1 life");
      }
      break;
    
    case ALIVE:
      if (hitPressed){
        levens = levens - 1;
        Serial.print("Hit! Lives remaining: ");
        Serial.println(levens);
        startMillis = currentMillis;
        currentState = HIT;
      } else if (levens <= 0 || doodPressed) {
        currentState = DEAD;
        Serial.println("DEAD - Press RESET to respawn");
      } else if (reloadPressed) {
        startMillis = currentMillis;
        currentState = RELOADING;
        Serial.println("Reloading...");
      } else if (triggerPressed && ammo > 0) {
        startMillis = currentMillis;
        currentState = SHOOTING;
      }
      break;

      case HIT:
        if (currentMillis - startMillis >= period4){
         startMillis = currentMillis;
        currentState = ALIVE;
        }else{
        currentState = HIT;
        }
        break;
      
    case DEAD:
      if (resetPressed) {
        ammo = 0;
        magazineUit = 1;
        levens = 0;
        Serial.println("Reset - Select game mode again (PVP/RDR)");
        currentState = SETUP;
      }
      break;

    case RELOADING:
      if ((ammo != 6 && ammo != 0 && magazineUit == 1 && currentMillis - startMillis >= period2) ||
          (ammo == 0 && magazineUit == 1 && currentMillis - startMillis >= period3)) {
        ammo = 6;
        magazineUit = 0;
        Serial.print("Reloaded, ammo = ");
        Serial.println(ammo);
        currentState = ALIVE;
      }
      break;
    

    case SHOOTING:
      ammo--;
      Serial.print("Ammo: ");
      Serial.println(ammo);
      if (ammo == 0) {
        Serial.println("kogels op");
        magazineUit = 1;
      }
      currentState = ALIVE;
      break;
  }
}
