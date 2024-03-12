#include "eth_mac.h"
#include "CH57x_common.h"

static  RXBUFST   ETHRxMagPara;   

static  __attribute__((aligned(4))) UINT8     MACRxBuf[RX_QUEUE_NUM][RX_BUF_SIZE];  


volatile bool phy_tx_finished = true;
static  __attribute__((aligned(4))) uint8_t	phy_tx_buf[phy_tx_buffer_len_macro];

uint8_t* phy_get_tx_buf(void)
{
	return phy_tx_buf;
}

UINT8   myCh579MAC[MACADDR_LEN] = {0,0,0,0,0,0};

UINT8 phy_flag=0;

static void WritePHYRegX(UINT8 reg_add,UINT16 reg_val)
{
	UINT8 reg_op=0;
	
	R16_ETH_MIWR = reg_val;
	reg_op = (reg_add&RB_ETH_MIREGADR_MIRDL);
	R8_ETH_MIREGADR = reg_op;   
}

static UINT16 ReadPHYRegX(UINT8 reg_add)
{
	UINT8 reg_op=0;
	UINT16 read_reg_val = 0xffff;
	
	reg_op = reg_add&RB_ETH_MIREGADR_MIRDL;
	R8_ETH_MIREGADR = RB_ETH_MIREGADR_MIIWR|reg_op;
	read_reg_val = R16_ETH_MIRD;
	
	return read_reg_val;
}

void ETHParaInitX(void)
{
	UINT8 i = 0;
	
	memset((char *)&MACRxBuf[0][0],0,sizeof(MACRxBuf));
	memset((char *)&ETHRxMagPara,0,sizeof(ETHRxMagPara));
	
	for(i=0; i<RX_QUEUE_NUM; i++) 
	{
		ETHRxMagPara.RxBufAddr[i] = (UINT32)(&MACRxBuf[i][0]);
	}

	ETHRxMagPara.RecvEn = 0;   

	GetMACAddress(myCh579MAC);
	printf("Mac: %x:%x:%x:%x:%x:%x\n\r",myCh579MAC[5], myCh579MAC[4],myCh579MAC[3],myCh579MAC[2],myCh579MAC[1],myCh579MAC[0]);
}

void phy_tx_init()
{
	phy_tx_finished = true;
	R16_ETH_ETXST = (uint16_t)((uint32_t)phy_tx_buf&0x0000ffff);
}

void ETHInitX(void)
{

	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_ETH;	     
	R8_SLP_POWER_CTRL &= ~RB_SLP_ETH_PWR_DN;
	R8_SAFE_ACCESS_SIG = 0;	

	R8_ETH_EIE = 0;
	R8_ETH_EIE |= RB_ETH_EIE_INTIE |
                  RB_ETH_EIE_RXIE  |
	              RB_ETH_EIE_LINKIE|
	              RB_ETH_EIE_TXIE  |
	              RB_ETH_EIE_TXERIE|
	              RB_ETH_EIE_RXERIE;   
	
	
	R8_ETH_EIE |= RB_ETH_EIE_R_EN50;    
	
	R8_ETH_EIR = 0xff;               			
	R8_ETH_ESTAT |= RB_ETH_ESTAT_INT | RB_ETH_ESTAT_BUFER;  
	
	R8_ETH_ECON1 |= (RB_ETH_ECON1_TXRST|RB_ETH_ECON1_RXRST);   
	R8_ETH_ECON1 &= ~(RB_ETH_ECON1_TXRST|RB_ETH_ECON1_RXRST);
	

	R8_ETH_ERXFCON = 0; 
	
	R8_ETH_MACON1 |= RB_ETH_MACON1_MARXEN;       
	R8_ETH_MACON2 &= ~RB_ETH_MACON2_PADCFG;

	R8_ETH_MACON2 |= PADCFG_AUTO_3;             
	
	R8_ETH_MACON2 |= RB_ETH_MACON2_TXCRCEN;     
	R8_ETH_MACON2 &= ~RB_ETH_MACON2_HFRMEN;        
	

	R8_ETH_MACON2 |= RB_ETH_MACON2_FULDPX;   
	R16_ETH_MAMXFL = MAC_MAX_LEN;        
	
	R8_ETH_MAADR1 = myCh579MAC[5];         
	R8_ETH_MAADR2 = myCh579MAC[4];
	R8_ETH_MAADR3 = myCh579MAC[3];
	R8_ETH_MAADR4 = myCh579MAC[2];
	R8_ETH_MAADR5 = myCh579MAC[1];
	R8_ETH_MAADR6 = myCh579MAC[0];
	
	R16_PIN_ANALOG_IE |= RB_PIN_ETH_IE;
	
	R16_ETH_ERXST = (UINT16)ETHRxMagPara.RxBufAddr[ETHRxMagPara.RecvIndex];    

	R8_ETH_ECON1 |= RB_ETH_ECON1_RXEN;

	phy_tx_init();                                 

	NVIC_EnableIRQ(ETH_IRQn); 
}

