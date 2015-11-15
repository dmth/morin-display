#include <DHT.h>

#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel



// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
unsigned int timer       = 0;
int reading = 0;                     //reading for distance sensor
int distanceWarning = 100; //in some measurement


int delay_time = 1000;           // The higher the number, the slower the timing.

#define CHAR 17   //Number of characters per LCD line
#define ROW  2    //Number of LCD rows

char firstrun[][ROW][CHAR] =
{ {"Dr. rer. nat", "Morin Ostkmap"},
  {"Es gratuliert", "Florian Alt :)"},
  {"Es gratuliert", "Thore"}
};



#define DHTPIN   2                   //PIN 2 for the DHT11
#define DHTTYPE DHT11
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

DHT dht(DHTPIN, DHTTYPE);

int read_LCD_buttons() {              // read the buttons
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

void setup() {
  // lcd stuff
  lcd.begin(16, 2);               
  //Start the privacy meter stuff...
  Wire.begin();
  //rinsomat stuff
  dht.begin();
  Serial.begin(9600);
}

void loop() {
   congratulationMessages();
}

void congratulationMessages()
{   
   for (int i = 0; i <= sizeof(firstrun)/ sizeof(firstrun[0]); i++) {
        int distance = getDistance();
        //Check distance to interrupt
        if(distance <= distanceWarning)
        {
            //this is absurd but okay https://www.arduino.cc/en/Tutorial/StringAdditionOperator
            String level="Level "+ (String) distance;
            writeToDisplay("Privacy Alert",level);
        }
        else {
             //As I don't know how to push to the array... well it works :P
             if(i==sizeof(firstrun)/ sizeof(firstrun[0])) {
             ambient();
             }
             //if nothing is there display
             else {
             lcdWrite(firstrun[i][0], firstrun[i][1]);
           }
        }
   }
  return;
}

void writeToDisplay(String line1,String line2)
{
      lcd.clear();
      lcd.print(line1);
      lcd.setCursor(0,1);
      lcd.print(line2);
      delay(delay_time);
      return;
}

//read Temperature from sensor
int getTemp() {
  return (int) dht.readTemperature();
}

//read Humidity from sensor
int getHumid() {
  return (int) dht.readHumidity();
}

//encapsulate the stuff
void ambient(){
  String tmp = "Temp. "+(String) getTemp()+(char)223;
  String humidity = " RH "+(String) getHumid()+"%";
  String complete = tmp+humidity;
  writeToDisplay("Rinsomat 3000",complete);
}

//Write two lines, delay and clear display
void lcdWrite(char line1[], char line2[]) {
  writeToDisplay(line1,line2);
  return;
}

int getDistance() {
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
  if (2 <= Wire.available())   // if two bytes were received
  {
    reading = Wire.read();  // receive high byte (overwrites previous reading)
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
  }
  Serial.print("Target in Range! (");
  Serial.print(reading);       // print distance
  Serial.println("cm)");
  return (int)reading;
}



