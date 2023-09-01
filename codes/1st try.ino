const double VCC = 3.3;             // NodeMCU on board 3.3v vcc
const double R2 = 10000;            // 10k ohm series resistor
const double adc_resolution = 1023; // 10-bit adc

const double A = 0.001129148;   // thermistor equation parameters
const double B = 0.000234125;
const double C = 0.0000000876741; 

char tempToWrite[6]; /*destination array size defined*/
char disp[8];
//for storing function output temperature
float temp;

// OLED 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire, -1);

//esp
#include <ESP8266WiFi.h>
#include <ThingESP.h>

ThingESP8266 thing("Tinallaksika", "Dewameter", "IOT thermometer");

unsigned long previousMillis = 0;
const long INTERVAL = 6000;  
//end of ESP

float tempCalc(){
  double Vout, Rth, temperatures, adc_value; 

  digitalWrite(D8,HIGH);
  adc_value = analogRead(A0);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;
  //Rth = (Vout/(VCC-Vout))*R2;

  /*  Steinhart-Hart Thermistor Equation:
 *  Temperature in Kelvin = 1 / (A + B[ln(R)] + C[ln(R)]^3)
 *  where A = 0.001129148, B = 0.000234125 and C = 8.76741*10^-8  */
  temperatures = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));   // Temperature in kelvin


  temperatures = temperatures - 273.15;  // Temperature in degree celsius
  Serial.print("Temperature = ");
  Serial.print(temperatures);
  Serial.println(" degree celsius");
  delay(500);

  return temperatures;
}

String HandleResponse(String query){

  temp = tempCalc();
  Serial.println("Temperature :");
  Serial.print(temp);
  String Temp = "Temperature: " + String(temp) + "°C \n";

  
  if (query == "temperature") {
   
    return String(Temp);
  }

  else return "Your query was invalid..";

}

void setup() {
  Serial.begin(115200);  /* Define baud rate for serial communication */
  delay(500);
  pinMode(D7,INPUT);
  pinMode(D8, OUTPUT);  

  //OLED
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.display();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,17);
  display.println("Welcome");
  display.display();
  delay(2000);
  display.setTextSize(1);
  display.setCursor(0,55);
  display.println("Dewameter");
  display.display();
  delay(2000);
  
  display.setCursor(100,55);
  display.println("2.0");
  display.display();
  delay(3000);
  

  //ESP init
  //Serial.begin(115200);
  delay(500);
  disply.clearDisplay();
  display.setCursor(20,17);
  display.println("Connecting to Wi-Fi");
  display.display();  
  thing.SetWiFi("TINALLAKSIKA 4518", "Zm68130-");
  thing.initDevice(); 

  delay(2000);

}

void loop() {

  Serial.println(digitalRead(D7));
  if (digitalRead(D7)==1){

  temp = tempCalc();
  display.clearDisplay();
  //display.display();
  dtostrf(temp,5,2,tempToWrite); /*Double converted to string*/
  //disp[0]= tempToWrite + "°C"
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20,27);
  display.print(tempToWrite);
  display.setTextSize(1);
  display.setTextSize(2);
  display.print((char)247);
  display.setTextSize(2);
  display.println("C");
  display.display();
  delay(2000);   
  display.clearDisplay();
  }
   
  //esp
  thing.Handle();
}

