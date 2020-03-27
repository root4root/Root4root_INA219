/*
   Root4root_INA219 - library for Texas Instruments INA219 - Bidirectional Current/Power Monitor
   Project page: https://github.com/root4root/Root4root_INA219
*/

/*
MIT License

Copyright (c) 2020 root4root

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef _LIB_ROOT4ROOT_INA219_
#define _LIB_ROOT4ROOT_INA219_

#include "Arduino.h"
#include <Wire.h>

#define DEBUG 0;

#define DEFAULT_MAX_EXPECTED_CURRENT 2000 //mA
#define DEFAULT_SHUNT_RESISTOR 100        //mOhm

/** default I2C address **/
#define INA219_ADDRESS (0x40) // 0100 0000 (A0 + A1 = GND) - DEFAULT

/** reset bit **/
#define INA219_CONFIG_RESET 0x8000 // 1000 0000 0000 0000


/** bus voltage range values **/
enum {
    INA219_CONFIG_BUS_VOLTAGE_RANGE_16V = (0x0000), // 0-16V Range
    INA219_CONFIG_BUS_VOLTAGE_RANGE_32V = (0x2000), // 0-32V Range - DEFAULT
};

/** values for gain bits **/
enum {
    INA219_CONFIG_PGA_GAIN_1_40MV =  (0x0000),  // Gain 1, 40mV Range
    INA219_CONFIG_PGA_GAIN_2_80MV =  (0x0800),  // Gain 2, 80mV Range
    INA219_CONFIG_PGA_GAIN_4_160MV = (0x1000), // Gain 4, 160mV Range
    INA219_CONFIG_PGA_GAIN_8_320MV = (0x1800), // Gain 8, 320mV Range  - DEFAULT
};

/** values for bus ADC resolution **/
enum {
    // 9-bit bus res = 0..511
    INA219_CONFIG_BUS_ADC_RESESOLUTION_9BIT =  (0x0000), // 0000 0000 0000 0000
    // 10-bit bus res = 0..1023
    INA219_CONFIG_BUS_ADC_RESESOLUTION_10BIT = (0x0080), // 0000 0000 1000 0000
    //11-bit bus res = 0..2047
    INA219_CONFIG_BUS_ADC_RESESOLUTION_11BIT = (0x0100), // 0000 0001 0000 0000
    //12-bit bus res = 0..4095
    INA219_CONFIG_BUS_ADC_RESESOLUTION_12BIT = (0x0180), // 0000 0001 1000 0000 - DEFAULT
};

/** values for shunt ADC resolution **/
enum {
    // 1 x 9-bit shunt sample
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_9BIT_1S_84US   =   (0x0000), // 0000 0000 0000 0000
    // 1 x 10-bit shunt sample
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_10BIT_1S_148US =   (0x0008), // 0000 0000 0000 1000
    // 1 x 11-bit shunt sample
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_11BIT_1S_276US =   (0x0010), // 0000 0000 0001 0000
    // 1 x 12-bit shunt sample - DEFAULT
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_1S_532US =   (0x0018), // 0000 0000 0001 1000
    // 2 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_2S_1060US =  (0x0048), // 0000 0000 0100 1000
    // 4 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_4S_2130US =  (0x0050), // 0000 0000 0101 0000
    // 8 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_8S_4260US =  (0x0058), // 0000 0000 0101 1000
    // 16 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_16S_8510US = (0x0060), // 0000 0000 0110 0000
    // 32 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_32S_17MS =   (0x0068), // 0000 0000 0110 1000
    // 64 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_64S_34MS =   (0x0070), // 0000 0000 0111 0000
    // 128 x 12-bit shunt samples averaged together
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_12BIT_128S_69MS =  (0x0078), // 0000 0000 0111 1000
};

/** values for operating mode **/
enum {
    INA219_CONFIG_MODE_POWERDOWN,                       // 0000
    INA219_CONFIG_MODE_SHUNT_VOLTAGE_TRIGGERED,         // 0001
    INA219_CONFIG_MODE_BUS_VOLTAGE_TRIGGERED,           // 0010
    INA219_CONFIG_MODE_SHUNT_AND_BUS_VOLTAGE_TRIGGERED, // 0011
    INA219_CONFIG_MODE_ADC_OFF,                         // 0100
    INA219_CONFIG_MODE_SHUNT_VOLTAGE_CONTINUOUS,        // 0101
    INA219_CONFIG_MODE_BUS_VOLTAGE_CONTINUOUS,          // 0110
    INA219_CONFIG_MODE_SHUNT_AND_BUS_VOLTAGE_CONTINUOUS // 0111
};

//Config masks
enum {
    INA219_CONFIG_BUS_VOLTAGE_RANGE_MASK  =   (0x2000), //0010 0000 0000 0000
    INA219_CONFIG_PGA_GAIN_MASK       =       (0x1800), //0001 1000 0000 0000
    INA219_CONFIG_BUS_ADC_RESESOLUTION_MASK = (0x0780), //0000 0111 1000 0000
    INA219_CONFIG_SHUNT_ADC_RESOLUTION_MASK = (0x0078), //0000 0000 0111 1000
    INA219_CONFIG_MODE_MASK         =         (0x0007)  //0000 0000 0000 0111
};


//Registers
enum {
    INA219_CONFIG_REGISTER     =    (0x00),
    INA219_SHUNT_VOLTAGE_REGISTER = (0x01),
    INA219_BUS_VOLTAGE_REGISTER =   (0x02),
    INA219_POWER_REGISTER      =    (0x03),
    INA219_CURRENT_REGISTER   =     (0x04),
    INA219_CALIBRATION_REGISTER  =  (0x05)
};


class Root4root_INA219 {
public:
    Root4root_INA219(uint8_t addr = INA219_ADDRESS, TwoWire *theWire = &Wire);
    void begin(uint16_t expected = DEFAULT_MAX_EXPECTED_CURRENT, uint8_t rshunt = DEFAULT_SHUNT_RESISTOR);

    void setCalibration(uint16_t expected, uint8_t rshunt);

    void changeConfig(uint16_t value, uint16_t mask);

    uint16_t getBusVoltage_mV();
    float getBusVoltage_V();

    float getShuntVoltage_mV();
    int16_t getShuntVoltage_raw();

    float getCurrent_mA();
    int16_t getCurrent_raw();

    float getPower_mW();
    int16_t getPower_raw();

    void powerSave(bool on);

    void reset();

    void trigger();

    /* Mostly used for testing purposes */
    void readRegister(uint8_t reg, uint16_t *value);
    void writeRegister(uint8_t reg, uint16_t value);

private:
    TwoWire * i2c = NULL;
    uint8_t ina219_i2caddr = 0;
    uint16_t ina219_config = (0x399F); //0011 1001 1001 1111 - DEFAULT CONFIG

    uint16_t ina219_calibrationValue = (0x1A36); // 0001 1010 0011 0110 - DEFAULT FOR INA219 Adafruit modules.
    float ina219_currentLSB = 0.0610352;
    float ina219_powerLSB = 1.220704;

    bool checkConfig();
};

#endif //_LIB_ROOT4ROOT_INA219_
