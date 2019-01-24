#ifndef __USART1_H
#define __USART1_H

#include "stm32f10x.h"

void USART_Config(void);
void USART_SB(USART_TypeDef* _USART, uint8_t ch);
void myorg(void);

#endif
