#include "twi.h"

void start();
void send(uint8_t data);
void stop();
uint8_t read();
uint8_t read_last_byte();
void wait();


void twi_init(uint8_t speed)
{
    TWBR = speed;
}

void send_byte_by_ADDR(uint8_t data, uint8_t addr)
{
    start();
    
    send(addr << 1);
    
    send(data);
    
    stop();
}

uint8_t twi_read_byte(uint8_t address, uint8_t reg_addr)
{
    
    start();
    
    send(address << 1);
    
    send(reg_addr);
    
    start();
    
    send((address << 1) + 1);
    
    uint8_t data = read_last_byte();
    
    stop();
    
    return data;
}

void twi_read_array(uint8_t address, uint8_t reg_addr, uint8_t *array, uint8_t num)
{
    start();
    
    send(address << 1);
    
    send(reg_addr);
    
    start();
    
    send((address << 1) + 1);
    
    for (uint8_t i = 0; i < num - 1; i++)
    {
        array[i] = read();
    }
    
    array[num - 1] = read_last_byte();
    
    stop();
}

void twi_write_byte(uint8_t address, uint8_t reg_addr, uint8_t data)
{
    start();
    
    send(address << 1);
    
    send(reg_addr);
    
    send(data);
    
    stop();
}

void twi_write_array(uint8_t address, uint8_t reg_addr, uint8_t* array, uint8_t num)
{
    start();
    
    send(address << 1);
    
    send(reg_addr);
    
    for (uint8_t i = 0; i < num; i++)
    {
        send(array[i]);
    }
    
    stop();
}

void start()
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    wait();
}

void send(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    wait();
}

void stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t read()
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    wait();
    return TWDR;
}

uint8_t read_last_byte()
{
    TWCR = (1 << TWINT) | (1 << TWEN);
    wait();
    return TWDR;
}

void wait()
{
    while(!(TWCR & (1 << TWINT)));
}