static void ETH_IRQ_ERR_Deal(UINT8 err_sta)
{
	if(err_sta&RB_ETH_ESTAT_BUFER)    printf("err:BUFER\r\n");
	if(err_sta&RB_ETH_ESTAT_RXCRCER)  printf("err:RXCRCER\r\n");
	if(err_sta&RB_ETH_ESTAT_RXNIBBLE) printf("err:RXNIBBLE\r\n");
	if(err_sta&RB_ETH_ESTAT_RXMORE)   printf("err:RXMORE\r\n");
	if(err_sta&RB_ETH_ESTAT_RXBUSY)   printf("err:RXBUSY\r\n");
	if(err_sta&RB_ETH_ESTAT_TXABRT)   printf("err:TXABRT\r\n");
}

void print_eth2_frame(uint8_t *p_data, uint16_t len)
{
	printf("eth2 len tx: %d\n\r", len);
	for(int i =0 ; i < len;i++)
	{
		uint8_t *pointer =  p_data+i;
		printf("%02x ", *pointer);
		// printf("%c", *pointer);
	}

	//dest
	printf("\n\rDest:");
	uint8_t dest = 0;
	for(int i =dest ; i < 6;i++)
	{
		printf("%02x ", *(uint8_t*)(p_data+i));
	}
	printf("\n\rSource:");
	uint8_t source = dest+6;
	for(int i = source ; i < (source+6);i++)
	{
		printf("%02x ", *(p_data+i));
	}
	printf("\n\r");
	printf("Ether type:");
	uint8_t ether_type = source+6;
	for(int i =ether_type ; i < (ether_type+2);i++)
	{
		printf("%02x ", *(p_data+i));
	}
	uint8_t data_loc = ether_type+2;
	printf("\n\rData:\n\r");
	for(int i =data_loc ; i < len;i++)
	{
		uint8_t *pointer =  p_data+i;
		// printf("%02x ", *pointer);
		printf("%c", *pointer);
	}
	printf("\n\r");
}

int8_t phy_send_tx_buf(uint16_t tx_len)
{
	// print_eth2_frame(phy_tx_buf, tx_len);
	R16_ETH_ETXLN = tx_len;
	phy_tx_finished = false;
	R8_ETH_ECON1 |= RB_ETH_ECON1_TXRTS;  
	return 0;
}

bool phy_is_data_sent(void)
{
	return phy_tx_finished;
}

UINT16 ETHRecX(UINT8     *pRecvBuf, UINT16 RecvBufSize)
{
	UINT16  recv_len = 0;
	UINT8  *p_data = NULL;
	UINT8  *p_rx_buf = NULL;

	
	if(!ETHRxMagPara.RemainCout) 
	{
		//ETHRxMagPara.RemainCout == 0 
		return 0;  
	}
	
	
	p_data = pRecvBuf;
	p_rx_buf = (UINT8 *)ETHRxMagPara.RxBufAddr[ETHRxMagPara.ReadIndex];
	recv_len = (ETHRxMagPara.RxBufLen[ETHRxMagPara.ReadIndex]<RecvBufSize)?ETHRxMagPara.RxBufLen[ETHRxMagPara.ReadIndex]:RecvBufSize ;
	
	memcpy(p_data, p_rx_buf, recv_len);
	//For debugging frame content
	// printf("\nlen: %d", recv_len);
	// for(int i =0 ; i < recv_len;i++)
	// {
	// 	uint8_t *pointer =  p_data+i;
	// 	printf("%02x ", *pointer);
	// }
	
	ETHRxMagPara.RxBufStau[ETHRxMagPara.ReadIndex] = 0;
	ETHRxMagPara.ReadIndex++;
	if(ETHRxMagPara.ReadIndex>=RX_QUEUE_NUM) ETHRxMagPara.ReadIndex = 0;

	ETHRxMagPara.RemainCout--;
	
	return recv_len;
	
}

