//Based on https://gitee.com/maji19971221/lwip-routine
// Modified by mztulip
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "lwip/ip4_frag.h"
#include "lwip/tcpip.h" 
#include "lwip/timeouts.h"
#include "lwip/netif.h"
#include "lwip/init.h"
#include "lwip/def.h"
#include "lwip/stats.h"
#include "ethernetif.h" 
#include "lwipcomm.h"
#include "timer0.h"

#include <stdio.h>
#include "eth_mac.h"
#include "ip4_addr.h"

struct netif lwip_netif;

UINT8 g_eth_phy_link_state = 0;
static UINT8 phyflag = 0;
ip4_addr_t ipaddr;  			
ip4_addr_t netmask; 			
ip4_addr_t gw;

void eth_green_led_on(void)
{
	GPIOB_ResetBits(GPIO_Pin_4);
}

void eth_green_led_off(void)
{
	GPIOB_SetBits(GPIO_Pin_4);
}

void eth_amber_led_on(void)
{
	GPIOB_SetBits(GPIO_Pin_7);
}

void eth_amber_led_off(void)
{
	GPIOB_SetBits(GPIO_Pin_4);
}

static void  IP4_ADDR_X(struct ip4_addr *ipaddr,u32_t ipaddrx)
{
	ipaddr->addr = htonl(ipaddrx);
}

void eth_status_led_init(void)
{
	GPIOB_ModeCfg(GPIO_Pin_4, GPIO_ModeOut_PP_20mA);
	GPIOB_ModeCfg(GPIO_Pin_7, GPIO_ModeOut_PP_20mA);

	eth_green_led_off();
	eth_green_led_off();
}

void netif_status_callback(struct netif *netif)
{
  printf("netif status changed %s\n\r", ip4addr_ntoa(netif_ip4_addr(netif)));
}

uint8_t lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;	

	eth_status_led_init();
	lwip_init();

	IP4_ADDR(&ipaddr,  192, 168,   2, 236);
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   2,  1);
   
	printf("\n\rIP:%ld.%ld.%ld.%ld\n\r",  \
        ((ipaddr.addr)&0x000000ff),       \
        (((ipaddr.addr)&0x0000ff00)>>8),  \
        (((ipaddr.addr)&0x00ff0000)>>16), \
        ((ipaddr.addr)&0xff000000)>>24);
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);

	if(Netif_Init_Flag==NULL)
	{
		printf("netif_add fail\n\r");
		return 4;
	}
	else
	{
		netif_set_status_callback(&lwip_netif, netif_status_callback);
		netif_set_default(&lwip_netif); 
		netif_set_up(&lwip_netif);
		netif_set_link_up(&lwip_netif); 
		// dhcp_start(&lwip_netif);
	}

	printf("lwip initialised sucessfully.\n\r");
	return 0;
	
}

void lwip_pkt_handle(void)
{
    ethernetif_input(&lwip_netif);
}

void lwip_periodic_handle(void)
{
	static UINT32 last_timestamp = 0;
	static UINT8  last_phy_link_state = 0;
	UINT8 phy_link_state = 0;
	UINT32 cur_timestamp = get_time_10ms();

	if(cur_timestamp < last_timestamp)
	{
		last_timestamp = 0;
		return;
	}

	if(cur_timestamp - last_timestamp < 50)
	{
		//500ms
		return;
	}

	//500ms
	last_timestamp = cur_timestamp;

	phy_link_state = GetPhyLinkState();
	
	if(phy_link_state == 0)
	{
		phy_change(phyflag);
		phyflag += 1;
		DelayUs(phyflag);
		if(phyflag == 200)
			phyflag=0;
	}
	
	g_eth_phy_link_state = phy_link_state;

	if(phy_link_state != last_phy_link_state)
	{
		if(phy_link_state)
		{
			eth_green_led_on();
			printf("\033[32mlink connected\033[0m\r\n");
			netif_set_link_up(&lwip_netif);
		}
		else
		{
			eth_green_led_off();
			printf("\033[31mlink disconnected\033[0m\r\n");
			netif_set_link_down(&lwip_netif);
		}
	}

	last_phy_link_state = phy_link_state;	
}

