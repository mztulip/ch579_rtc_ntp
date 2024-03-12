/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_SYS.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description 
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : SYS_GetInfoSta
* Description    : ��ȡ��ǰϵͳ��Ϣ״̬
* Input          : i: 
					refer to SYS_InfoStaTypeDef
* Return         : DISABLE  -  �ر�
				   ENABLE   -  ����
*******************************************************************************/
UINT8 SYS_GetInfoSta( SYS_InfoStaTypeDef i )
{
    if(i == STA_SAFEACC_ACT)		
        return (R8_SAFE_ACCESS_SIG & RB_SAFE_ACC_ACT);
    else  
        return (R8_GLOB_CFG_INFO&(1<<i));
}

/*******************************************************************************
* Function Name  : SYS_ResetExecute
* Description    : ִ��ϵͳ������λ
* Input          : None
* Return         : None
*******************************************************************************/
void SYS_ResetExecute( void )
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : SYS_DisableAllIrq
* Description    : �ر������жϣ���������ǰ�ж�ֵ
* Input          : pirqv����ǰ�����ж�ֵ
* Return         : None
*******************************************************************************/
void SYS_DisableAllIrq( PUINT32 pirqv)
{
	*pirqv = NVIC->ISER[0];
	NVIC->ICER[0] = 0xffffffff;
}

/*******************************************************************************
* Function Name  : SYS_RecoverIrq
* Description    : �ָ�֮ǰ�رյ��ж�ֵ
* Input          : irq_status����ǰ�����ж�ֵ
* Return         : None
*******************************************************************************/
void SYS_RecoverIrq( UINT32 irq_status )
{
	NVIC->ISER[0] = irq_status;
}

/*******************************************************************************
* Function Name  : SYS_GetSysTickCnt
* Description    : ��ȡ��ǰϵͳ(SYSTICK)����ֵ
* Input          : None
* Return         : ��ǰ����ֵ
*******************************************************************************/
uint32_t SYS_GetSysTickCnt( void )
{
	return(SysTick->VAL );
}

/*******************************************************************************
* Function Name  : WWDG_ITCfg
* Description    : ���Ź���ʱ������ж�ʹ��
* Input          : DISABLE-������ж�      ENABLE-����ж�
* Return         : None
*******************************************************************************/
void  WWDG_ITCfg( UINT8 s )
{
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	if(s == DISABLE)		R8_RST_WDOG_CTRL&=~RB_WDOG_INT_EN;
	else 					R8_RST_WDOG_CTRL|=RB_WDOG_INT_EN;
	R8_SAFE_ACCESS_SIG = 0;	
}

/*******************************************************************************
* Function Name  : WWDG_ResetCfg
* Description    : ���Ź���ʱ����λ����
* Input          : DISABLE-�������λ      ENABLE-���ϵͳ��λ
* Return         : None
*******************************************************************************/
void WWDG_ResetCfg( UINT8 s )
{
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	if(s == DISABLE)		R8_RST_WDOG_CTRL&=~RB_WDOG_RST_EN;
	else 					R8_RST_WDOG_CTRL|=RB_WDOG_RST_EN;
	R8_SAFE_ACCESS_SIG = 0;	
}

/*******************************************************************************
* Function Name  : WWDG_ClearFlag
* Description    : ������Ź��жϱ�־�����¼��ؼ���ֵҲ�����
* Input          : None
* Return         : None
*******************************************************************************/
void WWDG_ClearFlag( void )
{
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	R8_RST_WDOG_CTRL |= RB_WDOG_INT_FLAG;
	R8_SAFE_ACCESS_SIG = 0;	
}


/*******************************************************************************
* Function Name  : mDelayuS
* Description    : uS ��ʱ
* Input          : t: ʱ�����
* Return         : None
*******************************************************************************/
void mDelayuS( UINT16 t )
{
    UINT16 i, j;

    for(j=0; j<t; j++)
    {

#if     (FREQ_SYS == 40000000)	
    for(i=0; i<4; i++)		asm("nop");

#elif       (FREQ_SYS == 32000000)		
    i = 2;
    while(i--)	{ asm("nop");	asm("nop"); }		

#elif       (FREQ_SYS == 24000000)		
    i = 1;
    while(i--)	{ asm("nop");	asm("nop"); }

 #elif       (FREQ_SYS == 20000000)		
    for(i=0; i<1; i++)		asm("nop");

 #elif       (FREQ_SYS == 16000000)			
    asm("nop"); asm("nop");	asm("nop"); asm("nop");
    asm("nop"); asm("nop");   asm("nop"); asm("nop"); asm("nop");

#elif       (FREQ_SYS == 8000000)		
    asm("nop"); asm("nop");
    
#endif			
    
    }	
}

/*******************************************************************************
* Function Name  : mDelaymS
* Description    : mS ��ʱ
* Input          : t: ʱ�����
* Return         : None
*******************************************************************************/
void mDelaymS( UINT16 t )
{
    UINT16 i;

    for(i=0; i<t; i++)
        mDelayuS(1000);
}


#if( defined  DEBUG)
int fputc( int c, FILE *f )
{
#if  DEBUG == Debug_UART0
  while( R8_UART0_TFC == UART_FIFO_SIZE );                        /* �ȴ����ݷ��� */
  R8_UART0_THR = c;                                               /* �������� */
#elif DEBUG == Debug_UART1       
  while( R8_UART1_TFC == UART_FIFO_SIZE );                        /* �ȴ����ݷ��� */
  R8_UART1_THR = c;                                               /* �������� */
#elif DEBUG == Debug_UART2       
  while( R8_UART2_TFC == UART_FIFO_SIZE );                        /* �ȴ����ݷ��� */
  R8_UART2_THR = c;                                               /* �������� */
#elif DEBUG == Debug_UART3       
  while( R8_UART3_TFC == UART_FIFO_SIZE );                        /* �ȴ����ݷ��� */
  R8_UART3_THR = c;                                               /* �������� */
#endif
  return( c );
}
#endif
