#include "sr04.h"
#include "syslog.h"
#include <usart.h>

void SR04_Trig_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��

  //GPIOF9,F10��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
}

void SR04_Echo_Init(u32 arr, u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTAʱ��	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //PA0����λ��ʱ��5
  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);

	//��ʼ��TIM5���벶�����
    TIM_ICInitTypeDef  TIM5_ICInitStructure;
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
    TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
    TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
    TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
    TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ���������˲��� ���˲�
    TIM_ICInit(TIM5, &TIM5_ICInitStructure);
		
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC1IE�����ж�	
	
    TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5
 
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(����32λ��ʱ����˵,1us��������1,���ʱ��:4294��)
u8  TIM5CH1_CAPTURE_STA=0;	//���벶��״̬
u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ(TIM2/TIM5��32λ)
//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 		    

 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����	
	{
		if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//���
		{	     
			if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//�ߵ�ƽ̫����
				{
					TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;
			}	 
		}
		if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)//����1���������¼�
		{	
			if(TIM5CH1_CAPTURE_STA&0X40)		//����һ���½��� 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//��ǳɹ�����һ�θߵ�ƽ����
			  TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);//��ȡ��ǰ�Ĳ���ֵ.
	 			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //CC1P=0 ����Ϊ�����ز���
			}else  								//��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;			//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//��ǲ�����������
				TIM_Cmd(TIM5,DISABLE ); 	//�رն�ʱ��5
	 			TIM_SetCounter(TIM5,0);
	 			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
				TIM_Cmd(TIM5,ENABLE ); 	//ʹ�ܶ�ʱ��5
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
}

void vSR04_Init()
{
    SR04_Echo_Init(0XFFFFFFFF,84-1);    //��1Mhz��Ƶ�ʼ���
    SR04_Trig_Init();
}

void UltrasonicWave_StartMeasure(void)
{
	__TRIG_HIGH();
	vTaskDelay(20);                     //��ʱ20US
	__TRIG_LOW();
}

TimerHandle_t xTimers = NULL;
void vSR04TimerCallback(TimerHandle_t xTimer)
{
    u32 time = 0; 
    float UltrasonicWave_Distance=0;
    UltrasonicWave_StartMeasure();//��ʼ���
    if(TIM5CH1_CAPTURE_STA&0X80)//�ɹ�������һ��������
    {
        time=TIM5CH1_CAPTURE_STA&0X3F;
        time*=65536;//���ʱ���ܺ�
        time+=TIM5CH1_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��

        UltrasonicWave_Distance=(float)time*170/10000;//������㹫ʽ������=�ߵ�ƽ����ʱ��*����/2   
                                                      //�� ����= �ߵ�ƽ����ʱ��*340*100/��2*1000 000�� = �ߵ�ƽ����ʱ��*170/10000
        if (UltrasonicWave_Distance < 10.0f && UltrasonicWave_Distance > 1.0f)
        {
            ucUsart3_PutChar('s', 100);
        }
        ts_printf("Distance:%f cm\r\n",UltrasonicWave_Distance);//��ӡ�ܵĸߵ�ƽʱ��
        TIM5CH1_CAPTURE_STA=0;//������һ�β���
    }
}

void sr04_timer(unsigned char start)
{
    if (start)
    {
        if (!xTimers)
            xTimers = xTimerCreate("sr04", pdMS_TO_TICKS(300), pdTRUE, (void *)0, vSR04TimerCallback);
        if (xTimers)
            xTimerStart(xTimers, 0);
    }
    else
    {
        if (xTimers)
        {
            xTimerStop(xTimers, 0);
            xTimerDelete(xTimers,100);
            xTimers = NULL;
        }
    }
}

void vSR04_Task(void *pvParameters)
{
    u32 time=0; 
    float UltrasonicWave_Distance=0;
    
    while(1) 
    {
        UltrasonicWave_StartMeasure();//��ʼ���
        if(TIM5CH1_CAPTURE_STA&0X80)//�ɹ�������һ��������
        {
            time=TIM5CH1_CAPTURE_STA&0X3F;
            time*=65536;//���ʱ���ܺ�
            time+=TIM5CH1_CAPTURE_VAL;//�õ��ܵĸߵ�ƽʱ��

            UltrasonicWave_Distance=(float)time*170/10000;//������㹫ʽ������=�ߵ�ƽ����ʱ��*����/2   
                                                          //�� ����= �ߵ�ƽ����ʱ��*340*100/��2*1000 000�� = �ߵ�ƽ����ʱ��*170/10000
            if (UltrasonicWave_Distance < 10.0f && UltrasonicWave_Distance > 1.0f)
            {
                ucUsart3_PutChar('s', 100);
            }
            ts_printf("Distance:%f cm\r\n",UltrasonicWave_Distance);//��ӡ�ܵĸߵ�ƽʱ��
            TIM5CH1_CAPTURE_STA=0;//������һ�β���
        }
        vTaskDelay(500);
    }
}
