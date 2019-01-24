#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    

//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	0		//0,��ʹ��;1,ʹ��.	

uint8_t CAN1_Config(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode);
uint8_t CAN2_Config(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode);
uint8_t CAN_Send_Msg(uint8_t* TxM,uint8_t len, uint16_t id);
uint8_t CAN1_Receive_Msg(void);
uint8_t CAN2_Receive_Msg(uint8_t* buf);

#endif

















