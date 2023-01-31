#include "QN8027.h"


void QN8027::_writeRegister(uint8_t addr, uint8_t data)
{	
    twi_write_byte(QN8027_ADDRESS, addr, data);
}

uint8_t QN8027::_readRegister(uint8_t addr)
{
    return twi_read_byte(QN8027_ADDRESS, addr);
}

void QN8027::_updateRegister(uint8_t addr, uint8_t data, uint8_t mask)
{
	uint8_t dataOld = _readRegister(addr);
	uint8_t dataNew = (dataOld & ~mask) | (data & mask);
	_writeRegister(addr, dataNew);
}

void QN8027::begin(float fmFreq)
{
	uint16_t freqChannel = ((uint16_t)(FREQ_FLOAT2CHANNEL(fmFreq)));	
	reset();
	_setFrequency(freqChannel);
}

void QN8027::setTxPower(uint8_t pwr) // 20 - 75
{
	uint8_t data;
	if (pwr < 20) data = 20;
	else if (pwr > 75) data = 75;
	else data = pwr;	
	_writeRegister(QN8027_REG_PAC, data);
}

void QN8027::enable()
{
	_updateRegister(QN8027_REG_SYSTEM, (1 << QN8027_BIT_IDLE), QN8027_MASK_IDLE);
}

void QN8027::_setFrequency(uint16_t freqChannel)
{
	_writeRegister(QN8027_REG_CH1, (uint8_t)freqChannel);
	_updateRegister(QN8027_REG_SYSTEM, (uint8_t)(freqChannel >> 8), QN8027_MASK_CH_HI);
}

void QN8027::setFrequency(float freq)
{
	uint16_t freqChannel = ((uint16_t)(FREQ_FLOAT2CHANNEL(freq)));
	if (freqChannel > 640) freqChannel = 640;
	_setFrequency(freqChannel);
}

void QN8027::reset()
{
	_writeRegister(QN8027_REG_SYSTEM, 1 << QN8027_BIT_RESET);
	_writeRegister(QN8027_REG_XTL, (((uint8_t)CLOCK_XTAL) << QN8027_BIT_XINJ) | 0x0f);
	_writeRegister(QN8027_REG_VGA, (((uint8_t)CLOCKSPEED_12) << QN8027_BIT_XSEL) | (((uint8_t)GAIN_0) << QN8027_BIT_GVGA) | (((uint8_t)TXGAIN_2) << QN8027_BIT_GDB) | (((uint8_t)RIN_1) << QN8027_BIT_RIN));
	_writeRegister(QN8027_REG_SYSTEM, 1 << QN8027_BIT_RECAL);
	_writeRegister(QN8027_REG_RDS, 10);
	_writeRegister(QN8027_REG_SYSTEM, 0x00);
	_writeRegister(QN8027_REG_GPLT, 0x39);
}



