#include <M5Stack.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Enviromental Combo Sensor
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>

#define CCS811_ADDR 0x5B //Default I2C Address
//#define CCS811_ADDR 0x5A //Alternate I2C Address

//Global sensor objects
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;

String wifi_ssid = "ssid";
String wifi_password = "pass";

// Mqtt topic (increase the number according to your topic name)
char mqttCo2Topic[50] = "/EnvCombo/Co2";
char mqttTvocTopic[50] = "/EnvCombo/Tvoc";
char mqttTempTopic[50] = "/EnvCombo/Temperature";
char mqttHumTopic[50] = "/EnvCombo/Humidity";
char mqttPressTopic[50] = "/EnvCombo/Pressure";
WiFiClient httpClient;
PubSubClient client(httpClient);

void setup() {
    
  M5.begin();
  M5.Power.begin();
  Wire.begin(); //initialize I2C bus

  // wifi setup
  WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
  if (checkWifiConnection()) {
    // mqtt setup
    client.setServer("ip_addr", 1883);
    client.connect("M5StackEnvComboClient");
  }

  i2cTest(); // Execute this after I2C initialization

  //This begins the CCS811 sensor and prints error status of .begin()
  CCS811Core::status returnCode = myCCS811.begin();
  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("Problem with CCS811");
    printCCS811DriverError(returnCode);
  }

  //Initialize BME280
  initializeBme280();

  M5.Lcd.setTextSize(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (myCCS811.dataAvailable()) {
    
    //Calling this function updates the global tVOC and eCO2 variables
    myCCS811.readAlgorithmResults();
    
    displayData();
    sendData2Mqtt();
    
    float BMEtempC = myBME280.readTempC();
    float BMEhumid = myBME280.readFloatHumidity();

    //This sends the temperature data to the CCS811
    myCCS811.setEnvironmentalData(BMEhumid, BMEtempC);

    delay(1500); //Wait for next reading
  } else if (myCCS811.checkForStatusError()) {
    Serial.println(myCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
  }
  delay(50); //Wait for next reading
}

//---------------------------------------------------------------
void displayData()
{
  M5.Lcd.fillScreen(BLACK);  // Initialize the screen
  M5.Lcd.setCursor(0, 50); // (x, y)

  M5.Lcd.printf(" Co2: %d ppm\n", myCCS811.getCO2());
  M5.Lcd.printf("TVOC: %d ppb\n", myCCS811.getTVOC());
  M5.Lcd.printf("Temp: %.1f C\n", myBME280.readTempC());
  M5.Lcd.printf(" Hum: %.1f %\n", myBME280.readFloatHumidity());
  M5.Lcd.printf("Pres: %.1f hPa\n", myBME280.readFloatPressure() / 100.0);
}

void sendData2Mqtt() {
  // === Co2 ===
  StaticJsonDocument<100> co2Doc;
  co2Doc["type"] = "co2";
  co2Doc["value"] = myCCS811.getCO2();
  co2Doc["unit"] = "ppm";
  co2Doc["sensor"] = "CCS811";

  char co2Out[100];
  serializeJson(co2Doc,co2Out);
  client.publish(mqttCo2Topic, co2Out);

  // === Tvoc ===
  StaticJsonDocument<100> tvocDoc;
  tvocDoc["type"] = "tvoc";
  tvocDoc["value"] = myCCS811.getTVOC();
  tvocDoc["unit"] = "ppb";
  tvocDoc["sensor"] = "CCS811";
   
  char tvocOut[100];
  serializeJson(tvocDoc, tvocOut);
  client.publish(mqttTvocTopic, tvocOut);

  // === Temperature ===
  StaticJsonDocument<100> tempDoc;
  tempDoc["type"] = "temperature";
  tempDoc["value"] = myBME280.readTempC();
  tempDoc["unit"] = "C";
  tempDoc["sensor"] = "BME280";

  char tempOut[100];
  serializeJson(tempDoc, tempOut);
  client.publish(mqttTempTopic, tempOut);

  // === Humidity ===
  StaticJsonDocument<100> humDoc;
  humDoc["type"] = "humidity";
  humDoc["value"] = myBME280.readFloatHumidity();
  humDoc["unit"] = "%";
  humDoc["sensor"] = "BME280";

  char humOut[100];
  serializeJson(humDoc, humOut);
  client.publish(mqttHumTopic, humOut);

  // === Pressure ===
  StaticJsonDocument<100> pressDoc;
  pressDoc["type"] = "pressure";
  pressDoc["value"] = myBME280.readFloatPressure() / 100.0;
  pressDoc["unit"] = "hPa";
  pressDoc["sensor"] = "BME280";

  char pressOut[100];
  serializeJson(pressDoc, pressOut);
  client.publish(mqttPressTopic, pressOut);
 
}
