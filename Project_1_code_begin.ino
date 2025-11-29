//35 is dood
//34 is reload
//2 is shoot
//5 is revive

int reload;
int vorigeleven  = 1;
int levens = 1;
int sensor = {35};
int ammo = 0;
int magazine;
int magazineuit = 1;
int trigger;
int shot;

void setup() {
Serial.begin(9600);
pinMode(35, INPUT); //tempdoodknop
pinMode(34, INPUT); //reload knop
pinMode(2, INPUT); //trigger
pinMode(5, INPUT); //"reset"
}
void loop() {
/*trigger indrukken*/
if (digitalRead(2) == HIGH && shot == 0 && trigger == 0){
  trigger = 1;
  shot = 1;
}
/*trigger loslaten*/
if (digitalRead(2) == LOW){
  trigger = 0;
}
/*reload sequence start*/
if (digitalRead(34) == HIGH){
  reload = 1;
  magazine = 1;
  }else{
    reload = 0;
}
/*reload sequence controlle*/
if (magazine == 1 && magazineuit == 1 && levens == 1){
  ammo = 6;
  magazineuit = 0;
  Serial.println();
  Serial.print(ammo);
  }
/*tijdelijke out of ammo*/
if (ammo == 0){
  magazine = 0;
   magazineuit = 1;
  }
/*nu dood maar wordt levens-1 knop*/
if (digitalRead(sensor) == HIGH){
  levens = 0;
}
/*0 levens? dood*/
if (levens != vorigeleven && levens == 0){
  Serial.println();
  Serial.print("dood");
  vorigeleven = levens;
}
/*temp reset sequence*/
if(digitalRead(5) == 1 && levens == 0){
  levens = 1;
  vorigeleven = levens;
  ammo = 0;
  Serial.println();
  Serial.print("Back in the game, Ammo = 0");


}
/*schiet = -1 ammo met delay*/
if(shot == 1 && ammo != 0 && levens == 1){
  ammo = ammo - 1;
  delay(250);
  Serial.println();
  Serial.print(ammo);
}
if(shot == 1){
  shot = 0;
}
/*nog kogels = trage reload*/
if (reload == 1 && ammo != 0 && ammo != 6 && levens == 1){
  delay(1500); 
  magazineuit = 1;
}
}



