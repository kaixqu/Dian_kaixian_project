#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart1.h"

//CAN��ʼ����ʹ�ö�ʱ���жϷ���can����ʹ��can�Դ��ж�
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:1~3; CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
//tbs1:ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//ע�����ϲ����κ�һ����������Ϊ0,�������.
//������=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,��ͨģʽ;1,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Normal_Init(1,8,7,5,1);
//������Ϊ:36M/((1+8+7)*5)=450Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;

//CAN1 ����CAN ������ȡ0x300 ����0x322 0x329 	500kb������
//CAN2 �����������չ�ѧ��������Ϣ 1M������ ����0x1ffffffa  &&  0x1ffffffb

uint8_t InputInfo;  	//�̵���
uint8_t CAN1_Config(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode)//500kb
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef  CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��PORTAʱ��	 ////                  											 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	////

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;							/////
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��IO
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;							//////
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��IO				//////
	// CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM = DISABLE;		//��ʱ�䴥��ͨ��ģʽ  
	CAN_InitStructure.CAN_ABOM = DISABLE;		//����Զ����߹���	 
	CAN_InitStructure.CAN_AWUM = DISABLE;		//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART = ENABLE;		//��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM = DISABLE;		//���Ĳ�����,�µĸ��Ǿɵ�  
	CAN_InitStructure.CAN_TXFP = DISABLE;		//���ȼ��ɱ��ı�ʶ������ 
	CAN_InitStructure.CAN_Mode = mode;	    //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; //
	// ���ò�����
	CAN_InitStructure.CAN_SJW = tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = tbs1; 			//Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = tbs2;				//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp;  //��Ƶϵ��(Fdiv)Ϊbrp+1	
	CAN_Init(CAN1, &CAN_InitStructure);   // ��ʼ��CAN1    /////
// -------------- CAN filter ���� -------------------//
	CAN_FilterInitStructure.CAN_FilterNumber = 0;	  //������0
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; //16λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x6000;//
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//������0������FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //���������0
	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
#if CAN_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);												//FIFO0��Ϣ�Һ��ж�����.
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // �����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   

uint8_t CAN2_Config(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp,uint8_t mode)//1Mb
{ 
	GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef  CAN_InitStructure;
 	CAN_FilterInitTypeDef CAN_FilterInitStructure;
#if CAN_RX2_INT_ENABLE 
  NVIC_InitTypeDef  NVIC_InitStructure;
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PORTAʱ��	                   											 
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//ʹ��CAN1ʱ��	
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);		//��ʼ��IO
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��IO
	// -------------- CAN ���� -------------------//
 	CAN_InitStructure.CAN_TTCM = DISABLE;						 //��ʱ�䴥��ͨ��ģʽ  //
 	CAN_InitStructure.CAN_ABOM = DISABLE;						 //����Զ����߹���	 //
	CAN_InitStructure.CAN_AWUM = DISABLE;						 //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)//
	CAN_InitStructure.CAN_NART = ENABLE;						 	//��ֹ�����Զ����� //
	CAN_InitStructure.CAN_RFLM = DISABLE;						 //���Ĳ�����,�µĸ��Ǿɵ� // 
	CAN_InitStructure.CAN_TXFP = DISABLE;						 //���ȼ��ɱ��ı�ʶ������ //
	CAN_InitStructure.CAN_Mode = mode;	         //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; //
	CAN_InitStructure.CAN_SJW = tsjw;				//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
	CAN_InitStructure.CAN_BS1 = tbs1; //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
	CAN_InitStructure.CAN_BS2 = tbs2;//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
	CAN_InitStructure.CAN_Prescaler = brp;            //��Ƶϵ��(Fdiv)Ϊbrp+1	//
	CAN_Init(CAN2, &CAN_InitStructure);            // ��ʼ��CAN1 
  // -------------- CAN filter ���� -------------------//
	CAN_FilterInitStructure.CAN_FilterNumber=14;	  //������0
 	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0
 	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
#if CAN2_RX0_INT_ENABLE
	CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN2_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // �����ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // �����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}

uint8_t CAN_Send_Msg(uint8_t* TxM,uint8_t len, uint16_t id)	//����CAN
{	
  uint8_t mbox;
  uint16_t i = 0;
  CanTxMsg TxMessage;
  TxMessage.StdId = id;		 // ��׼��ʶ��Ϊid
  TxMessage.IDE = 0;			 // ʹ����չ��ʶ��
  TxMessage.RTR = 0;		   // ��Ϣ����Ϊ����֡��һ֡8λ
  TxMessage.DLC = len;							 // ����len�ֽڵ���Ϣ
  for (i=0; i<len; i++)
		TxMessage.Data[i]=TxM[i];				 // ��һ֡��Ϣ          
  mbox = CAN_Transmit(CAN1, &TxMessage);  
  i = 0;
  while((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i<0XFFF))
		i++;															//�ȴ����ͽ���
  if(i >= 0XFFF)
		return 1;
  return 0;		
}

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//����,���յ����ݳ���;
uint8_t CAN1_Receive_Msg(void)							//��������0x300
{		   		   
	CanRxMsg RxMessage;
  if(CAN_MessagePending(CAN1,CAN_FIFO0) == 0)
		return 0;																			//û�н��յ�����,ֱ���˳�
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);				//��ȡ����
	if(RxMessage.StdId == 0x300)										//���ؿ���֡	
		InputInfo = RxMessage.Data[2];         
	return RxMessage.DLC;	
}

uint8_t CAN2_Receive_Msg(uint8_t* buf)									//���չ�ѧ������ SPD����
{		   		
	CanRxMsg RxMessage;
	if (CAN_MessagePending(CAN2,CAN_FIFO0) == 0)
		return 0;																						//û�н��յ�����,ֱ���˳� 
	CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);							//��ȡ����	
	if(RxMessage.ExtId == 0x1ffffffb){
		buf[0] = RxMessage.Data[4];
		buf[1] = RxMessage.Data[5];
	}
	else if(RxMessage.ExtId == 0x1ffffffa){
		buf[2] = RxMessage.Data[2];
		buf[3] = RxMessage.Data[3];
	}
	return RxMessage.DLC;	
}
