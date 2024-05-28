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
#include <time.h>
#include "udp.h"
#include "rtc.h"

uint32_t arg = 0;
static struct tcp_pcb *tcp_pcb_handle = NULL;
bool action_reboot = false;
bool action_sntp_update = false;

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

void http_parse_uri_key_value(const char *params)
{
    static char key_buffer[20];
    static char value_buffer[20];
    char* value_pos = strchr(params, '=');
    if(value_pos == NULL)
    {
        printf("\033[31mchar '=' not found in params\033[0m\n");
        return;
    }

    int key_strlen = value_pos- params;
    if(key_strlen >= 19)
    {
        printf("\033[31mParameter too long ignoring\033[0m\n");
        return;
    }
    strncpy(key_buffer, params, key_strlen);
    printf("param key: %s\n", key_buffer);

    int value_strlen = strlen(value_pos+1);
    if(value_strlen >= 19 || value_strlen == 0)
    {
        printf("\033[31mParameter value too long or empty ignoring\033[0m\n");
        return;
    }
    strcpy(value_buffer, value_pos+1);
    printf("param value: %s\n", value_buffer);
    
    if(strcmp(key_buffer,"action") == 0)
    {
        if(strcmp(value_buffer,"reboot") == 0)
        {
            printf("action reboot\n");
            action_reboot = true;
        }
        else if(strcmp(value_buffer,"sntp") == 0)
        {
            printf("sntp get time action\n");
            action_sntp_update = true;
        }
        else if(strcmp(value_buffer,"led_on") == 0)
        {
            led_on();
        }
        else if(strcmp(value_buffer,"led_off") == 0)
        {
            led_off();
        }
    }

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
    printf("got parameters:%s\n", params);

    http_parse_uri_key_value(params);
}


u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen)
{
    size_t len=HTTPD_SSI_TAG_UNKNOWN;
    switch (iIndex) 
    {
        case 0: //test tag
            len = snprintf(pcInsert, iInsertLen, "Hello World!");
            break;
        case 1: //time
            {
                uint32_t seconds = time(NULL);
                uint32_t minutes = seconds/60;
                seconds = seconds - minutes*60;
                uint32_t hours = minutes/60;
                minutes -= hours*60;
                len = snprintf(pcInsert, iInsertLen, "Time: %02d:%02d:%02d\n", hours, minutes, seconds);
            }
    }
    return (u16_t)len;
}

err_t httpd_post_begin(void *connection, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
    printf("Postbegin\n");
    return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
    printf("Post payload: %.*s\n", p->len, (char*)p->payload);
	pbuf_free(p);
    return ERR_OK;
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
    const char *resp_uri = "/index.shtml";
    int len = strlen(resp_uri);
    if(len >= response_uri_len)
    {
        printf("\033[31mUri response buffer to small\033[0m\n");
        return;
    }
    strcpy(response_uri, resp_uri);
    printf("httpd_post_finished\n");
}

static void udp_received(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    LWIP_UNUSED_ARG(arg);
    if(p == NULL)
        return;
    printf("\n\rgot udp len: %d\n", p->len);
    uint8_t *data = (uint8_t*)p->payload;
    for(int x=0; x < p->len; x++)
    {
        printf("0x%x ", data[x]);
    }
    
    pbuf_free(p);
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

    rtc_switch_to_LSE();
    rtc_set_day_counter(10);
    rtc_set_time(22,43,0);
    rtc_setup_timing_mode();
    rtc_regs_print();

    struct Timer0Delay reset_delay;
    struct Timer0Delay info_print;
    timer0_init_empty(&reset_delay);
    timer0_init_wait_10ms(&info_print, 100);
    const char * ssi_tags[] = {
        "test",
        "time"
    };
    http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));

    struct udp_pcb *udp_pcb;
    struct udp_pcb *udp_pcb_receive;
    err_t result;
    udp_pcb = udp_new();
    udp_pcb_receive = udp_new();
    result = udp_bind(udp_pcb_receive, IP_ADDR_ANY, 123);

    ip_addr_t sntp_server_ip;
    IP4_ADDR(&sntp_server_ip, 91, 227, 212, 78);
    udp_recv(udp_pcb , udp_received, NULL);

    unsigned char sntp_msg[48]={0x1b,0,0,0,0,0,0,0,0};
    struct pbuf *send_buffer = pbuf_alloc_reference(sntp_msg, sizeof(sntp_msg), PBUF_ROM);

    while(1)
    {
        if( action_reboot == true)
        {
            action_reboot = false;
            printf("Starting reset delay timer\n");
            timer0_init_wait_10ms(&reset_delay, 200);
        }
        if (action_sntp_update == true)
        {
            printf("SNTP sending time request\n");
            err_t status = udp_sendto( udp_pcb,send_buffer ,&sntp_server_ip, 123);
            action_sntp_update = false;
        }
        if( timer0_check_wait(&reset_delay))
        {
            printf("Reseting...\n");
            NVIC_SystemReset();
        }
        if( timer0_check_wait(&info_print))
        {
            rtc_print_time();
        }
        lwip_pkt_handle();
        lwip_periodic_handle();
        sys_check_timeouts();	
    }
}

