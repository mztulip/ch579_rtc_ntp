/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_int.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/03/18
 * Description 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : NMI�жϺ���
* Input          : None
* Return         : None
*******************************************************************************/
//__attribute__((section("NMICode")))
void NMI_Handler( void )    
{
    UINT32  i=0;
	
		if( (R8_HFCK_PWR_CTRL & RB_CLK_INT32M_PON) == 0 )
		{
		    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
				R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
				R8_HFCK_PWR_CTRL |= RB_CLK_INT32M_PON;							// ���ڲ�32M
		}
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_CLK_SYS_CFG = 5;		        												// ��Ƶ HSI/5=6.4M
    R16_POWER_PLAN &= ~(RB_PWR_DCDC_EN|RB_PWR_DCDC_PRE);		// ��· DC/DC 
		R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON;											// ��PLL
		R8_SAFE_ACCESS_SIG = 0;
   
    while(1)
    { 
        if(R8_BAT_STATUS & RB_BAT_STAT_LOWER)   i = 0;
        else i++;
        if( i>2000 )
				{
					/* ִ��ϵͳ��λ */
					R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
					R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
					R8_RST_WDOG_CTRL = RB_SOFTWARE_RESET;
					R8_SAFE_ACCESS_SIG = 0;
				}
    }
}

/*******************************************************************************
* Function Name  : LowPower_Halt_1
* Description    : �͹���-Halt_1ģʽ��
                   �˵͹����е�HSI/5ʱ�����У����Ѻ���Ҫ�û��Լ�����ѡ��ϵͳʱ��Դ
* Input          : None
* Return         : None
*******************************************************************************/
//__attribute__((section("NMICode")))
void LowPower_Halt_1( void )
{
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%�����
    if(R16_RTC_CNT_32K>0x3fff){     // ����500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE�����������͵������
    }
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_SLP_POWER_CTRL &= ~RB_SLP_ROM_PWR_DN;		  // flash����
    R8_BAT_DET_CTRL = 0;                              // �رյ�ѹ���
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // ��Ƶ HSI/5=6.4M
    R8_SAFE_ACCESS_SIG = 0;
        
    SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;
 
    /* ������ѹ��� */
    PowerMonitor( ENABLE );    
}

/*******************************************************************************
* Function Name  : LowPower_Halt_2
* Description    : �͹���-Halt_2ģʽ��
                   �˵͹����е�HSI/5ʱ�����У����Ѻ���Ҫ�û��Լ�����ѡ��ϵͳʱ��Դ
* Input          : None
* Return         : None
*******************************************************************************/
//__attribute__((section("NMICode")))
void LowPower_Halt_2( void )
{
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%�����
    if(R16_RTC_CNT_32K>0x3fff){     // ����500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE�����������͵������
    }    
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_SLP_POWER_CTRL |= RB_SLP_ROM_PWR_DN;			  // flashͣ��
    R8_BAT_DET_CTRL = 0;                              // �رյ�ѹ���
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // ��Ƶ HSI/5=6.4M
    R8_SAFE_ACCESS_SIG = 0;

    SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;

    /* ������ѹ��� */
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
		R8_BAT_DET_CFG = 1;                     // 2.05V - 2.33V
		R8_BAT_DET_CTRL = RB_BAT_DET_EN;
		R8_SAFE_ACCESS_SIG = 0; 
		asm("nop");	asm("nop");
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
		R8_BAT_DET_CTRL = RB_BAT_LOW_IE|RB_BAT_LOWER_IE|RB_BAT_DET_EN;  
		R8_SAFE_ACCESS_SIG = 0;  
}

