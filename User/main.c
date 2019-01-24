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
uint8_t key;     //接收到的数据数组的长度
uint8_t key2;
// uint8_t RxM[8];			 //接收数据数组
u16 adcx0,adcx_save0;//采样转换值存放在adcx0,adcx1[0]存储PA0	DPRL
u16 adcx1,adcx_save1;//采样转换值存放在adcx0,adcx1[0]存储PA1	DPRR

int main(void)
{
	uint8_t	ReadValueC0;
	uint8_t	ReadValueC2;
	NVIC_Configuration();
	USART_Config();
	LED_Config();		  					//初始化与LED连接的硬件接口PD8-PD13 PC0~PC3
	Relay_Init();							//继电器上电时断开
	Adc_Init1();		  				//ADC初始化
	Adc_Init2();		  				//ADC初始化
	CAN1_Config(CAN_SJW_1tq, CAN_BS2_8tq, CAN_BS1_9tq, 4, CAN_Mode_Normal);		//CAN初始化普通模式,波特率500Kbps 
	CAN2_Config(CAN_SJW_1tq, CAN_BS2_4tq, CAN_BS1_4tq, 4, CAN_Mode_Normal);		//传感器通讯协议波特率为1M
	TIM3_Int_Init(99,7199);	//定时器中断发送can
	while(1) {
		adcx0 = Get_Adc_Average1(ADC_Channel_0, 10);			//读取PA0电压 DPRL
		adcx_save0 = adcx0;
		adcx1 = Get_Adc_Average1(ADC_Channel_1, 10);			//读取PA1电压 DPRR
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
		 
		TxM329[0] = ReadValueC0;											//存储PC0与PC2开闭情况
		TxM329[0] = TxM329[0]+(ReadValueC2<<1);
		
		if(key)//接收到主控控制信息
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_9);
			if(InputInfo&0x01)//取后悬架传感器命令
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				GPIO_ResetBits(GPIOC,GPIO_Pin_1);
			}
			else
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				GPIO_SetBits(GPIOC,GPIO_Pin_1);
			}
			if(InputInfo&0x04)//取陀螺仪命令
				GPIO_ResetBits(GPIOC,GPIO_Pin_2);
			else
				GPIO_SetBits(GPIOC,GPIO_Pin_2);
		}
	}
}
