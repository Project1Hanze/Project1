#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <stdio.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);






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


String lastDisplay = "";
unsigned long lastDisplayMillis = 0;
const unsigned long DISPLAY_UPDATE_MIN_INTERVAL = 250; 


unsigned long triggerLastChange = 0;
bool triggerRawLast = HIGH;
bool triggerStable = HIGH; 
const unsigned long TRIGGER_DEBOUNCE_MS = 50; 


unsigned long lastHUDMillis = 0;
const unsigned long HUD_UPDATE_INTERVAL = 300; 
void renderHUD() {
  const char* stateStr = "?";
  switch (currentState) {
    case SETUP: stateStr = "SETUP"; break;
    case ALIVE: stateStr = "ALIVE"; break;
    case HIT: stateStr = "HIT"; break;
    case DEAD: stateStr = "DEAD"; break;
    case RELOADING: stateStr = "RELOADING"; break;
    case SHOOTING: stateStr = "SHOOTING"; break;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(stateStr);
  display.print(F("Ammo: "));
  display.println(ammo);
  display.print(F("Lives: "));
  display.println(levens);
  display.display();
}

void displayMessage(const char* msg) {
  String s = String(msg);
  unsigned long now = millis();

  if (s == lastDisplay && (now - lastDisplayMillis) < DISPLAY_UPDATE_MIN_INTERVAL) {
    return;
  }
  lastDisplay = s;
  lastDisplayMillis = now;

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(s);
  display.display();
}


void logMsg(const char* msg) {
  Serial.println(msg);
}

void logFmt(const char* prefix, int val) {
  Serial.print(prefix);
  Serial.println(val);
}

void setup() {
  Serial.begin(115200);
  delay(100); 
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    logMsg("SSD1306 allocation failed");
    for (;;); 
  }

  display.display();
  delay(2000); 

  // Clear the buffer
  display.clearDisplay();

  
  display.drawPixel(10, 10, SSD1306_WHITE);



  display.display();
  delay(2000);   
  
  logMsg("System Starting...");
  
  pinMode(PIN_DOOD, INPUT_PULLUP);

  pinMode(PIN_RELOAD, INPUT);

  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_RESET, INPUT_PULLUP);
  pinMode(PIN_HIT, INPUT_PULLUP);
  pinMode(PIN_SETUP, INPUT_PULLUP);
  pinMode(PIN_RDR, INPUT_PULLUP);
  pinMode(PIN_PVP, INPUT_PULLUP);
  
  logMsg("Waiting for game mode selection...");
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
        logMsg("PVP mode selected - 3 lives");
      } else if (rdrPressed){
        levens = 1;
        currentState = ALIVE;
        logMsg("RDR mode selected - 1 life");
      }
      break;
    
    case ALIVE:
      if (hitPressed){
        levens = levens - 1;
        logFmt("Hit! Lives remaining: ", levens);
        startMillis = currentMillis;
        currentState = HIT;
      } else if (levens <= 0 || doodPressed) {
        currentState = DEAD;
        logMsg("DEAD - Press RESET to respawn");
        display.clearDisplay();
      display.setTextSize(2);            
      display.setTextColor(SSD1306_WHITE);       
      display.setCursor(0, 0);          
      display.println(F("ok"));

      } else if (reloadPressed) {
        // Start reloading only when ammo isn't full
        if (ammo < 6) {
          startMillis = currentMillis;
          currentState = RELOADING;
          logMsg("Reloading...");
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
        logMsg("Reset - Select game mode again (PVP/RDR)");
        currentState = SETUP;
      }
      break;

    case RELOADING:
      if ((ammo != 6 && ammo != 0 && currentMillis - startMillis >= period2) ||
          (ammo == 0 && currentMillis - startMillis >= period3)) {
        ammo = 6;
        logFmt("Reloaded, ammo = ", ammo);
        currentState = ALIVE;
      }
      break;
    

    case SHOOTING:
      ammo--;
      logFmt("Ammo: ", ammo);
      if (ammo == 0) {
        display.clearDisplay();
        display.setTextSize(1);             
        display.setTextColor(SSD1306_WHITE);       
        display.setCursor(0, 0);            
        display.println(F("kogels op"));

        display.display();
        
      }
      currentState = ALIVE;
      break;
  }

  if (currentMillis - lastHUDMillis >= HUD_UPDATE_INTERVAL) {
    lastHUDMillis = currentMillis;
    renderHUD();
  }

}