#include <Arduino.h>
#include "Root4root_INA219.h"
//Project page: https://github.com/root4root/Root4root_INA219

Root4root_INA219 ina219(0x40);

void setup() {

    //Max expected current 20A with 10 milliOhms shunt,
    //calculates calibration register value. 2000, 100 by default.
    ina219.begin(20000, 10); 

    //Change configuration register to fit your requirements.
    //Check Readme for more options, or read .h file
    ina219.changeConfig(INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_8S_4260US |
                        INA219_CONFIG_BUS_VOLTAGE_RANGE_16V,
                        INA219_CONFIG_SHUNT_ADC_RESOLUTION_MASK |
                        INA219_CONFIG_BUS_VOLTAGE_RANGE_MASK);
    //Same as above:
    //ina219.changeConfig(0x0058, 0x2078);
    
                        
    Serial.begin(9600);
}

void loop() {
    Serial.println(ina219.getBusVoltage_V());
    Serial.println(ina219.getCurrent_mA());
    Serial.println("--------------------");
    
    //ina219.getBusVoltage_mV();
    //ina219.getPower_mW();
    //ina219.getShuntVoltage_mV();

    delay(1000);
}
