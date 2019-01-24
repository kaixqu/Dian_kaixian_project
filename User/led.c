#include "led.h"

//³õÊ¼»¯PA1µ½PA5ÎªÊä³ö¿Ú.²¢Ê¹ÄÜÊ±ÖÓ		    
//Êä³öÄ£¿é IO³õÊ¼»¯
void LED_Config(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //Ê¹ÄÜPAºÍPC¶Ë¿ÚÊ±ÖÓ
	// GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2 ¼ÌµçÆ÷ GPIO_Pin_8|GPIO_Pin_9 LED
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_8|GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //ÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO¿ÚËÙ¶ÈÎª50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 //¸ù¾ÝÉè¶¨²ÎÊý³õÊ¼»¯GPIOB.5ß 
}

void Relay_Init(void)
{
	GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
	GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
}

