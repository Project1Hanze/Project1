#include <Arduino.h>

// Pin definitions
const int PIN_DEATH   = 35;
const int PIN_RELOAD  = 34;
const int PIN_TRIGGER = 17;
const int PIN_RESET   = 5;
const int PIN_HIT     = 4;
const int PIN_SETUP   = 16;
const int PIN_RDR     = 19;
const int PIN_PVP     = 18;
const int PIN_LED1    = 2;   // LED for 1 life
const int PIN_LED2    = 12;  // LED for 2 lives
const int PIN_LED3    = 13;  // LED for 3 lives

// Game variables
int lives = 0;
int ammo = 0;
bool magazineInserted = false;

// Timing variables
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long HIT_DURATION = 500;
const unsigned long RELOAD_DURATION_PARTIAL = 1800;  // For partial reload
const unsigned long RELOAD_DURATION_EMPTY = 600;     // For empty reload

// State machine
enum State {
  SETUP,
  ALIVE,
  HIT,
  DEAD,
  RELOADING,
  SHOOTING
};

State currentState = SETUP;

// Button debouncing variables
bool prevTrigger = HIGH; 
bool prevReload  = LOW;  
bool prevDeath   = HIGH;
bool prevReset   = HIGH;
bool prevHit     = HIGH;
bool prevPVP     = HIGH;
bool prevRDR     = HIGH;

unsigned long triggerLastChange = 0;
bool triggerRawLast = HIGH;
bool triggerStable = HIGH; 
const unsigned long TRIGGER_DEBOUNCE_MS = 50; 

// Interrupt flag for hit detection
volatile bool hitInterruptFlag = false;

// ISR for hit sensor
void IRAM_ATTR hitISR() {
  hitInterruptFlag = true;
}

// Function to update life LEDs
void updateLifeLEDs(int lives) {
  digitalWrite(PIN_LED1, lives >= 1 ? HIGH : LOW);
  digitalWrite(PIN_LED2, lives >= 2 ? HIGH : LOW);
  digitalWrite(PIN_LED3, lives >= 3 ? HIGH : LOW);
} 

void setup() {
  Serial.begin(115200);

  // Configure pins
  pinMode(PIN_DEATH, INPUT_PULLUP);
  pinMode(PIN_RELOAD, INPUT);
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_HIT, INPUT);  // IR sensor
  pinMode(PIN_SETUP, INPUT_PULLUP);
  pinMode(PIN_RDR, INPUT_PULLUP);
  pinMode(PIN_PVP, INPUT_PULLUP);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  // Attach interrupt for hit sensor
  attachInterrupt(digitalPinToInterrupt(PIN_HIT), hitISR, FALLING);
  
  startMillis = millis();
}

void loop() {
  // Update current time
  currentMillis = millis();
  
  // Read all inputs
  bool deathButton = digitalRead(PIN_DEATH);
  bool reloadButton = digitalRead(PIN_RELOAD);
  bool trigger = digitalRead(PIN_TRIGGER);
  bool resetButton = digitalRead(PIN_RESET);
  bool hitSensor = digitalRead(PIN_HIT);
  bool setupButton = digitalRead(PIN_SETUP);
  bool rdrButton = digitalRead(PIN_RDR);
  bool pvpButton = digitalRead(PIN_PVP);
  
  // Update magazine status
  magazineInserted = reloadButton;
  
  // Detect button presses (edges)
  bool deathPressed = (deathButton == LOW && prevDeath == HIGH);
  bool reloadPressed = (reloadButton == HIGH && prevReload == LOW);
  bool resetPressed = (resetButton == LOW && prevReset == HIGH);
  bool hitPressed = hitInterruptFlag || (hitSensor == LOW && prevHit == HIGH);  // Use interrupt flag or poll
  bool pvpPressed = (pvpButton == LOW && prevPVP == HIGH);
  bool rdrPressed = (rdrButton == LOW && prevRDR == HIGH);
  
  // Reset interrupt flag
  hitInterruptFlag = false;
  
  // Trigger debouncing
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
  
  // Update previous states
  prevDeath = deathButton;
  prevReload = reloadButton;
  prevTrigger = trigger;
  prevReset = resetButton;
  prevHit = hitSensor;
  prevPVP = pvpButton;
  prevRDR = rdrButton;
  // State machine
  switch (currentState) {
    case SETUP:
      if (pvpPressed) {
        lives = 3;
        currentState = ALIVE;
      } else if (rdrPressed) {
        lives = 1;
        currentState = ALIVE;
      }
      break;
    
    case ALIVE:
      if (hitPressed) {
        lives--;
        startMillis = currentMillis;
        currentState = HIT;
      } else if (lives <= 0 || deathPressed) {
        currentState = DEAD;
      } else if (reloadPressed && ammo < 6) {
        startMillis = currentMillis;
        currentState = RELOADING;
      } else if (triggerPressed && ammo > 0 && magazineInserted) {
        startMillis = currentMillis;
        currentState = SHOOTING;
      }
      break;

    case HIT:
      if (currentMillis - startMillis >= HIT_DURATION) {
        currentState = ALIVE;
      }
      // Else remain in HIT
      break;
      
    case DEAD:
      if (resetPressed) {
        ammo = 0;
        lives = 0;
        currentState = SETUP;
      }
      break;

    case RELOADING:
      if ((ammo != 6 && ammo != 0 && currentMillis - startMillis >= RELOAD_DURATION_PARTIAL) ||
          (ammo == 0 && currentMillis - startMillis >= RELOAD_DURATION_EMPTY)) {
        ammo = 6;
        currentState = ALIVE;
      }
      break;
    
    case SHOOTING:
      ammo--;
      currentState = ALIVE;
      break;
  }

  // Update life LEDs
  updateLifeLEDs(lives);
}
