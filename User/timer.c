#include "timer.h"
#include "can.h"
#include "delay.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ

extern u8 TxM345[8];
extern u8 TxM329[8];
extern u8 TxM322[8];
extern u8 TxM323[6];
extern u8 TxM324[6];
extern u8 key;
extern u8 key2;
extern u8 Rx[8];
// extern u8 RxM[8];			 //������������ 2017/8/2 unvalid

void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	//----------- NVIC Config -----------//
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx					 
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	static uint8_t flag = 0X00;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){  //���TIM3�����жϷ������
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־ 
		key2 = CAN2_Receive_Msg(Rx);					
		key = CAN1_Receive_Msg();
		if (flag == 0X00){
			flag = 0X01;
			CAN_Send_Msg(TxM323,6, 0X323);
			CAN_Send_Msg(TxM324,6, 0X324);
		}
		else {
			flag = 0X00;
			CAN_Send_Msg(TxM322,7, 0X322);
			CAN_Send_Msg(TxM329,1, 0X329);
		}
	}
}











