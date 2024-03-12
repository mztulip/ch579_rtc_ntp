
#ifndef 	__PARAMETER_SETTING_H__
#define   __PARAMETER_SETTING_H__

void eth_green_led_on(void);
void eth_green_led_off(void);
void eth_amber_led_on(void);
void eth_amber_led_off(void);
uint8_t lwip_comm_init(void);
void  lwip_pkt_handle(void);
void  lwip_periodic_handle(void);
#endif