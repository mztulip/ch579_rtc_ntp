#ifndef  	__ETHERNET_IF_H__
#define  	__ETHERNET_IF_H__

extern   struct netif  lwip_netif;

void  ethernetif_input(struct netif *netif);
err_t ethernetif_init(struct netif *netif);

#endif 


