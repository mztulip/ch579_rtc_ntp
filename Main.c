#include "CH57x_common.h"
#include <stdio.h>
#include <string.h>
#include "timer0.h"
#include "lwipcomm.h"
#include "lwip/timeouts.h"
#include "eth_mac.h"
#include "ethernetif.h"
#include "tcp.h"
#include "httpd.h"

uint32_t arg = 0;
static struct tcp_pcb *tcp_pcb_handle = NULL;
bool action_reboot = false;

void uart_init(void)		
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

void led_init(void)
{
    GPIOB_ModeCfg( GPIO_Pin_0, GPIO_ModeOut_PP_20mA );
}

void led_on(void)
{
    GPIOB_SetBits( GPIO_Pin_0 ); 
}

void led_off(void)
{
    GPIOB_ResetBits( GPIO_Pin_0 );
}

void httpd_GET_uri_params_parse(const char *uri)
{
    printf("http_get_uri_params_parse uri: %s\n", uri);
    char* params_char_pos = strchr(uri, '?');
    if(params_char_pos == NULL)
    {
        return;
    }
    char *params = &params_char_pos[1];
    printf("got parameters:%s", params);
    if(strcmp(params,"action=reboot") == 0)
    {
        printf("action reboot\n");
        action_reboot = true;
    }
    else if(strcmp(params,"action=led_on") == 0)
    {
        led_on();
    }
    else if(strcmp(params,"action=led_off") == 0)
    {
        led_off();
    }

}

// Very helpful link https://lwip.fandom.com/wiki/Raw/TCP
int main()
{ 
	SystemInit();

    PWR_UnitModCfg(ENABLE, (UNIT_SYS_PLL|UNIT_ETH_PHY)); 
    DelayMs(3); 
    SetSysClock(CLK_SOURCE_HSE_32MHz); 

    led_init();
    InitTimer0();
    uart_init();
    printf("\n\rHTTP server.\n\r");
    lwip_comm_init(); 

    httpd_init();

    struct Timer0Delay reset_delay;

    while(1)
    {
        if( action_reboot == true)
        {
            action_reboot = false;
            printf("Starting reset delay timer\n");
            timer0_init_wait_10ms(&reset_delay, 200);
        }
        if( timer0_check_wait(&reset_delay))
        {
            printf("Reseting...\n");
            NVIC_SystemReset();
        }
        lwip_pkt_handle();
        lwip_periodic_handle();
        sys_check_timeouts();	
    }
}

