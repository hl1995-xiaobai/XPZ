#include ".\BF7615BMXX_XXXX_Library\Config.h"

#if (TRACEMODE == TRUE)
void TraceMode_Master_Read_Data(unsigned int xdata *p_rawData,unsigned int xdata *p_baseline,unsigned long  t_keysFlagSN,unsigned long  t_keysFlagSN_H);   
unsigned char xdata ReadCnt = 0;
unsigned char xdata ReceData = 0;
unsigned char xdata DataNum = 0;
bit OddCheck = 0,NeedCheckOdd = 0;
unsigned int xdata raw = 0;
unsigned int xdata bas = 0;

#if (TRACEMODE==1)
	#pragma message "����,TRACEMODE����"
	unsigned long xdata keys_flag_l_Tracemode = 0;	
	unsigned long xdata keys_flag_h_Tracemode = 0;	
	//-----------------------------------------------------------------//
	//�������ƣ� void Trace_Mode_IIC_ISR_PA() interrupt 10 
	//�������ܣ� PA��IIC�ж��Ӻ���
	//��������� ��
	//��������� ��
	//�� �� ֵ�� ��
	//-----------------------------------------------------------------//

	void Trace_Mode_IIC_ISR_PA() interrupt 10
	{
			uchar tmp;
			static uchar xdata addr_bak = 0;
			PUSH_REG_ADDR_SFR();
			EA = 0;
			IRCON1 &= ~0x08;
			if ((IICSTAT & 0x02))//д��ͻ��־λ
			{ 
					IICSTAT &= ~0x02;
			}
			if ((IICSTAT & 0x01))//����ͻ��־λ  
			{
					
					IICSTAT &= ~0x01;
					tmp = IICBUF;
			}
			if ((IICSTAT & 0x10) == 0)//�ǵ�ַ
			{	 
			
					if((IICSTAT & 0x20))//������ 
					{
							
							ReadCnt = 0;
							OddCheck = 0;
							DataNum = 0;
							
							TraceMode_Master_Read_Data(&raw_data[0],&base_line[0],keys_flag_l_Tracemode,keys_flag_h_Tracemode);

							IICCON |= 0x04; 
					}
					else //����д
					{
							tmp = IICBUF;
					}   
				}
				else 
				{
					if ((IICSTAT & 0x20))//������ 
					{
							       
							TraceMode_Master_Read_Data(&raw_data[0],&base_line[0],keys_flag_l_Tracemode,keys_flag_h_Tracemode);
							IICCON |= 0x04;	  
					}
					else//����д  
					{   
				
						if ((IICSTAT & 0x08)) 
						{ 
							TraceMode_Master_Write_Data();
						}
					}
					
			}
			IICCON |= 0x04;
			POP_REG_ADDR_SFR();
			EA = 1;
	}

	#endif

//-----------------------------------------------------------------//
//�������ƣ� void BF7615BMXX_XXXX_Trace_Mode_Init(unsigned char IICSlaveAddr,unsigned char iic_port)
//�������ܣ� BF7615BMXX_XXXX_Trace_Mode_Init��ʼ��
//��������� unsigned char IICSlaveAddr:IIC�ӻ���ַ;,unsigned char iic_port://IIC����ӳ��ѡ��0-Ϊѡ��PC4/5Ϊ���ڣ�1ѡ��PE4/5Ϊ����
//��������� ��
//�� �� ֵ�� ��
//-----------------------------------------------------------------//

