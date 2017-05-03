#include <Wire.h>
#include "TSL2561.h"
#include <LibHumidity.h>
#include<SoftwareSerial.h>
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
//
TSL2561 tsl(TSL2561_ADDR_FLOAT);
LibHumidity humidity = LibHumidity(0);

float temp , humi , light, soil , vpd;

#define VREF 5000
#define SCOUNT  30
int analogBuffer[SCOUNT];    //store the sample voltage
int analogBufferIndex = 0;

const int staion = 10;

int printTime = 0, printTimeInterval = 2000;
SoftwareSerial xbee(9, 8);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  xbee.begin(9600);
  Serial.println("Initializing...");
  //ms.begin(9600);
  tsl.setGain(TSL2561_GAIN_16X);
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);
  //setDateDs1307(); //Set current time;
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
    light = getLight();
    
//  light = 103;
//  temp = 32.3;
//      humi = 52.4;
//      soil = 50.0;
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

  



  //ms.println(str);
}

float getVpd(float t , float rh) {
  float spv = (610.7) * pow(10, ( (7.5 * t) / (237.3 + t)));
  return (1 - (rh / 100)) * spv;
}

uint32_t getLight() {
  uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full, vis;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  vis = tsl.calculateLux(full, ir);
  return vis;
}

void getSoil() {
  static unsigned long sampleTimepoint = millis();
  if (millis() - sampleTimepoint > 40U)
  {
    sampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(0) / 1024.0 * VREF; //read the voltage and store into the buffer,every 40ms
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT) {
      analogBufferIndex = 0;
      int averageVoltage = getMedianNum(analogBuffer, SCOUNT);  // read the stable value by the median filtering algorithm
      soil = float(5000 - averageVoltage) / 5000 * 100;
      //Serial.println(String(averageVoltage) + " " + String(soil));
    }

  }
}

unsigned int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
  {
    bTab[i] = bArray[i];
  }
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}