UINT16 GetRecvMacDataLen(void)
{
	UINT16  recv_len = 0;

	if(!ETHRxMagPara.RemainCout) 
	{
		//ETHRxMagPara.RemainCout == 0
		return 0;  
	}

	recv_len = ETHRxMagPara.RxBufLen[ETHRxMagPara.ReadIndex];

	return recv_len;
}


extern void  lwip_pkt_handle(void);

void ETH_IRQHandler(void)
{
	UINT8   eth_irq_flag = 0;
	UINT8   eth_irq_status = 0;
	UINT16  phy_reg = 0;
	UINT16  rec_len = 0;

	eth_irq_flag = R8_ETH_EIR;
	
	if(eth_irq_flag&RB_ETH_EIR_RXIF)                                           
	{	
		rec_len = R16_ETH_ERXLN;
		if(ETHRxMagPara.RemainCout<(RX_QUEUE_NUM-1))  
		{
			ETHRxMagPara.RxBufStau[ETHRxMagPara.RecvIndex] = 1;
			ETHRxMagPara.RxBufLen[ETHRxMagPara.RecvIndex] = rec_len;
			ETHRxMagPara.RemainCout++;
			ETHRxMagPara.RecvIndex++;
			if(ETHRxMagPara.RecvIndex>=RX_QUEUE_NUM) ETHRxMagPara.RecvIndex = 0;
			R16_ETH_ERXST = (UINT16)ETHRxMagPara.RxBufAddr[ETHRxMagPara.RecvIndex]; 
		}
		else                                                                        
		{
			ETHRxMagPara.RxBufStau[ETHRxMagPara.RecvIndex] = 1;
			ETHRxMagPara.RxBufLen[ETHRxMagPara.RecvIndex] = rec_len;
		}

		// printf("recv finish:%d\r\n", rec_len);
		//lwip_pkt_handle();
		R8_ETH_EIR = RB_ETH_EIR_RXIF; 
	}	
	if(eth_irq_flag&RB_ETH_EIR_LINKIF)                                         
	{
		//currently status is polled in main loop with lwip_periodic_handle
		R8_ETH_EIR = RB_ETH_EIR_LINKIF;
	}
	if(eth_irq_flag&RB_ETH_EIR_TXIF)                                           
	{
		// printf("send finish interrupt\r\n");
		phy_tx_finished = true;                                    
		R8_ETH_EIR = RB_ETH_EIR_TXIF;
	}

	
	if(eth_irq_flag&(RB_ETH_EIR_TXERIF|RB_ETH_EIR_RXERIF)) 
	{
		eth_irq_status = R8_ETH_ESTAT;
		ETH_IRQ_ERR_Deal(eth_irq_status);  
		R8_ETH_EIR = (RB_ETH_EIR_TXERIF|RB_ETH_EIR_RXERIF);
		if(eth_irq_flag&RB_ETH_EIR_TXERIF)
		{
			printf("\033[31mPhy transmission error.\033[0m\n\r");
		}
		if(eth_irq_flag&RB_ETH_EIR_RXERIF)
		{
			printf("\033[31mPhy reception error.\033[0m\n\r");
		}
	}
	
}

void phy_change(UINT8 flag)
{
	if((flag%2)==0)
		R32_ETH_MIWR=0x0004011e;
	else if(flag%2)
   	R32_ETH_MIWR=0x0000011e; 	
}

UINT8 GetPhyLinkState(void)
{
	UINT16  phy_reg = 0;
	phy_reg = ReadPHYRegX(PHY_BMSR);                                     
	if(phy_reg&0x04) 
	{
//		printf("link connected\r\n");
		phy_flag = 1;
		return 1;
	}
	//printf("link disconnected\r\n");
	return 0;
	
}

