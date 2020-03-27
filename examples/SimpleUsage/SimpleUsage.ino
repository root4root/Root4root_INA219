#include <Arduino.h>
#include "Root4root_INA219.h"
//Project page: https://github.com/root4root/Root4root_INA219

Root4root_INA219 ina219; //0x40 address

void setup() {
    Serial.begin(9600);
    ina219.begin();
}

void loop() {
    float busvoltage = 0;
    float current_mA = 0;

    busvoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    
    Serial.println(busvoltage);
    Serial.println(current_mA);

    delay(1000);
}
