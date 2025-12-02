// Pin definities
const int PIN_DOOD    = 35;
const int PIN_RELOAD  = 34;
const int PIN_TRIGGER = 2;
const int PIN_RESET   = 5;
const int PIN_HIT     = 4;
const int PIN_SETUP   = 21;
      int levens      = 0
const int PIN_RDR     = 19
const int PIN_PVP     = 18
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 250;
const unsigned long period2 = 1800;
const unsigned long period3 = 600;
const unsigned long period3 = 500;
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

void setup() {
  Serial.begin(9600);
  pinMode(PIN_DOOD, INPUT_PULLUP);
  pinMode(PIN_RELOAD, INPUT_PULLUP);
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_HIT, INPUT_PULLUP);
  pinMode(PIN_SETUP, INPUT_PULLUP);
  startMillis = millis();
}

void loop() {
  // Lees inputs
  bool doodKnop   = digitalRead(PIN_DOOD);
  bool reloadKnop = digitalRead(PIN_RELOAD);
  bool trigger    = digitalRead(PIN_TRIGGER);
  bool resetKnop  = digitalRead(PIN_RESET);
  bool hitknop    = digitalRead(PIN_HIT);
  bool setupknop  = digitalRead(SETUPKNOP);
  bool rdr        = digitalRead(PIN_RDR);
  bool pvp        = digitalRead(PIN_PVP);
  

  // Detecteer rising edges (LOW -> HIGH)
  bool doodPressed   = (doodKnop == HIGH   && prevDood   == LOW);
  bool reloadPressed = (reloadKnop == HIGH && prevReload == LOW);
  bool triggerPressed= (trigger == HIGH    && prevTrigger== LOW);
  bool resetPressed  = (resetKnop == HIGH  && prevReset  == LOW);

  // Update vorige waarden
  prevDood   = doodKnop;
  prevReload = reloadKnop;
  prevTrigger= trigger;
  prevReset  = resetKnop;

  switch (currentState) {
    case SETUP:
      if (pvp && currentState == setup){
        levens = 3;
        currentState = Alive;
    }else if (rdr && currentState == setup){
        levens = 1;
        currentState = Alive;
    
    case ALIVE:
      if (hitknop){
        levens = levens - 1;
        startMillis = currentMillis;
        currentState = hit;
      } else if (levens == 0 || doodPressed) {
        currentState = DEAD;
        Serial.println("dood");
      } else if (reloadPressed) {
        currentState = RELOADING;
      } else if (triggerPressed && ammo > 0 && currentMillis - startMillis >= period) {
        startMillis = currentMillis;
        currentState = SHOOTING;
      }
      break;

      case HIT:
        if (currentMillis - startMillis >= period4){
         startMillis = currentMillis;
        currentState = ALIVE;
        }else{
        currentState = hit;
        }
        break;
      
    case DEAD:
      if (resetPressed) {
        currentState = ALIVE;
        ammo = 0;
        magazineUit = 1;
        Serial.println("Back in the game, Ammo = 0");
      }
      break;

    case RELOADING:
      startMillis = currentMillis;
      if (ammo != 6 && != 0 && magazineUit = 1 && currentMillis - startMillis >= period2){
       ammo = 6;
        magazineUit = 0;
        Serial.print("Reloaded, ammo = ");
        Serial.println(ammo);
        currentState = ALIVE;
      }else if(ammo == 0 && magazineUit == 1 && currentMillis - startMillis >= period3) {
        ammo = 6;
        magazineUit = 0;
        Serial.print("Reloaded, ammo = ");
        Serial.println(ammo);
        currentState = ALIVE;
      }
      currentState = RELOAD;
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
