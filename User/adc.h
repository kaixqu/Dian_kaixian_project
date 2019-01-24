#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

void Adc_Init1(void);
void Adc_Init2(void);
uint16_t Get_Adc1(uint8_t ch); 
uint16_t Get_Adc_Average1(uint8_t ch,uint8_t times); 
uint16_t Get_Adc2(uint8_t ch); 
uint16_t Get_Adc_Average2(uint8_t ch,uint8_t times); 
 
#endif 
