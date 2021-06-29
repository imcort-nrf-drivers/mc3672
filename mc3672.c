/**
 * @file    mc3672.c
 * @author  Simpreative
 * @date    April 2021
 * @brief   Driver interface header file for accelerometer mc3672 series.
 * @see     https://github.com/imcort-nrf-drivers
 */

#include "mc3672.h"

#define MC3672_I2C_ADDR        (0x4c)

//Default settings. This settings will be used in _begin();
#define MC3672_CFG_SAMPLE_RATE_CWAKE_DEFAULT    MC36XX_CWAKE_SR_400Hz
#define MC3672_CFG_RANGE_DEFAULT                MC36XX_RANGE_8G
#define MC3672_CFG_RESOLUTION_DEFAULT           MC36XX_RESOLUTION_14BIT

//Internal settings. Not accessibe out of this .c
/******************************************************************************
 *** Register Map
 *****************************************************************************/
#define MC3672_REG_EXT_STAT_1       (0x00)
#define MC3672_REG_EXT_STAT_2       (0x01)
#define MC3672_REG_XOUT_LSB         (0x02)
#define MC3672_REG_XOUT_MSB         (0x03)
#define MC3672_REG_YOUT_LSB         (0x04)
#define MC3672_REG_YOUT_MSB         (0x05)
#define MC3672_REG_ZOUT_LSB         (0x06)
#define MC3672_REG_ZOUT_MSB         (0x07)
#define MC3672_REG_STATUS_1         (0x08)
#define MC3672_REG_STATUS_2         (0x09)
#define MC3672_REG_FEATURE_CTL      (0x0D)
#define MC3672_REG_PWR_CONTROL      (0X0F)
#define MC3672_REG_MODE_C           (0x10)
#define MC3672_REG_WAKE_C           (0x11)
#define MC3672_REG_SNIFF_C          (0x12)
#define MC3672_REG_SNIFFTH_C        (0x13)
#define MC3672_REG_SNIFF_CONF_C     (0x14)
#define MC3672_REG_RANGE_C          (0x15)
#define MC3672_REG_FIFO_C           (0x16)
#define MC3672_REG_INTR_C           (0x17)
#define MC3672_REG_PROD             (0x18)
#define MC3672_REG_PMCR             (0x1C)
#define MC3672_REG_DMX              (0x20)
#define MC3672_REG_DMY              (0x21)
//#define MC3672_REG_GAIN             (0x21)
#define MC3672_REG_DMZ              (0x22)
#define MC3672_REG_RESET            (0x24)
#define MC3672_REG_XOFFL            (0x2A)
#define MC3672_REG_XOFFH            (0x2B)
#define MC3672_REG_YOFFL            (0x2C)
#define MC3672_REG_YOFFH            (0x2D)
#define MC3672_REG_ZOFFL            (0x2E)
#define MC3672_REG_ZOFFH            (0x2F)
#define MC3672_REG_XGAIN            (0x30)
#define MC3672_REG_YGAIN            (0x31)
#define MC3672_REG_ZGAIN            (0x32)
#define MC3672_REG_OPT              (0x3B)
#define MC3672_REG_LOC_X            (0x3C)
#define MC3672_REG_LOC_Y            (0x3D)
#define MC3672_REG_LOT_dAOFSZ       (0x3E)
#define MC3672_REG_WAF_LOT          (0x3F)

#define MC3672_NULL_ADDR            (0)

static bool mc3672_is_initialized = false;

// Read 8-bit from register
static uint8_t mc3672_readRegister8(uint8_t reg)
{
    uint8_t value;
	
    iic_send(MC3672_I2C_ADDR, &reg, 1, true);
    iic_read(MC3672_I2C_ADDR, &value, 1);
    return value;
}

// Repeated Read Byte(s) from register
static void mc3672_readRegisters(uint8_t reg, uint8_t *buffer, uint8_t len)
{
    iic_send(MC3672_I2C_ADDR, &reg, 1, true);
    iic_read(MC3672_I2C_ADDR, buffer, len);
}

// Write 8-bit to register
static void mc3672_writeRegister8(uint8_t reg, uint8_t value)
{
    uint8_t sendbuf[2] = {reg, value};
    iic_send(MC3672_I2C_ADDR, sendbuf, 2, false);
}

