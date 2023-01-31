#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "QN8027.h"
#include "twi.h"
#include "iarduino_OLED_txt.h"
#include "Rotary.h"


QN8027 fm;
Rotary rotary;
iarduino_OLED_txt myOLED(0x3C);

float frequency = eeprom_read_float(0);
uint8_t power = eeprom_read_byte((uint8_t *)4);
int8_t vol = eeprom_read_byte((uint8_t *)5);
int8_t vol_scr;
uint8_t counter = 0;

void trans(int8_t num)
{
    if(num == 3) vol_scr = 1;
    if(num == 2) vol_scr = 2;
    if(num == 1) vol_scr = 3;
    if(num == 0) vol_scr = 4;
}

void RF_init()
{
    fm.begin(frequency);
    fm.setTxPower(power);
    fm.enable();
    fm._updateRegister(0x04, vol, 1);
}

ISR(TIMER1_OVF_vect)
{
    TIMSK1 &= ~(1 << TOIE1); 
    eeprom_update_float(0, frequency);
    if (power != eeprom_read_byte((uint8_t *)4))
    {
        eeprom_update_byte((uint8_t *)4, power);
        RF_init();
    }
    
    eeprom_update_byte((uint8_t *)5, vol);
}

ISR(TIMER0_OVF_vect)
{
    static uint8_t count = 0;
    count++;
    if (count == 15)
    {
        counter++;
        if (counter > 3) counter = 0;
        count = 0;
        
        switch (counter)
        {
            case 0:
            myOLED.print(frequency, 54, 3);
            myOLED.print(" MHz");
            if (frequency < 100) myOLED.print(" ");
            myOLED.print(((power - 20)/5) + 1, 60, 5);
            if (((power - 20)/5 + 1) < 10) myOLED.print(" ");
            trans(vol);
            myOLED.print(vol_scr, 66, 7);
            break;
            
            case 1:
            myOLED.invText(1);
            myOLED.print(frequency, 54, 3);
            myOLED.invText(0);
            myOLED.print(" MHz");
            if (frequency < 100) myOLED.print(" ");
            break;
            
            case 2:
            myOLED.print(frequency, 54, 3);
            myOLED.print(" MHz");
            if (frequency < 100) myOLED.print(" ");
            myOLED.invText(1);
            myOLED.print(((power - 20)/5) + 1, 60, 5);
            myOLED.invText(0);
            break;
            
            case 3:
            myOLED.print(((power - 20)/5) + 1, 60, 5);
            if (((power - 20)/5 + 1) < 10) myOLED.print(" ");
            trans(vol);
            myOLED.invText(1);
            myOLED.print(vol_scr, 66, 7);
            myOLED.invText(0);
            break;
        }
    
        TIMSK0 &= ~(1 << TOIE0);
    }
    
    TCNT0 = 0;
}

ISR(PCINT0_vect)
{
    TIMSK0 = 1 << TOIE0;
    TCNT0 = 0;    
}

ISR(PCINT2_vect) {
    uint8_t result = rotary.process();
    if (result == DIR_CCW) {
        switch(counter)
        {
            case 1:
            frequency+=0.1;
            if (frequency > 108) frequency = 108.0;
            fm.setFrequency(frequency);
            myOLED.invText(1);
            myOLED.print(frequency, 54, 3);
            myOLED.invText(0);
            myOLED.print(" MHz");
            if (frequency < 100) myOLED.print(" ");
            break;
            
            case 2:
            power+=5;
            if (power > 75) power = 75;
            fm.setTxPower(power);
            myOLED.invText(1);
            myOLED.print(((power - 20)/5) + 1, 60, 5); 
            myOLED.invText(0);
            if ((((power - 20)/5) + 1) < 10) myOLED.print(" ");
            break;
            
            case 3:
            vol-=1;
            if(vol < 0) vol = 0;
            fm._updateRegister(0x04, vol, 1);
            trans(vol);
            myOLED.invText(1);
            myOLED.print(vol_scr, 66, 7);
            myOLED.invText(0);
            break;
        }        
    }
    else if (result == DIR_CW) {
        switch(counter)
        {
            case 1:
            frequency-=0.1;
            if (frequency < 88) frequency = 88.0;
            fm.setFrequency(frequency);
            myOLED.invText(1);
            myOLED.print(frequency, 54, 3);
            myOLED.invText(0);
            myOLED.print(" MHz");
            if (frequency < 100) myOLED.print(" ");
            break;
            
            case 2:
            power-=5;
            if (power < 20) power = 20;
            fm.setTxPower(power);
            myOLED.invText(1);
            myOLED.print(((power - 20)/5) + 1, 60, 5);
            myOLED.invText(0);
            if (((power - 20)/5) < 10) myOLED.print(" ");
            break;
            
            case 3:
            vol+=1;
            if(vol > 3) vol = 3;
            fm._updateRegister(0x04, vol, 1);
            trans(vol);
            myOLED.invText(1);
            myOLED.print(vol_scr, 66, 7);
            myOLED.invText(0);
            break;
        }    
    }
    
    TIMSK1 = 1 << TOIE1;
    TCNT1 = 0;
}

int main(void)
{ 
    PORTC |= 1 << PORTC6;
    //установка предделителя-------
    TCCR1B = 1 << CS12 | 1 << CS10;
    TCCR0B = 1 << CS02 | 1 << CS00;   
    //-----------------------------
    PCICR |= (1 << PCIE2) | (1 << PCIE0);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    PCMSK0 |= (1 << PCINT0);
    //-----------------------------
    
    twi_init(12);
    sei();
    
    trans(vol);

    extern uint8_t SmallFontRus[];
    
    myOLED.begin();
    myOLED.setFont(SmallFontRus);
    myOLED.print("FM ПЕРЕДАТЧИК", OLED_C, 0);
    myOLED.print("Рабочая частота:", OLED_C, 2);
    myOLED.print("88 - 108 MHz", OLED_C, 3);
    myOLED.print("Версия прошивки:", OLED_C, 5);
    myOLED.print("1.0", OLED_C, 6);
    RF_init();
    _delay_ms(3000);
    myOLED.clrScr();
    myOLED.print("FM ПЕРЕДАТЧИК", OLED_C, 0);
    myOLED.print("Частота: ", 0, 3);
    myOLED.print(frequency);
    myOLED.print(" MHz");
    myOLED.print("Мощность: ", 0, 5);
    myOLED.print((power - 20)/5 + 1);
    myOLED.print("Громкость: ", 0, 7);
    myOLED.print(vol_scr);
    
    while (1) 
    {        
    }
}

