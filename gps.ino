
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/*librerias del la pantalla oled*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

/*modelo de la pantalla oled SPI OLED 0,96” Driver SSD1306*/
#include <Adafruit_SSD1306.h>

/*pines de la pantalla oled*/
#define OLED_MOSI 14  //D5 -> PIN ESP8266
#define OLED_CLK 16   //D0 -> PIN ESP8266
#define OLED_DC 13    //D7 -> PIN ESP8266
#define OLED_CS 15    //D8 -> PIN ESP8266
#define OLED_RESET 12 //D6 -> PIN ESP8266

static const int RXPin = D3 /*D2*/, TXPin = D4/*D4*/; // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
static const uint32_t GPSBaud = 9600; //if Baud rate 9600 didn't work in your case then use 4800

/* A continuación informamos al soft del driver,cuales pines del arduino usamos pantalla Oled*/
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget

SoftwareSerial ss(RXPin, TXPin);  // The serial connection to the GPS device

BlynkTimer timer;

float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GPS

/*
 * //llaves wifi de casa de estefania
  char auth[] =  "mtopkuvTFuGzu2F-862haa6c8svfId6s";
  char ssid[] = "FAMILIATABORDA";
  char pass[] = "4358431700";
*/

 //llaves wifi de tres patios
  char auth[] =  "mtopkuvTFuGzu2F-862haa6c8svfId6s";
  char ssid[] = "C3P_GUEST";
  char pass[] = "pensamiento";

/*
 //llaves wifi my home
  char auth[] = "mtopkuvTFuGzu2F-862haa6c8svfId6s"; //Your Project authentication key
  char ssid[] = "TIGO-1106";
  char pass[] = "2NB144200193";
*/

//unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1;       // fixed location for now
  
void setup()
{
  Serial.begin(9600);
  Serial.println("Activando GPS");
  ss.begin(GPSBaud);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(5000L, checkGPS); // every 5s check if GPS is connected, only really needs to be done once

  /*pantalla oled parametros*/
    display.begin(SSD1306_SWITCHCAPVCC); // Inicia el display OLED (Carga Buffer)
    display.clearDisplay(); // Borrar imagen en el OLED
    display.setTextSize(1); // Definir Tamaño del Texto
    display.setTextColor(WHITE); // Definir color del texto. (mono=>Blanco)
    display.setCursor(0,10 ); // Definir posición inicio texto Columna (0) Fila (10)
    display.println("Activando GPS"); // Carga la información al buffer
    display.display(); // Actualiza display con datos en Buffer
    delay(1000); // Demora de 2 segundos.

}

void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No se ha detectado un GPS: verificar el cableado"));
      Blynk.virtualWrite(V4, "GPS ERROR");  // Value Display widget  on V4 if GPS not detected
      
      display.clearDisplay(); // Borrar imagen en el OLED
      display.setTextSize(0.5); // Definir Tamaño del Texto
      display.setTextColor(WHITE); // Definir color del texto. (mono=>Blanco)
      display.setCursor(0,1); // Definir posición inicio texto Columna (0) Fila (10)
      display.println("No se ha detectado un GPS: verificar el cableado"); // Carga la información al buffer
      display.display(); // Actualiza display con datos en Buffer
      delay(1000); // Demora de 2 segundos.
  }
}

void loop()
{
    while (ss.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(ss.read()))
        displayInfo();
    }
  Blynk.run();
  timer.run();
}

void displayInfo()
{ 
  if (gps.location.isValid() ) 
  {
    float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
    float longitude = (gps.location.lng());
    
    Serial.print("LAT:  ");
    Serial.println(latitude, 6);  // float to x decimal places
    
    display.clearDisplay(); // Borrar imagen en el OLED
    display.setTextSize(0.5); // Definir Tamaño del Texto
    display.setTextColor(WHITE); 
    display.setCursor(0,1); // Definir posición inicio texto Columna (0) Fila (10)
    display.println("Lat: "); // Carga la información al buffer
    display.println(latitude, 6);  // float to x decimal places
    display.display(); // Actualiza display con datos en Buffer
    delay(1000); // Demora de 2 segundos.
    
    Serial.print("LONG: ");
    Serial.println(longitude, 6);
    
    display.clearDisplay(); // Borrar imagen en el OLED
    display.setTextSize(0.5); // Definir Tamaño del Texto
    display.setTextColor(WHITE);
    display.setCursor(0,1); // Definir posición inicio texto Columna (0) Fila (10)
    display.println("Lon: "); // Carga la información al buffer
    display.println(longitude, 6);  // float to x decimal places
    display.display(); // Actualiza display con datos en Buffer
    delay(1000); // Demora de 2 segundos.
    
    Blynk.virtualWrite(V1, String(latitude, 6));   
    Blynk.virtualWrite(V2, String(longitude, 6));  
    myMap.location(move_index, latitude, longitude, "GPS_Location");
    spd = gps.speed.kmph();               //get speed
       Blynk.virtualWrite(V3, spd);
       
       sats = gps.satellites.value();    //get number of satellites
       Blynk.virtualWrite(V4, sats);

       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
       Blynk.virtualWrite(V5, bearing);               
  
  }
  Serial.println();
}