//Not common used functions. Prepare for begin.
//Initial reset
void mc3672_reset()
{
    mc3672_writeRegister8(0x10, 0x01);  //Go to standby.

    delay(2);

    mc3672_writeRegister8(0x24, 0x40);  //Power on reset.

    delay(2);

    mc3672_writeRegister8(0x09, 0x00);
    delay(10);
    mc3672_writeRegister8(0x0F, 0x42);
    delay(10);
    mc3672_writeRegister8(0x20, 0x01);
    delay(10);
    mc3672_writeRegister8(0x21, 0x80);
    delay(10);
    mc3672_writeRegister8(0x28, 0x00);
    delay(10);
    mc3672_writeRegister8(0x1a, 0x00);

    delay(50);

    uint8_t _bRegIO_C = 0;

    _bRegIO_C = mc3672_readRegister8(0x0D);

    //MC36XX_CFG_BUS_I2C
    _bRegIO_C &= 0x3F;
    _bRegIO_C |= 0x40;

    mc3672_writeRegister8(0x0D, _bRegIO_C);

    delay(50);

    mc3672_writeRegister8(0x10, 0x01);

    delay(10);
}

void mc3672_setMode(uint8_t mode)
{
    uint8_t value;
	uint8_t cfgfifovdd = 0x42;
	
    value = mc3672_readRegister8(MC3672_REG_MODE_C);
    value &= 0xf0;
    value |= mode;
	
	mc3672_writeRegister8(MC3672_REG_PWR_CONTROL, cfgfifovdd);
    mc3672_writeRegister8(MC3672_REG_MODE_C, value);
}

void mc3672_setRange(MC36XX_range_t range)
{
    uint8_t value;
    mc3672_setMode(MC36XX_MODE_STANDBY);
    value = mc3672_readRegister8(MC3672_REG_RANGE_C);
    value &= 0x07;
    value |= (range << 4)&0x70 ;
    mc3672_writeRegister8(MC3672_REG_RANGE_C, value);
}

void mc3672_setResolution(MC36XX_resolution_t resolution)
{
    uint8_t value;
    mc3672_setMode(MC36XX_MODE_STANDBY);
    value = mc3672_readRegister8(MC3672_REG_RANGE_C);
    value &= 0x70;
    value |= resolution;
    mc3672_writeRegister8(MC3672_REG_RANGE_C, value);
}

void mc3672_setSampleRate(MC36XX_cwake_sr_t sample_rate)
{
    uint8_t value;
    mc3672_setMode(MC36XX_MODE_STANDBY);
    value = mc3672_readRegister8(MC3672_REG_WAKE_C);
    value &= 0x00;
    value |= sample_rate;
    mc3672_writeRegister8(MC3672_REG_WAKE_C, value);
}

// Begin
bool mc3672_begin(void)
{
    if(!mc3672_is_initialized)
    {
        //Communication init.
        iic_init();
        
        //Sensor reset.
        mc3672_reset();
        mc3672_setMode(MC36XX_MODE_STANDBY);

        /* Check I2C connection */
        uint8_t id = mc3672_readRegister8(MC3672_REG_PROD); //Not shown in datasheet.
        if (id != 0x71)
        {
            /* No MC36XX detected ... return false */
            Debug("No MC36XX detected!");
            // Serial.println(id, HEX);
            return false;
        }
        
        //Range: 8g
        mc3672_setRange(MC3672_CFG_RANGE_DEFAULT);
        //Resolution: 14bit
        mc3672_setResolution(MC3672_CFG_RESOLUTION_DEFAULT);
        //Sampling Rate: 50Hz by default
        mc3672_setSampleRate(MC3672_CFG_SAMPLE_RATE_CWAKE_DEFAULT);
        
        mc3672_is_initialized = true;
    
    }

    //Mode: Active
    mc3672_setMode(MC36XX_MODE_CWAKE);
    
    return true;

}

void mc3672_sleep(void)
{
    //Set mode as Sleep
    mc3672_setMode(MC36XX_MODE_STANDBY);
}

int16_t mc3672_readXAccel(void)
{
	uint8_t rawData[2];
    // Read the six raw data registers into data array
    mc3672_readRegisters(MC3672_REG_XOUT_LSB, rawData, 2);
    return (short)((((unsigned short)rawData[1]) << 8) | rawData[0]);

}

int16_t mc3672_readYAccel(void)
{
	uint8_t rawData[2];
    // Read the six raw data registers into data array
    mc3672_readRegisters(MC3672_REG_YOUT_LSB, rawData, 2);
    return (short)((((unsigned short)rawData[1]) << 8) | rawData[0]);

}

int16_t mc3672_readZAccel(void)
{
	uint8_t rawData[2];
    // Read the six raw data registers into data array
    mc3672_readRegisters(MC3672_REG_ZOUT_LSB, rawData, 2);
    return (short)((((unsigned short)rawData[1]) << 8) | rawData[0]);

}
