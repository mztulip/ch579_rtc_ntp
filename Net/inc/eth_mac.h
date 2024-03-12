#ifndef   __ETH_MAC_H__
#define   __ETH_MAC_H__


#include <stdio.h>
#include <string.h>

#include "CH579SFR.h"
#include "core_cm0.h"
#include "CH57x_common.h"


extern  UINT8   myCh579MAC[];
extern UINT8 phy_flag;
//MAC
#define R8_ETH_MAADR1           (*((PUINT8V)(ETH_BASE_ADDR+0x28)))              /* RW MAC Address Byte1 */
#define R8_ETH_MAADR2           (*((PUINT8V)(ETH_BASE_ADDR+0x29)))              /* RW MAC Address Byte2 */
#define R8_ETH_MAADR3           (*((PUINT8V)(ETH_BASE_ADDR+0x2a)))              /* RW MAC Address Byte3 */
#define R8_ETH_MAADR4           (*((PUINT8V)(ETH_BASE_ADDR+0x2b)))              /* RW MAC Address Byte4 */
#define R8_ETH_MAADR5           (*((PUINT8V)(ETH_BASE_ADDR+0x2c)))              /* RW MAC Address Byte5 */
#define R8_ETH_MAADR6           (*((PUINT8V)(ETH_BASE_ADDR+0x2d))) 


#define  RB_ETH_MIREGADR_MIIWR  	0x20      
#define  RB_ETH_MIREGADR_MIRDL  	0x1f  


//PHY
#define PHY_BMCR                0x00                 /* Control Register */
#define PHY_BMSR                0x01                 /* Status Register */
#define PHY_ANAR                0x04                 /* Auto-Negotiation Advertisement Register */
#define PHY_ANLPAR              0x05                 /* Auto-Negotiation Link Partner Base  Page Ability Register*/
#define PHY_ANER                0x06                 /* Auto-Negotiation Expansion Register */
#define PHY_MDIX                0x1e                 /* �Զ��� MDIX ģʽ�Ĵ��� */
//  @PHY_MDIX
#define PN_NORMAL               0x04                 /* ģ��p��n����ѡ�� */
#define MDIX_MODE_MASK          0x03                 /* mdix���� */
#define MDIX_MODE_AUTO          0x00                 /*  */
#define MDIX_MODE_MDIX          0x01
#define MDIX_MODE_MDI           0x02
//ECON2
#define RX_VCM_MODE_0
#define RX_VCM_MODE_1
#define RX_VCM_MODE_2
#define RX_VCM_MODE_3
//RX  @RX_REF
#define RX_REF_25mV             (0<<2)              /* 25mV */
#define RX_REF_49mV             (1<<2)              /* 49mV */
#define RX_REF_74mV             (2<<2)              /* 74mV */
#define RX_REF_98mV             (3<<2)              /* 98mV */
#define RX_REF_123mV            (4<<2)              /* 123mV */
#define RX_REF_148mV            (5<<2)              /* 148mV */
#define RX_REF_173mV            (6<<2)              /* 173mV */
#define RX_REF_198mV            (7<<2)              /* 198mV */
//TX DRIVER  @TX_AMP
#define TX_AMP_0                (0<<0)              /* 43mA   / 14.5mA   (1.4V/0.7V) */
#define TX_AMP_1                (1<<0)              /* 53.1mA / 18mA     (1.8V/0.9V) */
#define TX_AMP_2                (2<<0)              /* 75.6mA / 25.6mA   (2.6V/1.3V) */
#define TX_AMP_3                (3<<0)                                          /* 122mA  / 41.45mA  (4.1V/2.3V) */
//FCEN     @FCEN
#define FCEN_0_TIMER            (3<<4)              /* ���� 0 timer ��ͣ֡��Ȼ��ֹͣ���� */
#define FCEN_CYCLE              (2<<4)              /* �����Է�����ͣ֡ */
#define FCEN_ONCE               (1<<4)              /* ����һ����ͣ֡��Ȼ��ֹͣ���� */
#define FCEN_STOP               (0<<4)              /* ֹͣ������ͣ֡ */
//PADCFG @PADCFG
#define PADCFG_AUTO_0           (7<<5)              /* ���ж̰����00h��64�ֽڣ���4�ֽ�crc */
#define PADCFG_NO_ACT_0         (6<<5)              /* �����̰� */
#define PADCFG_DETE_AUTO        (5<<5)             /* ��⵽�ֶ�Ϊ8100h��VLAN������Զ����00h��64�ֽڣ�����̰����60�ֽ�0��������4�ֽ�crc */
#define PADCFG_NO_ACT_1         (4<<5)             /* �����̰� */
#define PADCFG_AUTO_1           (3<<5)             /* ͬ111 */
#define PADCFG_NO_ACT_2         (2<<5)             /* �����̰� */
#define PADCFG_AUTO_3           (1<<5)             /* ���ж̰����00h��60�ֽڣ���4�ֽ�crc */
#define PADCFG_NO_ACT_3         (0<<5)            /* �����̰� */


#define ETHER_HEAD_LEN          14                 
#define MACADDR_LEN             6
#define MAC_MAX_LEN             1064      
#define RX_MAX_BUF_LEN          1064          
#define RX_BUF_SIZE             1064             
#define RX_QUEUE_NUM            3         
//Payload 1500+ 2x6MAC + 2 ether type
#define phy_tx_buffer_len_macro       (1500+2+6+6)

#define  MAX_FRAMELEN			(MAC_MAX_LEN-18)

typedef struct __attribute__((__packed__)) _RXBUFST
{
    UINT8  RecvEn;                                
    UINT8  RecvIndex;                           
    UINT8  ReadIndex;                          
    UINT8  RemainCout;                       
    UINT8  RxBufStau[RX_QUEUE_NUM];            
    UINT16 RxBufLen[RX_QUEUE_NUM];           
    UINT32 RxBufAddr[RX_QUEUE_NUM];        
}RXBUFST;

void    ETHInitX(void);
void    ETHParaInitX(void);
UINT16  ETHRecX(UINT8     *pRecvBuf, UINT16 RecvBufSize);
UINT8   ETHSendX(UINT8 *p_send_buf, UINT16 send_len);
void    ETH_IRQHandler(void);
UINT16  GetRecvMacDataLen(void);
UINT8   GetPhyLinkState(void);

void phy_change(UINT8 flag);
uint8_t* phy_get_tx_buf(void);
bool phy_is_data_sent(void);
int8_t phy_send_tx_buf(uint16_t tx_len);
#endif 

