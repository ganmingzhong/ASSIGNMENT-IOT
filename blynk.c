#define BLYNK_TEMPLATE_ID "TMPL6kbGOx8q6"
#define BLYNK_TEMPLATE_NAME "Air monitoring system"
#define BLYNK_AUTH_TOKEN "qJpNVg5rkgjsQq9a9NxWqm1z46vD1n9D"

#include "DHT.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <MQUnifiedsensor.h>



#define DHTPIN 18 // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22


#define VIRTUAL_PIN_TEMP V6
#define VIRTUAL_PIN_HUMIDITY V5
#define VIRTUAL_PIN_MQ7 V4

#define placa3 "ESP32 Dev Module"
#define voltage_Resolution3 5
#define mq7_PIN 34//34 to 12 to 34
#define mq7_TYPE "MQ-7"
#define ADC_Bit_Resolution3 10
#define mq7_RL 10
#define mq7_RO_CLEAN_AIR 27.5
#define mq7_A 99.042
#define mq7_B -1.518

// Define the WiFi network SSID and password.
const char* ssid = "Wokwi-GUEST";
const char* password = "";

char auth[] = BLYNK_AUTH_TOKEN;

DHT dht(DHTPIN, DHTTYPE);
MQUnifiedsensor mq7(placa3, voltage_Resolution3, ADC_Bit_Resolution3, mq7_PIN, mq7_TYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin(); // Initialize the DHT sensor

  // Connect to the WiFi network.
  WiFi.begin(ssid, password);
  // Wait until the connection is established.
  while (WiFi.status() != WL_CONNECTED) {
    // Print a message to the serial port.
    Serial.println("Connecting to WiFi...");
    // Wait 500 milliseconds before checking again.
    delay(500);
  }
  Serial.println("WiFi connected");

  Blynk.begin(auth, ssid, password);

  // MQ7 setup ////////////////////////////////////
  mq7.setRegressionMethod(1);
  mq7.setA(mq7_A);
  mq7.setB(mq7_B);
  mq7.init();
  mq7.setRL(mq7_RL);
  Serial.print("Calibrating please wait.");
  float calcR0_3 = 0;
  for(int i = 1; i<=10; i ++)
  {
    mq7.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0_3 += mq7.calibrate(mq7_RO_CLEAN_AIR);
    Serial.print(".");
  }
  mq7.setR0(calcR0_3/10);
  Serial.println("  done!.");
  
  //if(isinf(calcR0_3)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  //if(calcR0_3 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  mq7.serialDebug(true);
}

void loop() {
  Blynk.run();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.printf("\nFailed to read from DHT sensor!");
    return;
  }
  // Print the humidity and temperature to the serial port
  Serial.printf("\nHumidity: %.2f %, Temperature: %.2f °C\n", h, t);
  delay(200);

  mq7.update();
  //mq7.readSensor();
  //mq7.serialDebug();
  Serial.printf("\nmq7:  %.2f", mq7.readSensor());

  Blynk.virtualWrite(VIRTUAL_PIN_TEMP, t);
  Blynk.virtualWrite(VIRTUAL_PIN_HUMIDITY, h);
  Blynk.virtualWrite(VIRTUAL_PIN_MQ7, mq7.readSensor());
}
