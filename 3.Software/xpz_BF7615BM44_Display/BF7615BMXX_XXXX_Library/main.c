#include "include.h"

#include ".\BF7615BMXX_XXXX_Library\Config.h"




void main(void)
{
	//MCU_Init()初始化函数
	MCU_Init();

	g_View.VersionNumber = HOST_FIRMWARE_VERSION_NUMBER;
		
	Led_init();
	SM16xx_Init();
	CmdInit();

	UART1_BUF = 0x5A;
	
	while(1)
	{
		//清看门狗。
		WDT_CTRL = 7;
		
#if(1)		
		if(Low_Volotage_Flag_Confirm == 0)//进入低电压中断时，定时VolDet_COUNT*(timer0定时)后确认为低电压Low_Volotage_Flag_Confirm = 1
		{
			//获取按键标志位
			BF7615BMXX_XXXX_Set_Touch_Flag();
		}
#endif		
		
		//建议将项目功能函数定义在Project.c中或新建c文件。
		Project();

#if(0)
		if(g_View.PowerOn){
			SET_PB6_H;
		}else{
			SET_PB6_L;
		}
#endif
//		m_LedData.LedCom++;
//		if(m_LedData.LedCom >= 20) m_LedData.LedCom = 0;

		
	}			
}
