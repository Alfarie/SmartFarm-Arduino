#include <Wire.h>
#include <LibHumidity.h>
#include <SHT1x.h>
#include<SoftwareSerial.h>
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

LibHumidity humidity = LibHumidity(0);

float temp , humi , light, soil , vpd;

#define VREF 5000
#define SCOUNT  30
int analogBuffer[SCOUNT];    //store the sample voltage
int analogBufferIndex = 0;

const int staion = 10;

int printTime = 0, printTimeInterval = 2000;
SoftwareSerial xbee(9, 8);

#define dataPin  10
#define clockPin 11
SHT1x sht1x(A4, A5);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  xbee.begin(9600);
  Serial.println("Initializing...");
  printTime = millis();

}
void loop()
{

  //Serial.println("Test");
  int diffTime = millis() - printTime;
  if (diffTime > printTimeInterval) {
    printTime = millis();

    temp = humidity.GetTemperatureC();
    humi = humidity.GetHumidity();
    vpd = getVpd(temp , humi);
    String str = "";
    str += "{";
    str +=  String(temp) + ",";
    str +=  String(humi) + ",";
    str +=  String(light) + ",";
    str +=  String(soil) + ",";
    str +=  String(vpd) + ",";
    str +=  String(staion);
    str += "}";
    xbee.println(str);
    Serial.println(str);
  }

  getSoil();
}

float getVpd(float t , float rh) {
  float spv = (610.7) * pow(10, ( (7.5 * t) / (237.3 + t)));
  return (1 - (rh / 100)) * spv;
}



void getSoil() {
  soil =  sht1x.readHumidity();
}

