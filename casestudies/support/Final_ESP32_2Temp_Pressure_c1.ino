#include "max6675.h"
#include "BluetoothSerial.h"

int thermoDO_small = 19;
int thermoCS_small= 23;
int thermoCLK_small= 5;

MAX6675 thermocouple_small(thermoCLK_small, thermoCS_small, thermoDO_small);

BluetoothSerial SerialBT;
bool isBluetoothConnected = false;

unsigned long previousMillis = 0;
const unsigned long interval = 200; // Time interval between readings in milliseconds

void setup() {
  Serial.begin(9600);

  Serial.println("MAX6675 test");
  // Wait for MAX chip to stabilize
  delay(200);

  SerialBT.begin("ESP32_C1"); // Set the Bluetooth name of your ESP32
  SerialBT.register_callback(handleBluetoothEvent);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Basic readout test, just print the current temperature
    float temperature_small = (thermocouple_small.readCelsius()-16.0*0)*1.019;
    Serial.print(temperature_small);
    Serial.print(" C\t");

    Serial.print(currentMillis / 1000); // Convert timestamp to seconds

    // Display the hundredths place for seconds
    int hundredths = (currentMillis / 10) % 100;
    if (hundredths < 10) {
      Serial.print(".0");
    } else {
      Serial.print(".");
    }
    Serial.print(hundredths);

    Serial.println(" seconds");

    // Send temperature, pressure, and timestamp via Bluetooth when connected
    if (isBluetoothConnected) {
      String data = String(temperature_small) + "," + String(currentMillis / 1000) + "." + String(hundredths);
      SerialBT.println(data);
    }
  }

  // Adjust the delay to achieve temperature readings every half second
  delay(200);
}

void handleBluetoothEvent(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    isBluetoothConnected = true;
    SerialBT.println("Small Thermocouple (C) , Timestamp (seconds)"); // Send units when connected
  } else if (event == ESP_SPP_CLOSE_EVT) {
    isBluetoothConnected = false;
  }
}
