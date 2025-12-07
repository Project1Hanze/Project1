# Project1
Tijdens dit project moeten we een spel maken
shootoff--> aantal levens = 1 (int levens = 1)
deathmatch -- aantal levens = 3 (werkt nog niet, is nog geen code voor
zit er magazijn in wapen? ja(magazine = 1) nee(magazine = 0 en Serial.print(geen magazijn);)
status wapen = actief (moet nog)
target geraakt = ja(digitalRead(35) HIGH) --> levens over(moet nog), is nu levens = 0 = Serial.print("dood")
zijn kogels op? (nee -->) (ja = (tijdelijk moet nog aangepast worden)magazine = 0 en magazineuit = 1 (werkt niet samen met magazijn aanwezig controle) = Serial.print("kogels op") -->herladen met 2de magazijn moet nog in code
trigger indrukken wordt ir (is nu if (ammo != 0 && levens == 1){ammo = ammo - 1;)
#include <Arduino.h>







const int PIN_DOOD    = 35;
const int PIN_RELOAD  = 34;
const int PIN_TRIGGER = 17;
const int PIN_RESET   = 5;
const int PIN_HIT     = 4;
const int PIN_SETUP   = 16;
      int levens      = 0;
const int PIN_RDR     = 19;
const int PIN_PVP     = 18;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 250;
const unsigned long period2 = 1800;
const unsigned long period3 = 600;
const unsigned long period4 = 500;

enum State {
  SETUP,
  ALIVE,
  HIT,
  DEAD,
  RELOADING,
  SHOOTING
};

State currentState = SETUP;


int ammo = 0;



bool prevTrigger = HIGH; 
bool prevReload  = LOW;  
bool prevDood    = HIGH;
bool prevReset   = HIGH;
bool prevHit     = HIGH;
bool prevPVP     = HIGH;
bool prevRDR     = HIGH;





unsigned long triggerLastChange = 0;
bool triggerRawLast = HIGH;
bool triggerStable = HIGH; 
const unsigned long TRIGGER_DEBOUNCE_MS = 50; 



  








void setup() {
  Serial.begin(115200);







  
  pinMode(PIN_DOOD, INPUT_PULLUP);

  pinMode(PIN_RELOAD, INPUT);

  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_HIT, INPUT_PULLUP);
  pinMode(PIN_SETUP, INPUT_PULLUP);
  pinMode(PIN_RDR, INPUT_PULLUP);
  pinMode(PIN_PVP, INPUT_PULLUP);
  
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
  

  bool doodPressed   = (doodKnop == LOW && prevDood   == HIGH);            // falling edge
  bool reloadPressed = (reloadKnop == HIGH && prevReload == LOW);         // rising edge (active-high)
  bool resetPressed  = (resetKnop == LOW && prevReset  == HIGH);         // falling edge
  bool hitPressed    = (hitknop == LOW && prevHit    == HIGH);           // falling edge
  bool pvpPressed    = (pvp == LOW && prevPVP    == HIGH);               // falling edge
  bool rdrPressed    = (rdr == LOW && prevRDR    == HIGH);               // falling edge


  bool triggerPressed = false;
  if (trigger != triggerRawLast) {
    triggerRawLast = trigger;
    triggerLastChange = currentMillis;
  }
  if ((currentMillis - triggerLastChange) > TRIGGER_DEBOUNCE_MS) {
    if (triggerStable != trigger) {
     
      triggerStable = trigger;
      if (triggerStable == LOW) {
       
        triggerPressed = true;
      }
    }
  }

 
  if (triggerPressed) {
 
  }

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
        
      } else if (rdrPressed){
        levens = 1;
        currentState = ALIVE;
        
      }
      break;
    
    case ALIVE:
      if (hitPressed){
        levens = levens - 1;
        
        startMillis = currentMillis;
        currentState = HIT;
      } else if (levens <= 0 || doodPressed) {
        currentState = DEAD;
        

      } else if (reloadPressed) {
        // Start reloading only when ammo isn't full
        if (ammo < 6) {
          startMillis = currentMillis;
          currentState = RELOADING;
         
        }
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
        levens = 0;
        
        currentState = SETUP;
      }
      break;

    case RELOADING:
      if ((ammo != 6 && ammo != 0 && currentMillis - startMillis >= period2) ||
          (ammo == 0 && currentMillis - startMillis >= period3)) {
        ammo = 6;
     
        currentState = ALIVE;
      }
      break;
    

    case SHOOTING:
      ammo--;
    
      if (ammo == 0) {
  

        
      }
      currentState = ALIVE;
      break;
  }


  }


