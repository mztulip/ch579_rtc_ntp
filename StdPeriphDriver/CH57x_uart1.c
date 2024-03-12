/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_uart1.c
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
* Function Name  : UART1_DefInit
* Description    : ����Ĭ�ϳ�ʼ������
* Input          : None
* Return         : None
*******************************************************************************/
void UART1_DefInit( void )
{	
    UART1_BaudRateCfg( 115200 );
    R8_UART1_FCR = (2<<6) | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;		// FIFO�򿪣�������4�ֽ�
    R8_UART1_LCR = RB_LCR_WORD_SZ;	
    R8_UART1_IER = RB_IER_TXD_EN;
    R8_UART1_DIV = 1;	
}

/*******************************************************************************
* Function Name  : UART1_BaudRateCfg
* Description    : ���ڲ���������
* Input          : 
* Return         : 
*******************************************************************************/
void UART1_BaudRateCfg( UINT32 baudrate )
{
    UINT32	x;

    x = 10 * GetSysClock() / 8 / baudrate;
    x = ( x + 5 ) / 10;
    R16_UART1_DL = (UINT16)x;
}

/*******************************************************************************
* Function Name  : UART1_ByteTrigCfg
* Description    : �����ֽڴ����ж�����
* Input          : b: �����ֽ���
                    refer to UARTByteTRIGTypeDef
* Return         : 
*******************************************************************************/
void UART1_ByteTrigCfg( UARTByteTRIGTypeDef b )
{
    R8_UART1_FCR = (R8_UART1_FCR&~RB_FCR_FIFO_TRIG)|(b<<6);
}

/*******************************************************************************
* Function Name  : UART1_INTCfg
* Description    : �����ж�����
* Input          : s:  �жϿ���״̬
					ENABLE  - ʹ����Ӧ�ж�    
					DISABLE - �ر���Ӧ�ж�
				   i:  �ж�����
					RB_IER_MODEM_CHG  - ���ƽ��������״̬�仯�ж�ʹ��λ���� UART0 ֧�֣�
					RB_IER_LINE_STAT  - ������·״̬�ж�
					RB_IER_THR_EMPTY  - ���ͱ��ּĴ������ж�
					RB_IER_RECV_RDY   - ���������ж�
* Return         : None
*******************************************************************************/
void UART1_INTCfg( UINT8 s,  UINT8 i )
{
    if( s )
    {
        R8_UART1_IER |= i;
        R8_UART1_MCR |= RB_MCR_INT_OE;
    }
    else
    {
        R8_UART1_IER &= ~i;
    }
}

/*******************************************************************************
* Function Name  : UART1_Reset
* Description    : ����������λ
* Input          : None
* Return         : None
*******************************************************************************/
void UART1_Reset( void )
{
    R8_UART1_IER = RB_IER_RESET;
}

/*******************************************************************************
* Function Name  : UART1_SendString
* Description    : ���ڶ��ֽڷ���
* Input          : buf - �����͵����������׵�ַ
                     l - �����͵����ݳ���
* Return         : None
*******************************************************************************/
void UART1_SendString( PUINT8 buf, UINT16 l )
{
    UINT16 len = l;

    while(len)
    {
        if(R8_UART1_TFC != UART_FIFO_SIZE)
        {
            R8_UART1_THR = *buf++;
            len--;
        }		
    }
}

/*******************************************************************************
* Function Name  : UART1_RecvString
* Description    : ���ڶ�ȡ���ֽ�
* Input          : buf - ��ȡ���ݴ�Ż������׵�ַ
* Return         : ��ȡ���ݳ���
*******************************************************************************/
UINT16 UART1_RecvString( PUINT8 buf )
{
    UINT16 len = 0;

    while( R8_UART1_RFC )
    {
        *buf++ = R8_UART1_RBR;
        len ++;
    }

    return (len);
}


