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


#include "Root4root_INA219.h"

/**
 * A constructor.
 *
 * @param addr the I2C address of a device. Default is 0x40
 * @param theWire the TwoWire interface pointer
 */
Root4root_INA219::Root4root_INA219(uint8_t addr, TwoWire *theWire)
{
    this->ina219_i2caddr = addr;
    this->i2c = theWire;
}

/**
 * Initialize I2C, set expected max current and shunt resistance
 *
 * @param expected max current in milliAmps. Default is 2000.
 * @param rshunt the shutn resistance in milliOhms. Default is 100.
 *
 * @return void
 */
void Root4root_INA219::begin(uint16_t expected, uint8_t rshunt)
{
    this->i2c->begin();
    setCalibration(expected, rshunt);
}

/**
 * Calculate calibration value, current LSB abd power LSB
 * See datasheet for more details.
 *
 * @param expected max current in milliAmps.
 * @param rshunt the shutn resistance in milliOhms.
 *
 * @return void
 */
void Root4root_INA219::setCalibration(uint16_t expected, uint8_t rshunt)
{
    this->ina219_currentLSB = (float)expected/32768.0;
    this->ina219_calibrationValue = (uint32_t)(40960.0/(this->ina219_currentLSB * rshunt));
    this->ina219_powerLSB = 20 * this->ina219_currentLSB;

    writeRegister(INA219_CALIBRATION_REGISTER, this->ina219_calibrationValue);
}

/**
 * Changes configuration register in binary format.
 *
 * @param value 16 bit of config value
 * @param mask for isolating unchanged bits.
 *
 * @return void
 */
void Root4root_INA219::changeConfig(uint16_t value, uint16_t mask)
{
    this->ina219_config &= ~mask;
    this->ina219_config |= value;

    writeRegister(INA219_CONFIG_REGISTER, this->ina219_config);
}

/**
 * Get bus voltage.
 *
 * @return 16 bit value in milliVolts.
 */
uint16_t Root4root_INA219::getBusVoltage_mV()
{
    uint16_t value;
    readRegister(INA219_BUS_VOLTAGE_REGISTER, &value);

    // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
    return (uint16_t)((value >> 3) * 4);
}

/**
 * Get bus voltage in Volts
 *
 * @return float value in Volts
 */
float Root4root_INA219::getBusVoltage_V()
{
    int16_t value = getBusVoltage_mV();
    return value * 0.001; // x/1000
}

/**
 * Get shunt voltage in milliVolts
 *
 * @return float value in mV
 */
float Root4root_INA219::getShuntVoltage_mV()
{
    int16_t value;
    value = getShuntVoltage_raw();
    //LSB 10uV, x = value*10/1000 = value/100mV or value * 0.01
    return value * 0.01;
}


/**
 * Get content of shunt register. (raw, not in Volts)
 *
 * @return 16 bit shunt voltage register value
 */
int16_t Root4root_INA219::getShuntVoltage_raw()
{
    uint16_t value;
    readRegister(INA219_SHUNT_VOLTAGE_REGISTER, &value);
    return (int16_t)value;
}


/**
 * Get current value in milliApms
 *
 * @return float mAmp
 */
float Root4root_INA219::getCurrent_mA()
{
    float valueDec = getCurrent_raw();
    valueDec *= this->ina219_currentLSB;
    return valueDec;
}

/**
 * Get current register value (raw, not in Amps).
 * If value is 0x00, initiates calibration register checking/updating.
 * And call itself again recursively.
 * @see checkConfig()
 *
 * @return 16 bit register value (raw, not in Amps)
 */
int16_t Root4root_INA219::getCurrent_raw()
{
    uint16_t value;

    readRegister(INA219_CURRENT_REGISTER, &value);

    if (!value) {
        if(!checkConfig()) {
            delay(10);
            return getCurrent_raw();
        }
    }

    return (int16_t)value;
}


/**
 * Get power value in milliWatts
 *
 * @return float power value
 */
float Root4root_INA219::getPower_mW()
{
    float valueDec = getPower_raw();
    valueDec *= this->ina219_powerLSB;
    return valueDec;
}

/**
 * Get power register value (raw, not in Watts)
 * If value is 0x00 initiates calibration register checking/updating.
 * And call itself again recursively.
 * @see checkConfig()
 *
 * @return 16 bit power register value
 */
int16_t Root4root_INA219::getPower_raw()
{
    uint16_t value;

    readRegister(INA219_POWER_REGISTER, &value);

    if (!value) {
        if(!checkConfig()) {
            delay(10);
            return getPower_raw();
        }
    }

    return (int16_t)value;
}

/**
 * Enable or disable power save mode
 *
 * @param on boolean true - enable, false - disable
 *
 * @return void
 */
void Root4root_INA219::powerSave(bool on)
{
    if (on) {
        uint16_t save = this->ina219_config & ~INA219_CONFIG_MODE_MASK;
        writeRegister(INA219_CONFIG_REGISTER, save);
    } else {
        writeRegister(INA219_CONFIG_REGISTER, this->ina219_config);
    }
}

/**
 * Resets INA219 IC.
 *
 * @return void
 */
void Root4root_INA219::reset()
{
    writeRegister(INA219_CONFIG_REGISTER, INA219_CONFIG_RESET);
    delay(10);
}

/**
 * Write register value to IC
 *
 * @param reg 8 bit register address
 * @param value 16 bit register value
 *
 * @return void
 */
void Root4root_INA219::writeRegister(uint8_t reg, uint16_t value)
{
    this->i2c->beginTransmission(ina219_i2caddr);
    this->i2c->write(reg);                     // Register
    this->i2c->write((uint8_t)(value >> 8));   // Upper 8-bit
    this->i2c->write((uint8_t)(value & 0xFF)); // Lower 8-bit
    this->i2c->endTransmission();
}

/**
 * Reads register value from IC
 *
 * @param reg 8 bit register address
 * @param value pointer to 16 bit variable to store read value
 *
 * @return void
 */
void Root4root_INA219::readRegister(uint8_t reg, uint16_t *value)
{
    this->i2c->beginTransmission(ina219_i2caddr);
    this->i2c->write(reg); // Register
    this->i2c->endTransmission();

    delay(5); // Max 12-bit conversion time is 586us per sample

    this->i2c->requestFrom(ina219_i2caddr, (uint8_t)2);

    *value = ((i2c->read() << 8) | i2c->read());
}


/**
 * Check calibration registers for correct value
 * in case unwanted resets, which may be occour switching on/off a sharp load.
 * If so, calibration and configuration registers will be written up again.
 *
 * @return void
 */
bool Root4root_INA219::checkConfig()
{
    uint16_t calibrationRegister = 0;

    readRegister(INA219_CALIBRATION_REGISTER, &calibrationRegister);

    if (!calibrationRegister) {
        writeRegister(INA219_CONFIG_REGISTER, this->ina219_config);
        writeRegister(INA219_CALIBRATION_REGISTER, this->ina219_calibrationValue);
        return false;
    }

    return true;
}

/**
 * Measure in "triggered" mode.
 * According to datasheet, we have to write configuration register to do so every time.
 *
 * @return void
 */
void Root4root_INA219::trigger()
{
    writeRegister(INA219_CONFIG_REGISTER, this->ina219_config);
}
