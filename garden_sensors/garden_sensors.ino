
/* Austin Hill
 *  Garden Base Station
 *  Purposes:
 *  * Read temperature and humidity from AHT20
 *  * Read Light readings from BH1750
 *  * Use Sparkfun qwiic mux to read multiple soil moisture sensors
 *  * detect multiple soil moisture sensors
 *  * send data readings to InfluxDB over Wifi
 *  
 *  All of these components need to be allowed to fail.  
 *  The goal is to keep trying and fail gracefully. 
 */
#include <Adafruit_AHTX0.h>
#include <hp_BH1750.h>  //  include the library
#include <SparkFun_I2C_Mux_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_I2C_Mux
#include <Wire.h>

QWIICMUX myMux;
hp_BH1750 lightSensor;       //  create the temp/humidity sensor
bool lightAvailable;

float currentLight;


void setup()
{

  initSerial(115200);
  lightAvailable = initBH1750_light_sensor();

  Wire.begin();
  
  if (myMux.begin() == false)
  {
    Serial.println("Mux not detected. Freezing...");
    while (1)
      ;
  }
  Serial.println("Mux detected");

  myMux.setPort(1); //Connect master to port labeled '1' on the mux

  byte currentPortNumber = myMux.getPort();
  Serial.print("CurrentPort: ");
  Serial.println(currentPortNumber);

  Serial.println("Begin scanning for I2C devices");
}

void loop()
{
  Serial.println();
  if (lightAvailable == true) {
    currentLight = getLightMeasurement();
    Serial.printf("lux: %f\n", currentLight); 
  }

  byte nDevices = 0;
  for (byte address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 0x10)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 0x10)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Done");

  byte currentPortNumber = myMux.getPort();
  Serial.print("CurrentPort: ");
  Serial.println(currentPortNumber);

  delay(1000);
}

void initSerial(int baud){
  Serial.begin(baud);
  Serial.println();
  Serial.println("Garden Base Station Initialization...");
}

bool initBH1750_light_sensor(){
  bool avail = lightSensor.begin(BH1750_TO_GROUND);
  return avail;
}


// getLightMeasurement - non-blocking reading of light sensor data from a BH1750 sensor. 
// Returns a float value, units: lux.
float getLightMeasurement(){
  float lux;
  if (lightSensor.hasValue() == true){
    lux = lightSensor.getLux(); 
    lightSensor.start();
  }
  return lux;
}
