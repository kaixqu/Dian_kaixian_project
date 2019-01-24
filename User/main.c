#include "led.h"
#include "usart1.h"
#include "can.h"
#include "timer.h"
#include "adc.h"

#define offset 120

extern uint8_t TxM323[6];		//cc_rear_sensor_b
extern uint8_t TxM324[6];		//cc_rear_sensor_c
extern u8 InputInfo;

u8 TxM329[8];		//cc_rear_status					
u8 TxM322[8];		//cc_rear_sensor_a
uint8_t Rx[8];
uint8_t key;     //���յ�����������ĳ���
uint8_t key2;
// uint8_t RxM[8];			 //������������
u16 adcx0,adcx_save0;//����ת��ֵ�����adcx0,adcx1[0]�洢PA0	DPRL
u16 adcx1,adcx_save1;//����ת��ֵ�����adcx0,adcx1[0]�洢PA1	DPRR

int main(void)
{
	uint8_t	ReadValueC0;
	uint8_t	ReadValueC2;
	NVIC_Configuration();
	USART_Config();
	LED_Config();		  					//��ʼ����LED���ӵ�Ӳ���ӿ�PD8-PD13 PC0~PC3
	Relay_Init();							//�̵����ϵ�ʱ�Ͽ�
	Adc_Init1();		  				//ADC��ʼ��
	Adc_Init2();		  				//ADC��ʼ��
	CAN1_Config(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);		//CAN��ʼ����ͨģʽ,������500Kbps 
	CAN2_Config(CAN_SJW_1tq, CAN_BS2_4tq, CAN_BS1_4tq, 4, CAN_Mode_Normal);		//������ͨѶЭ�鲨����Ϊ1M
	TIM3_Int_Init(99,7199);	//��ʱ���жϷ���can
	while(1) {
		adcx0 = Get_Adc_Average1(ADC_Channel_0, 10);			//��ȡPA0��ѹ DPRL
		adcx_save0 = adcx0;
		adcx1 = Get_Adc_Average1(ADC_Channel_1, 10);			//��ȡPA1��ѹ DPRR
		adcx_save1 = adcx1;
		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_0))
			ReadValueC0 = 0;
		else
			ReadValueC0 = 1;
		if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_2))
			ReadValueC2 = 0;
		else
			ReadValueC2 = 1;
		if(key2) {
			 TxM322[3] = Rx[0];
			 TxM322[4] = Rx[1];
			 TxM322[5] = Rx[2];
			 TxM322[6] = Rx[3];
		}
		TxM322[0] = (adcx_save0)&0x00FF;
		TxM322[1] =0;
		TxM322[1] = (adcx_save0>>8)&0x000F;
 	  TxM322[1] |= (adcx_save1&0x000F)<<4;
		TxM322[2] = (adcx_save1>>4&0x00FF);
		 
		TxM329[0] = ReadValueC0;											//�洢PC0��PC2�������
		TxM329[0] = TxM329[0]+(ReadValueC2<<1);
		
		if(key)//���յ����ؿ�����Ϣ
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_9);
			if(InputInfo&0x01)//ȡ�����ܴ���������
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				GPIO_ResetBits(GPIOC,GPIO_Pin_1);
			}
			else
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				GPIO_SetBits(GPIOC,GPIO_Pin_1);
			}
			if(InputInfo&0x04)//ȡ����������
				GPIO_ResetBits(GPIOC,GPIO_Pin_2);
			else
				GPIO_SetBits(GPIOC,GPIO_Pin_2);
		}
	}
}