/*******************************************************************************
* Function Name  : LowPower_Sleep
* Description    : �͹���-Sleepģʽ��
                   �˵͹����е�HSI/5ʱ�����У����Ѻ���Ҫ�û��Լ�����ѡ��ϵͳʱ��Դ
                   ע����ô˺�����DCDC����ǿ�ƹرգ����Ѻ�����ֶ��ٴδ�
* Input          : rm:
                    RB_PWR_RAM2K	-	���2K SRAM ����
                    RB_PWR_RAM14K	-	0x20004000 - 0x20007800 14K SRAM ����
                    RB_PWR_EXTEND	-	USB��RF ��Ԫ�������򹩵�
                   NULL	-	���ϵ�Ԫ���ϵ�
* Return         : None
*******************************************************************************/
//__attribute__((section("NMICode")))
void LowPower_Sleep( UINT8 rm )
{
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%�����
    if(R16_RTC_CNT_32K>0x3fff){     // ����500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE�����������͵������
    } 
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_POWER_PLAN = RB_PWR_PLAN_EN		    \
                    |RB_PWR_MUST_0010		\
                    |RB_PWR_CORE            \
                    |rm;    
    R8_SAFE_ACCESS_SIG = 0;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_BAT_DET_CTRL = 0;                // �رյ�ѹ���
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // ��Ƶ HSI/5=6.4M  
    R8_SAFE_ACCESS_SIG = 0;

	SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;

    /* ������ѹ��� */
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
		R8_BAT_DET_CFG = 1;                     // 2.05V - 2.33V
		R8_BAT_DET_CTRL = RB_BAT_DET_EN;
		R8_SAFE_ACCESS_SIG = 0; 
		asm("nop");	asm("nop");
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
		R8_BAT_DET_CTRL = RB_BAT_LOW_IE|RB_BAT_LOWER_IE|RB_BAT_DET_EN;  
		R8_SAFE_ACCESS_SIG = 0;  
}

/*******************************************************************************
* Function Name  : LowPower_Shutdown
* Description    : �͹���-Shutdownģʽ��
                   �˵͹����е�HSI/5ʱ�����У����Ѻ���Ҫ�û��Լ�����ѡ��ϵͳʱ��Դ
                   ע����ô˺�����DCDC����ǿ�ƹرգ����Ѻ�����ֶ��ٴδ�
* Input          : rm:
                    RB_PWR_RAM2K	-	���2K SRAM ����
                   NULL	-	���ϵ�Ԫ���ϵ�
* Return         : None
*******************************************************************************/
//__attribute__((section("NMICode")))
void LowPower_Shutdown( UINT8 rm )
{	
    UINT8  x32Kpw, x32Mpw;
    
    x32Kpw = R8_XT32K_TUNE;
    x32Mpw = R8_XT32M_TUNE;
    x32Mpw = (x32Mpw&0xfc)|0x03;            // 150%�����
    if(R16_RTC_CNT_32K>0x3fff){     // ����500ms
        x32Kpw = (x32Kpw&0xfc)|0x01;        // LSE�����������͵������
    }
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_POWER_PLAN = RB_PWR_PLAN_EN		    \
                    |RB_PWR_MUST_0010		\
                    |rm;
    R8_SAFE_ACCESS_SIG = 0; 

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_BAT_DET_CTRL = 0;                // �رյ�ѹ���
    R8_XT32K_TUNE = x32Kpw;
    R8_XT32M_TUNE = x32Mpw;
    R16_CLK_SYS_CFG = 5;		        // ��Ƶ HSI/5=6.4M    
    R8_SAFE_ACCESS_SIG = 0;    

	SCB -> SCR |= SCB_SCR_SLEEPDEEP_Msk;				//deep sleep
    __WFI();
    R8_SAFE_ACCESS_SIG = 0;

    /* ������ѹ��� */
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
		R8_BAT_DET_CFG = 1;                     // 2.05V - 2.33V
		R8_BAT_DET_CTRL = RB_BAT_DET_EN;
		R8_SAFE_ACCESS_SIG = 0; 
		asm("nop");	asm("nop");
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
		R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
		R8_BAT_DET_CTRL = RB_BAT_LOW_IE|RB_BAT_LOWER_IE|RB_BAT_DET_EN;  
    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
		R8_SAFE_ACCESS_SIG = 0;  
}

