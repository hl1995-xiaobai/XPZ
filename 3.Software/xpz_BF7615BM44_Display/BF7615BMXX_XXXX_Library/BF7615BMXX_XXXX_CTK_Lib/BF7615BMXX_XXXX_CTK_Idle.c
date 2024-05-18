#include ".\BF7615BMXX_XXXX_Library\Config.h"
#if IDLE_MODE
	uint Idle_Cnt = 0;
	bit Idle_Flag = 0;
	uint xdata Idle_Wait_Count = 0;
	uint xdata Idle_Time_Count = 0;
	uint xdata Idle_Time_Count1 = 0;
	uint xdata Step_In_Idle_Time = 0;
	uint xdata Idle_Wake_Up_Time = 0;
	
	unsigned int  xdata raw_data_parallel[1] =  {0};
	unsigned int  xdata base_line_parallel[1] = {0};	
	unsigned char xdata Cnt_Par[4] = 0;
#endif

#if IDLE_MODE

	#pragma message "����,IDLE����"

	//-----------------------------------------------------------------//
	//�������ƣ� Idle_IORecovrey()
	//�������ܣ� ���Ѻ󣬽�ʹ�õ�Ӧ�õ�IO�����ó���Ҫ��״̬
	//��������� ��
	//��������� ��
	//�� �� ֵ�� ��
	//-----------------------------------------------------------------//
	void Idle_IORecovrey()
	{
	//����Ӧ�û�ԭIO��״̬
	}

	//-----------------------------------------------------------------//
	//�������ƣ� Idle_IO_Init()
	//�������ܣ� Idleģʽ��ʼ��IO,���齫δ�õ���IO������Ϊ���Ϊ��
	//��������� ��
	//��������� ��
	//�� �� ֵ�� ��
	//-----------------------------------------------------------------//
	void Idle_IO_Init()
	{
	//����Ӧ�ó�ʼ��IO	
		
	}

	//-----------------------------------------------------------------//
	//�������ƣ� void Idle_Wake_Up_Init(uint _Step_In_Idle_Time,uint _Idle_Wake_Up_Time)
	//�������ܣ� Idleģʽ��ʼ������
	//��������� uint _Step_In_Idle_TimeΪ�����ް�������_Step_In_Idle_Time��������˯��ģʽ(����>=5000),��ֵ�������һ��keysFlagSN��λ��ʱ��
	//			 uint _Idle_Wake_Up_TimeΪ��ȴ�����ʱ�䣬��ΧΪ0~7��0:18ms,1:36ms,2:72ms,3:144ms,4:288ms,5:576ms,6:1156ms,7:2304ms
	//��������� ��
	//�� �� ֵ�� ��
	//-----------------------------------------------------------------//
	void Idle_Wake_Up_Init(uint _Step_In_Idle_Time,uint _Idle_Wake_Up_Time)
	{
		Idle_IO_Init();
		Step_In_Idle_Time = _Step_In_Idle_Time;//����Time0��Timer0�жϺ�����ʹ�á�
		Idle_Wake_Up_Time = _Idle_Wake_Up_Time;

	}
	//-----------------------------------------------------------------//
	//�������ƣ� void Idle()
	//�������ܣ� ����Idleģʽ��������������ָ���������˯�ߵ�ʱ���Լ�Ϊ����ʱɨ�谴����ʱ��
	//��������� ��
	//��������� ��
	//�� �� ֵ�� ��
	//-----------------------------------------------------------------//
	void Idle()
	{
		long differ_para = 0;

		if(Idle_Flag == 1)
		{
			if(ctk_para_done == 1)//�ж�ɨ����һ��
			{
					CTK_IE_CLR;//��CTK�ж�
					differ_para = ((long)base_line_parallel[0] - (long)raw_data_parallel[0]);
				
					if(differ_para >= 0)
					{
						if(differ_para >= FTH_PARA)
						{
							

								Cnt_Par[0]++;
								if(Cnt_Par[0] >= 0)
								{
										
										Cnt_Par[0]= 0; 
									
										Idle_Flag = 0;
										parallel_mode = 0;
										
									
										differ_para = 0;
										Idle_Time_Count1 = 0;
										Idle_Wait_Count = 0;
											
								}
								Cnt_Par[1] = 0;
								Cnt_Par[2] = 0;
								Cnt_Par[3] = 0;
								
						}
						else if(differ_para >= FTH_PARA/5)
						{
								Cnt_Par[1]++;
								if(Cnt_Par[1] >= PAR_PnoiseRenMaxCnt)
								{
										Cnt_Par[1]= 0; 
										base_line_parallel[0] -= 5;
								}
								Cnt_Par[0] = 0;
								Cnt_Par[2] = 0;
								Cnt_Par[3] = 0;
							
							
						}
						
						else if(differ_para >= 0)
						{
								Cnt_Par[2]++;
								if(Cnt_Par[2] >= PAR_PnoiseRenMaxCnt)
								{
										Cnt_Par[2]= 0; 
										base_line_parallel[0]--;
								}
								Cnt_Par[0] = 0;
								Cnt_Par[1] = 0;
								Cnt_Par[3] = 0;
								
						}
					}
					else
					{

							Cnt_Par[3]++;
							if(Cnt_Par[3] >= PAR_NnoiseRenMaxCnt)
							{
									Cnt_Par[3]= 0; 
									base_line_parallel[0] = raw_data_parallel[0];
							}
							Cnt_Par[0] = 0;
							Cnt_Par[1] = 0;
							Cnt_Par[2] = 0;
					}
					
					#if (TRACEMODE == 0)
						if(Idle_Flag == 1)
						{
					
							EA = 0; //�����ж�
							WDT_IE_SET;	//��WDT�ж�ʹ��
							
							Idle_IO_Init();
							//�ر��Ѵ򿪵����л�����жϵ�ģ�鼰���ж�ʹ�ܲ��������жϱ�־λ�����������ڻ��ѵ�ģ��
							T0_STOP; //�ر� Timer0
							T0_IE_CLR; //�ر� Timer0 �ж�
							T0_INT_FLAG_CLR; //��� Timer0 �жϱ�־
							
							CTK_STOP;
							CTK_IE_CLR;//��CTK�ж�
							CTK_PD_SET(1);//0ΪCTK����1ΪCTK��
							
							LVDT_ON_OFF_SET(1);//0Ϊ����LVDT���ģ��,1Ϊ�ر�LVDT���ģ��
							LVDT_IE_CLR;//�ؽ�ѹ��ѹ���ж�
							LVDT_INT_FLAG_CLR;//�����ѹ��ѹ�жϱ�־λ
							LVDT_INT_DOWN_FLAG_CLR;//�����ѹ�½��жϱ�־λ	
							LVDT_INT_UP_FLAG_CLR;//�����ѹ�����жϱ�־λ	
							
							Delay_Ms(1);//ִ��PCON = 0x01ǰ��ʱ>=100us
							
							WDT_CTRL = Idle_Wake_Up_Time;//���ÿ��Ź����ʱ��
							
							EA = 1; //�����ж�
							
							PCON = 0x01;//����ʡ��ģʽ�ȴ�����
							
							EA = 0; //�����ж�					
							
							//���Ѻ����ʵ��Ӧ��������ָ���Ӧģ�鼰���ж�ʹ��
							T0_IE_SET;//����ʱ��0�ж�,���Ѻ����ʵ��Ӧ�ÿ���ģ�鹦��
							T0_RUN; //���� Timer0
							
							CTK_PD_SET(0);//0ΪCTK����1ΪCTK��
							CTK_IE_SET;//��CTK�ж�
							CTK_START;
							
							LVDT_ON_OFF_SET(0);//0Ϊ����LVDT���ģ��,1Ϊ�ر�LVDT���ģ��
							LVDT_IE_SET;//����ѹ��ѹ���ж�
							
							Idle_IORecovrey();
							
							EA = 1; //�����ж�
						}
					#endif	
					
					ctk_para_done = 0;
					CTK_IE_SET;//��CTK�ж�
			}
			
			#if TRACEMODE
		
					raw_data[25] = raw_data_parallel[0];
					base_line[25] = base_line_parallel[0];//�鿴����ģʽ����ʱ��raw_data[25]��Ӧ��SNS�ر�
					
			#endif
			
		}//IDLE
		else
		{
				#if TRACEMODE
		
					raw_data[25] = raw_data_parallel[0];
					base_line[25] = base_line_parallel[0];//�鿴����ģʽ����ʱ��raw_data[25]��Ӧ��SNS�ر�
					
				#endif
		}
	
	}	
#endif	
		