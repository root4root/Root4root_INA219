#include <Arduino.h>
#include "Root4root_INA219.h"
//Project page: https://github.com/root4root/Root4root_INA219

Root4root_INA219 ina219(0x40); //0x40 address

/**
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * Do NOT use readRegister() and writeRegister() directly.
 * These methods used only for testing purposes.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

void setup() {
    Serial.begin(9600);
    Wire.begin();
}

void loop() {

    uint16_t response = 0;

    ina219.reset();

    Serial.println("--- Checking INA219 without any configuration ---");

    ina219.readRegister(INA219_CONFIG_REGISTER, &response);

    if (response != 0b0011100110011111) {
        Serial.print("Configuration register. Expected: 399F, got: ");
        Serial.println(response, HEX);
    }

    //--
    ina219.readRegister(INA219_CALIBRATION_REGISTER, &response);

    if (response != 0b0) {
        Serial.print("Calibration register. Expected: 0, got: ");
        Serial.println(response, HEX);
    }

    //--
    ina219.readRegister(INA219_CURRENT_REGISTER, &response);

    if (response != 0b0) {
        Serial.print("Current register. Expected: 0, got: ");
        Serial.println(response, HEX);
    }

    //--
    ina219.readRegister(INA219_POWER_REGISTER, &response);

    if (response != 0b0) {
       Serial.print("Power register. Expected: 0, got: ");
       Serial.println(response, HEX);
    }

    //-- Checking for autosetup calibration and configuration registers (after reset the module)
    ina219.getCurrent_mA();
    ina219.readRegister(INA219_CALIBRATION_REGISTER, &response);

    if (response != 0b0001101000110110) {
        Serial.print("Auto setup calibration register. Expected: 1A36, got: ");
        Serial.println(response, HEX);
    }

    //--
    Serial.println("--- Begin command with 100mA 100mOhm params ---");

    ina219.begin(100, 100);

    delay(100);

    ina219.readRegister(INA219_CALIBRATION_REGISTER, &response);

    if (response != 0b0000110001001000) {
        Serial.print("Calibration register. Expected: C48, got: ");
        Serial.println(response, HEX);
    }

    //--
    Serial.println("--- Change config to 16V 12bit 2s shunt ADC  ---");

    ina219.changeConfig(INA219_CONFIG_BUS_VOLTAGE_RANGE_16V |
                        INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_2S_1060US,
                        INA219_CONFIG_BUS_VOLTAGE_RANGE_MASK |
                        INA219_CONFIG_SHUNT_ADC_RESOLUTION_MASK);

    delay(100);

    ina219.readRegister(INA219_CONFIG_REGISTER, &response);

    if (response != 0b0001100111001111) {
        Serial.print("Configuration register. Expected: 19CF, got: ");
        Serial.println(response, HEX);
    }

    Serial.println("--- Reset IC ---");
    ina219.reset();

    delay(100);

    ina219.readRegister(INA219_CONFIG_REGISTER, &response);

    if (response != 0b0011100110011111) {
         Serial.print("Configuration register. Expected: 399F, got: ");
         Serial.println(response, HEX);
    }

    //--
    ina219.getCurrent_mA();

    ina219.readRegister(INA219_CALIBRATION_REGISTER, &response);

    if (response != 0b0000110001001000) {
        Serial.print("Calibration register. Expected: C48, got: ");
        Serial.println(response, HEX);
    }

    //--
    ina219.readRegister(INA219_CONFIG_REGISTER, &response);

    if (response != 0b0001100111001111) {
       Serial.print("Auto setup config register to value before reset. Expected: 19CF, got: ");
       Serial.println(response, HEX);
    }

    //--
    ina219.powerSave(true);

    ina219.readRegister(INA219_CONFIG_REGISTER, &response);

    if (response != 0b0001100111001000) {
           Serial.print("Set power save mode and read register. Expected: 19C8, got: ");
           Serial.println(response, HEX);
    }


    //--

    Serial.println(ina219.getBusVoltage_mV());
    delay(1000);
    Serial.println(ina219.getBusVoltage_mV());
    delay(1000);

    ina219.powerSave(false);

    Serial.println(ina219.getBusVoltage_mV());
    delay(1000);
    Serial.println(ina219.getBusVoltage_mV());

    ina219.readRegister(INA219_CONFIG_REGISTER, &response);

    if (response != 0b0001100111001111) {
           Serial.print("Disable power save and read register. Expected: 19CF, got: ");
           Serial.println(response, HEX);
    }

    Serial.println("!!! Test complete !!!");
    while(1);
}
