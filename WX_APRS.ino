// hardware :
// adafruit feather M0
// sensor GY21P or SHT21
// breakoutadafruit GPS

#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <Sodaq_SHT2x.h>

///////////////////////////////////////////////////////////////////////////////////

int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

String call = "YD1RUH-13"; //your callsign
String digi = ">APRSWX,";
String path = "TCPIP*:=";

#define HARDWARE_SERIAL
#define setPin 9

#if defined HARDWARE_SERIAL
#include <HardwareSerial.h>
#define GPSSerial Serial1

#elif defined SOFTWARE_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial GPSSerial( 51, 52 );  // M-Duino: Rx - 50, 51, 52; Tx - 3, 50, 51, 52
#endif

#include <Adafruit_GPS.h>  // uncomment line 42 in Adafruit_GPS.h to #define USE_SW_SERIAL
Adafruit_GPS GPS(&GPSSerial);
#define GPSECHO false
#define VT100 true
// Set DMS to 'true' to turn on DD° MM' SS"
// Set DMS to 'false' to turn on DDMM.MMMM
#define DMS false

uint32_t timer = millis();
int count = 100;

///////////////////////////////////////////////////////////////////////////////////

void setup()
{
  while (!Serial);  // uncomment to have the sketch wait until Serial is ready
  Serial.begin(115200);
  Wire.begin();
  pinMode(13, OUTPUT);
  Serial.println("Adafruit GPS library basic parsing test!");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

///////////////////////////////////////////////////////////////////////////////////

void TX() {
  float tempC;
  float tempF;
  int alt;
  int spd;
  long alt_a;
  long tempF_a;
  long spd_a;
  long hum_a;
  float dew;

  Serial.print(call + digi + path);

  if ( DMS )
  {
    int   latDegs = GPS.latitude / 100;
    int   latMins = GPS.latitude - ( 100 * latDegs );
    float latSecs = 60 * ( GPS.latitude - ( 100 * latDegs ) - latMins );
    Serial.print( latDegs ); Serial.print( "° " ); Serial.print( latMins); Serial.print( "\' " ); Serial.print( latSecs, 2 ); Serial.print( "\" " ); Serial.print(GPS.lat);
  }
  else
  {
    String LAT = String(GPS.latitude, 2);
    String LAT_dir = String(GPS.lat);
    if (LAT.length() < 7) {
      String LAT_OK = "0" + LAT + LAT_dir;
      String LAT_FIX = LAT_OK;
      Serial.print(LAT_OK);
    } else {
      Serial.print(GPS.latitude, 2);
      Serial.print(GPS.lat);
    }
  }

  Serial.print("/");

  if ( DMS )
  {
    int   lonDegs = GPS.longitude / 100;
    int   lonMins = GPS.longitude - ( 100 * lonDegs );
    float lonSecs = 60 * ( GPS.longitude - ( 100 * lonDegs ) - lonMins );
    Serial.print( lonDegs ); Serial.print( "° " ); Serial.print( lonMins); Serial.print( "\' " ); Serial.print( lonSecs, 2 ); Serial.print( "\" " ); Serial.print(GPS.lon);
  }
  else
  {
    String LON = String(GPS.longitude, 2);
    String LON_dir = String(GPS.lon);
    if (LON.length() < 7) {
      String LON_OK = "0" + LON + LON_dir;
      String LON_FIX = LON_OK;
      Serial.print(LON_OK);
    } else {
      Serial.print(GPS.longitude, 2);
      Serial.print(GPS.lon);
    }
  }

  Serial.print("_");

  spd = int((GPS.speed));
  spd_a = (long)(spd * 1L);
  String spd_b = String(spd_a);
  do {
    spd_b = "0" + spd_b;
  } while (spd_b.length() < 3);
  Serial.print(".../");
  
  Serial.print(spd_b);
  
  alt = int((GPS.altitude) * 3.28);
  alt_a = (long)(alt * 1L);
  String alt_b = String(alt_a);
  do {
    alt_b = "0" + alt_b;
  } while (alt_b.length() < 6);
  
  tempC = SHT2x.GetTemperature();
  tempF = round(int((tempC * 1.8) + 32));
  tempF_a = (long)(tempF * 1L);
  String tempF_b = String(tempF_a);
  do {
    tempF_b = "0" + tempF_b;
  } while (tempF_b.length() < 3);

  hum = SHT2x.GetHumidity();
  hum_a = (long)(hum * 1L);
  String hum_b = String(hum_a);
  
  dew = SHT2x.GetDewPoint();

  Serial.print("g...t");
  Serial.print(tempF_b);
  Serial.print("r...p...P...b.....h");
  Serial.print(hum_b);
  Serial.print(" REALTIME GPS");
  Serial.print(" dew="); Serial.print(dew);
  Serial.print(" alt="); Serial.print(alt_b); Serial.println(" feet");
}

///////////////////////////////////////////////////////////////////////////////////

void loop() // run over and over again
{
  digitalWrite(13, HIGH);
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  if (GPSECHO)
    if (c) Serial.print(c);
  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    if (GPS.fix)
    {
      TX();
      delay(30000);
    }
    else
    {
      Serial.println("YD1RUH-13>APRSWX,TCPIP*:=0611.00S/10641.80E_.../...g...t...r...p...P...b.....h.. STATIC GPS");
    }
  }
  delay(500);
}