void BF7615BMXX_XXXX_Trace_Mode_Init(unsigned char IICSlaveAddr,unsigned char iic_port)
{
	
		EA = 0;		//�����ж�
		IPL1 |= 0x08;			//����IIC�ж����ȼ�
		IRCON1 &= ~0x08;	// ���IIC�жϱ�־λ
	
		REG_ADDR = 0x50;REG_DATA = 0x03;//bit1:IICģ���˲�ѡ��1Ϊѡ��ģ���˲���0Ϊ��ѡ��ģ���˲�;bit0:IICģ���˲�ѡ��1Ϊѡ�������˲���0Ϊ��ѡ�������˲�(Sleepģʽʱ��IIC���ܻ���MCU)
	
		if(iic_port == 1)
		{

				REG_ADDR = 0x1B;REG_DATA |= 0x10;//PE4��������
				REG_ADDR = 0x1B;REG_DATA |= 0x20;//PE5��������
				

				TRISE |= 0x10;//PE4����Ϊ����
				TRISE |= 0x20;//PE5����Ϊ����
			
				{REG_ADDR = 0x34;REG_DATA &= ~(0x10);REG_DATA |= (0x10&(iic_port<<4));}
		}
		else
		{
	
				REG_ADDR = 0x19;REG_DATA |= 0x10;//PC4��������
				REG_ADDR = 0x19;REG_DATA |= 0x20;//PC5��������
				TRISC |= 0x10;//PC4����Ϊ����
				TRISC |= 0x20;//PC5����Ϊ����
			
				{REG_ADDR = 0x34;REG_DATA &= ~(0x10);REG_DATA |= (0x10&(iic_port<<4));}
		}
		IICADD 		= IICSlaveAddr;		//����IIC�ӻ���ַ��
		IICSTAT 	= 0x00;						//���IIC״̬�Ĵ�����
		IICCON	  |= 0x01;					//IIC����ʹ��
		IICCON 		|= (0x02);				//IICת��������Ӧ100K
		IICCON 		|= 0x04;					//SLCʱ����������
		IICCON 		&= (~0x08);				//��ʹ��д����,����IIC������дʱ������
		IICCON 		|= (0x10);				//ʹ�ܶ����ͣ�����IIC������дʱ��
		IEN1 			|= 0x08;					//��IIC�ж�ʹ��
		EA = 1; 										//�����ж�			
}
//-----------------------------------------------------------------//
//�������ƣ� void TraceMode_Master_Read_Data(uint *p_rawData,uint *p_baseline,ulong  t_keysFlagSN,unsigned long  t_keysFlagSN_H)   
//�������ܣ� �����͵�����д��IICBUF
//��������� ��
//��������� ��
//�� �� ֵ�� ��
//-----------------------------------------------------------------//
#pragma disable
void TraceMode_Master_Read_Data(unsigned int xdata *p_rawData,unsigned int xdata *p_baseline,unsigned long  t_keysFlagSN,unsigned long  t_keysFlagSN_H)     
{   
	unsigned char data tmp = 0;

	if(ReceData == 0xb2)
	{
		if(ReadCnt == 0)
		{
			tmp = 0xae;
		}
		else if(ReadCnt == 1)
		{
			tmp = CH_MAX;
			
		}			
	}
	else if(ReceData == 0xb3)
	{
		
		if(ReadCnt < 1)			//����ͷ��0XAE
		{
			tmp = 0xae;	
		
		}
		else if(ReadCnt < ((CH_MAX<<1) + 1))	//����RAWDATA
		{
			NeedCheckOdd = 1;
			if(ReadCnt % 2)	
			{
				
				raw = *(p_rawData + DataNum);
				
				
				tmp = (raw >> 8);
			}
			else
			{
				tmp = raw;
				DataNum ++;	
			}
		}
		else if(ReadCnt == ((CH_MAX << 1) + 1))  //����BASELINE[0]��8λ
		{
			NeedCheckOdd = 1;
			DataNum = 0;
			
			bas = *(p_baseline + DataNum);
			
			tmp = (bas >> 8);
			
		}
		else if(ReadCnt < ((CH_MAX << 2) + 1))		//����BASELINE
		{
			
			NeedCheckOdd = 1;
			
			if(ReadCnt % 2)	
			{
				bas = *(p_baseline + DataNum);
				
				tmp = (bas >> 8);
			}
			else
			{
				tmp = bas; 
				DataNum++;	
			}
			
		} 
		else if(ReadCnt == ((CH_MAX << 2) + 1))		//������żУ��λ
		{
			tmp = OddCheck;
			
		}
		else if(ReadCnt == ((CH_MAX << 2) + 2))		//������Ч���ݳ���
		{						
			tmp = (CH_MAX << 2);
		}		
		
		else if(ReadCnt == ((CH_MAX << 2) + 3))		//������־λbit31:17
		{ 						
			tmp = (unsigned char )(t_keysFlagSN >> 24);
		}
		else if(ReadCnt == ((CH_MAX << 2) + 4))		//������־λbit23:16
		{						 
			tmp = (unsigned char )(t_keysFlagSN >> 16);		
		}
		else if(ReadCnt == ((CH_MAX << 2) + 5))		//������־λbit15:8
		{						
			tmp = (unsigned char )(t_keysFlagSN >> 8);
		}					
		else if(ReadCnt == ((CH_MAX << 2) + 6))//������־λbit7:0
		{						
			tmp = (unsigned char )t_keysFlagSN;	
		}
		
		else if(ReadCnt == ((CH_MAX << 2) + 7))		//������־λbit63:56
		{ 						
			tmp = (unsigned char )(t_keysFlagSN_H >> 24);
		}
		else if(ReadCnt == ((CH_MAX << 2) + 8))		//������־λbit55:48
		{						 
			tmp = (unsigned char )(t_keysFlagSN_H >> 16);		
		}
		else if(ReadCnt == ((CH_MAX << 2) + 9))		//������־λbit47:40
		{						
			tmp = (unsigned char )(t_keysFlagSN_H >> 8);
		}					
		else if(ReadCnt == ((CH_MAX << 2) + 10))//������־λbit39:32
		{						
			tmp = (unsigned char )t_keysFlagSN_H;	
		}
	
		else //���ͽ�����0xff��0xff
		{			
			ReadCnt = 0;
			tmp = 0xFF;	
			
		}
	}
	
	IICBUF = tmp;//��IICBUF = tmp;���ڡ�else if(ReceData == 0xb3)���ж���
	
	if(NeedCheckOdd)		//��ǰ�����Ƿ���Ҫ������żУ��
	{
		NeedCheckOdd = 0;
		
		ACC = tmp;
		if((PSW & 0x01))
			OddCheck = !OddCheck;
	}
	ReadCnt++;
	
}

		
//-----------------------------------------------------------------//
//�������ƣ� void TraceMode_Master_Write_Data(void)  
//�������ܣ� ��IICBUF��ȡ����
//��������� ��
//��������� ��
//�� �� ֵ�� ��
//-----------------------------------------------------------------//
#pragma disable
void TraceMode_Master_Write_Data(void) 
{   
	ReceData = IICBUF;
}
#endif