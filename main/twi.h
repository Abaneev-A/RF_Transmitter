#ifndef TWI_H_
#define TWI_H_
#include <avr/io.h>

void twi_init(uint8_t speed);

void send_byte_by_ADDR(uint8_t data, uint8_t addr);

uint8_t twi_read_byte(uint8_t address, uint8_t reg_addr);
void twi_read_array(uint8_t address, uint8_t reg_addr, uint8_t *array, uint8_t num);

void twi_write_byte(uint8_t address, uint8_t reg_addr, uint8_t data);
void twi_write_array(uint8_t address, uint8_t reg_addr, uint8_t* array, uint8_t num);

#endif