# Root4root_INA219

##### Library for Texas Instruments INA219 IC - Bidirectional Current/Power Monitor
---

### Quick Example:
```cpp
#include <Arduino.h>
#include "Root4root_INA219.h"

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

```

### Advanced Usage:
```cpp
#include <Arduino.h>
#include "Root4root_INA219.h"

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

```

### Available Methods:
```cpp
//Constructor
Root4root_INA219(uint8_t addr = INA219_ADDRESS, TwoWire *theWire = &Wire);

//DEFAULT_MAX_EXPECTED_CURRENT 2000 mA
//DEFAULT_SHUNT_RESISTOR 100 mOhm
void begin(uint16_t expected = DEFAULT_MAX_EXPECTED_CURRENT, uint8_t rshunt = DEFAULT_SHUNT_RESISTOR);

void setCalibration(uint16_t expected, uint8_t rshunt);

void changeConfig(uint16_t value, uint16_t mask); //See header file to findout config and masks constants.

uint16_t getBusVoltage_mV();
float getBusVoltage_V();

float getShuntVoltage_mV();
int16_t getShuntVoltage_raw(); //raw, shunt voltage register value

float getCurrent_mA();
int16_t getCurrent_raw(); //raw, current register value (not in Amps)

float getPower_mW();
int16_t getPower_raw();  //raw, power register value (not in Watts)

void powerSave(bool on);

void reset();

void trigger(); //For trigged measure mode

//!! Following methods are mainly used for testing purpouses:
void readRegister(uint8_t reg, uint16_t *value);
void writeRegister(uint8_t reg, uint16_t value);
```

### License:
MIT
### Author:
Paul aka root4root \<root4root at gmail dot com><br/>
**Any comments/suggestions are welcomed.**
