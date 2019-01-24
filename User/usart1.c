#include "usart1.h"
#include "stm32f10x_it.h"

__IO uint8_t mytmp = 0x11;
int tmp3[3] = {1, 2, 3};
int able_to_org = 1;
int index_of_pack = -1, length_of_pack = 0;
int count = 0;

uint8_t TxM323[6];		//cc_rear_sensor_b
uint8_t TxM324[6];		//cc_rear_sensor_c		

uint8_t rbuffer[100] = {0};
uint8_t tbuffer[100] = {0};
uint16_t ACC_X = 0X01, ACC_Y = 0X01, ACC_Z = 0X03;
uint8_t ACC_XH = 0X44, ACC_YH = 0X44, ACC_ZH = 0X44;
uint8_t ACC_XL = 0X33, ACC_YL = 0X33, ACC_ZL = 0X33;
uint16_t AV_X, AV_Y, AV_Z;
uint8_t AV_XH = 0X22, AV_YH = 0X22, AV_ZH = 0X22;
uint8_t AV_XL = 0X11, AV_YL = 0X11, AV_ZL = 0X11;

//extern void myorg(void);
// function which receive data from sensor via usart
// and write it to rbuffer 
// when the full packet is received, and we are not sending data to CAN, write it to tbuffer
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		// write to tmp3
		mytmp = USART_ReceiveData(USART1);
		tmp3[0] = tmp3[1];
		tmp3[1] = tmp3[2];
		tmp3[2] = mytmp;
		// we need to find head
		if (index_of_pack == -1){ 
			// we find the head
			if (tmp3[0] == 0X0D && tmp3[1] == 0X0A && tmp3[2] == 0X3A){
				GPIO_SetBits(GPIOC, GPIO_Pin_9);
				// index -> 0
				index_of_pack ++; 
				rbuffer[index_of_pack] = mytmp; //tmp
			}
		}
		// we have the head
		else {
			// we find the end
			if (tmp3[1] == 0X0D && tmp3[2] == 0X0A) {
				index_of_pack ++;
				rbuffer[index_of_pack] = mytmp; //tmp;
				length_of_pack = index_of_pack + 1;
				// tbuffer -> rbuffer
				if (able_to_org){
					myorg();
					USART_SB(USART1, ACC_ZH);
					USART_SB(USART1, ACC_ZL); 
					TxM323[0] = ACC_XL;
					TxM323[1] = ACC_XH;
					TxM323[2] = ACC_YL;
					TxM323[3] = ACC_YH;
					TxM323[4] = ACC_ZL;
					TxM323[5] = ACC_ZH;
					TxM324[0] = AV_XL;
					TxM324[1] = AV_XH;
					TxM324[2] = AV_YL;
					TxM324[3] = AV_YH;
					TxM324[4] = AV_ZL;
					TxM324[5] = AV_ZH;
				}
				// wait to find the next head
				index_of_pack = -1;
				length_of_pack = 0;
			}
			else {
				index_of_pack ++;
				rbuffer[index_of_pack] = mytmp; //tmp;
			}
		}
	}
}

// organize all the useful data
void myorg(void)
{
	float *_ACC_X, *_ACC_Y, *_ACC_Z, *_AV_X, *_AV_Y, *_AV_Z;
	able_to_org = 0;
	// TRANSLATE TO VECTOR3F
	_AV_X = (float *)&rbuffer[11]; 
	_AV_Y = (float *)&rbuffer[15]; 	// tbuffer[15] + (tbuffer[16] << 8) + (tbuffer[17] << 16) + (tbuffer[18] << 24);
	_AV_Z = (float *)&rbuffer[19];		// tbuffer[19] + (tbuffer[20] << 8) + (tbuffer[21] << 16) + (tbuffer[22] << 24);
	_ACC_X = (float *)&rbuffer[23]; 		// tbuffer[23] + (tbuffer[24] << 8) + (tbuffer[25] << 16) + (tbuffer[26] << 24);
	_ACC_Y = (float *)&rbuffer[27]; 		// tbuffer[27] + (tbuffer[28] << 8) + (tbuffer[29] << 16) + (tbuffer[30] << 24);
	_ACC_Z = (float *)&rbuffer[31];		// tbuffer[31] + (tbuffer[32] << 8) + (tbuffer[33] << 16) + (tbuffer[34] << 24);
	// TRANSLATE TO uint16_t
	ACC_X = (*_ACC_X + 4) / 4 * 32768;
	ACC_Y = (*_ACC_Y + 4) / 4 * 32768;
	ACC_Z = (*_ACC_Z + 4) / 4 * 32768;
	ACC_XH = ACC_X >> 8 & 0XFF; ACC_XL = ACC_X & 0XFF;
	ACC_YH = ACC_Y >> 8 & 0XFF; ACC_YL = ACC_Y & 0XFF;
	ACC_ZH = ACC_Z >> 8 & 0XFF; ACC_ZL = ACC_Z & 0XFF;
	AV_X = (*_AV_X + 245) / 245 * 32768;
	AV_Y = (*_AV_Y + 245) / 245 * 32768;
	AV_Z = (*_AV_Z + 245) / 245 * 32768;
	AV_XH = AV_X >> 8 & 0XFF; AV_XL = AV_X & 0XFF;
	AV_YH = AV_Y >> 8 & 0XFF; AV_YL = AV_Y & 0XFF;
	AV_ZH = AV_Z >> 8 & 0XFF; AV_ZL = AV_Z & 0XFF;
	able_to_org = 1;
}

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_DeInit(USART1);
	/****************** USART1 ***************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// TX - PA9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// RX - PA10	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/****************** USART1 ***************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = \
											USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx \
																	  | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

void USART_SB(USART_TypeDef* _USART, uint8_t ch)
{
	USART_SendData(_USART, ch);
	while (USART_GetFlagStatus(_USART, USART_FLAG_TXE) == RESET)
		;
}
