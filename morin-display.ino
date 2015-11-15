#include <DHT.h>

#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

int reading = 0;           //reading for distance sensor
int distanceWarning = 50; //warning threashold in some measurement

int delay_time = 3000;           // The higher the number the longer the messages are displayed.

#define CHAR 17   //Number of characters per LCD line
#define ROW  2    //Number of LCD rows

//Congratulation messages comp
char firstrun[][ROW][CHAR] =
{ {"Dr. rer. nat.", "Morin Ostkmap"},
  {"Designing ", "PPPPDS"},
  {"Contribution C1", "STRIDED*!"},
  {"Contribution C2", "Countermeasures!"},
  {"Contribution C3", "IPED Toolkit!"},
  {"Nil desperandum!", ""},
  {"Mr. 10", "publications...."},
  {"453 Seiten", "ja, ne, is klar."},
  {"Es gratulieren:", "Thore & Ina"},
  {"Es gratulieren:", "Dustin & Krissi"},
  {"Es gratulieren:", "Holger & Steffi"},
  {"Es gratulieren:", "Gwenny"},
  {"Florian & Nigel", "sind Helden!111!"},
  {"I have every-", "thing to hide."},
  {"NSA approved and", "monitored device"},
  {"Annova <= 31", "Teilnehmer"},
  {"Beloved UMUX,", "SUS and NASA TLX"},
  {"Multipleye =", "Augenkrebs"},
  {"Morgen trink ich", "mit Euch!"}, 
  {"The cake", "was a lie..."}
};

//Check http://omerk.github.io/lcdchargen/ & http://www.circuitbasics.com/how-to-set-up-an-lcd-display-on-an-arduino/ for custom chars 

#define DHTPIN   2                   //PIN 2 for the DHT11
#define DHTTYPE DHT11
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

DHT dht(DHTPIN, DHTTYPE);

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
        //Check distance to interrupt
        if(getDistance() <= distanceWarning)
        {
            //this is absurd but okay https://www.arduino.cc/en/Tutorial/StringAdditionOperator
            String level="Level "+ (String) getDistance();
            writeToDisplay("Privacy Alert",level,3000);
            lcd.noDisplay();
            delay(700);
            lcd.display();
            delay(700);
            lcd.noDisplay();
            delay(700);
            lcd.display();
            delay(700);
            lcd.display();
            delay(700);
            lcd.noDisplay();
            delay(700);
            lcd.display();
        }
        else {
             //As I don't know how to push to the array... well it works :P
             if(i==sizeof(firstrun)/ sizeof(firstrun[0])) {
             ambient();
             }
             //if nothing is there display the congrat messages
             else {
             lcdWrite(firstrun[i][0], firstrun[i][1]);
           }
        }
   }
  return;
}

void writeToDisplay(String line1,String line2, int delay_time)
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
  writeToDisplay("Rinsomat 3000",complete, delay_time);
  return;
}

//Write two lines, delay and clear display
void lcdWrite(char line1[], char line2[]) {
  writeToDisplay(line1,line2, delay_time);
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



