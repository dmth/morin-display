#include <DHT.h>

#include <LiquidCrystal.h>
#include <Wire.h>
 
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel


 
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
unsigned int timer       = 0;
int reading = 0;                     //reading for distance sensor
int distanceWarning = 100;

#define DHTPIN   2                   //PIN 2 for the DHT11
#define DHTTYPE DHT11 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

DHT dht(DHTPIN, DHTTYPE);
 
int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 
 
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result
 
    if (adc_key_in > 1000) return btnNONE; 
 
    // For V1.1 us this threshold
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP; 
    if (adc_key_in < 450)  return btnDOWN; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT;  
 
   // For V1.0 comment the other threshold and use the one below:
   /*
     if (adc_key_in < 50)   return btnRIGHT;  
     if (adc_key_in < 195)  return btnUP; 
     if (adc_key_in < 380)  return btnDOWN; 
     if (adc_key_in < 555)  return btnLEFT; 
     if (adc_key_in < 790)  return btnSELECT;   
   */
 
    return btnNONE;                // when all others fail, return this.
}
 
void setup(){
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
   lcd.print("Dr. rer. nat");     // print a simple message on the LCD
   lcd.setCursor(0,1);
   lcd.print("Morin Ostkamp");

   Wire.begin();

   Serial.begin(9600);
   Serial.println("DHTxx test!");
   dht.begin();
}
  
void loop(){
   timer = (millis()/1000) / 3; //wechsel alle 7 sekunden

   //change text
   switch (timer % 6 ){               // depending on which button was pushed, we perform an action 
       case 1:{
          if (getDistance() < distanceWarning) {printPrivacyAlert();}else{
            lcd.setCursor(0,0);
            lcd.print("the hat is a lie");
            lcd.setCursor(0,1);
            lcd.print("the lie is a hat");
          }
            break;
       }
       case 2:{
          if (getDistance() < distanceWarning) {printPrivacyAlert();}else{
            lcd.setCursor(0,0);
            lcd.print("Es gratuliert ...          ");
            lcd.setCursor(0,1);
            lcd.print("Florian Alt  ;-)       ");
          }
            break;
       }    
       case 3:{
          if (getDistance() < distanceWarning) {printPrivacyAlert();}else{
            lcd.setCursor(0,0);
            lcd.print("Rinsomat 3000          ");
            lcd.setCursor(0,1);
            lcd.print(getTemp());
            lcd.print((char)223);
            lcd.print("C / ");
            lcd.print(getHumid());
            lcd.print("% rhd        ");
          }
            break;
       }
       case 4:{
          if (getDistance() < distanceWarning) {printPrivacyAlert();}else{
            lcd.setCursor(0,0);
            lcd.print("Distanz          ");
            lcd.setCursor(0,1);
            lcd.print(getDistance());
            lcd.print("           ");
          }
            break;
       }
       case 5:{
          if (getDistance() < distanceWarning) {printPrivacyAlert();}else{
            lcd.setCursor(0,0);
            lcd.print("text 5          ");
            lcd.setCursor(0,1);
            lcd.print("text 5          ");
          }
            break;
       }

       case 0:{
          if (getDistance() < distanceWarning) {printPrivacyAlert();}else{
            timer = 0;
            lcd.setCursor(0,0);
            lcd.print("Dr. rer. nat          ");
            lcd.setCursor(0,1);
            lcd.print("Morin Ostkamp          ");
          }
            break;
       }
       
   }
  

  
  // delay at the end of the full loop:
  delay(1000);
}

//read Temperature from sensor
int getTemp(){
  return (int) dht.readTemperature();
}

//read Humidity from sensor
int getHumid(){
  return (int) dht.readHumidity();
}

int getDistance(){

  //Wire stuff

  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(112); // transmit to device #112 (0x70)
  // the address specified in the datasheet is 224 (0xE0)
  // but i2c adressing uses the high 7 bits so it's 112
  Wire.write(byte(0x00));      // sets register pointer to the command register (0x00)  
  Wire.write(byte(0x51));      // command sensor to measure in "inches" (0x50)
  // use 0x51 for centimeters
  // use 0x52 for ping microseconds
  Wire.endTransmission();      // stop transmitting
  
  // step 2: wait for readings to happen
  delay(70);                   // datasheet suggests at least 65 milliseconds
  
  // step 3: instruct sensor to return a particular echo reading
  Wire.beginTransmission(112); // transmit to device #112
  Wire.write(byte(0x02));      // sets register pointer to echo #1 register (0x02)
  Wire.endTransmission();      // stop transmitting
  
  // step 4: request reading from sensor
  Wire.requestFrom(112, 2);    // request 2 bytes from slave device #112
  
  // step 5: receive reading from sensor
  if(2 <= Wire.available())    // if two bytes were received
  {
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
  }
    Serial.print("Target in Range! (");
    Serial.print(reading);       // print distance
    Serial.println("cm)");

    return reading;
}

void printPrivacyAlert(){
  lcd.setCursor(0,0);
  lcd.print("Privacy ALERT   ");
  lcd.setCursor(0,1);
  lcd.print("privacy at risk");
}

