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

void rtc_switch_to_LSE(void)
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    //Turn on external oscillator
    R8_CK32K_CONFIG |= RB_CLK_XT32K_PON;
    //bias current to 200%
    R8_XT32K_TUNE|= 0x11;
    mDelaymS(200);
    // bias to rated current(100%)
    R8_XT32K_TUNE= (R8_XT32K_TUNE&(~0x11)) | 0x01;
    // Switch clock source
    R8_CK32K_CONFIG|= RB_CLK_OSC32K_XT;
    R8_SAFE_ACCESS_SIG = 0;
    //Wait at least half of the 32khz period to complete switching
    mDelayuS(16);
}

void rtc_regs_print(void)
{
    uint16_t rtc_int_tune = RB_INT32K_TUNE;
    printf("RB_INT32K_TUNE:0x%04x\n", rtc_int_tune);

    uint8_t rtc_external_tune =  R8_XT32K_TUNE;
    printf("R8_XT32K_TUNE:0x%02x\n", rtc_int_tune);

    uint8_t rtc_config = R8_CK32K_CONFIG;
    printf("R8_CK32K_CONFIG:0x%02x\n", rtc_config);

    uint16_t rtc_cal_counter = R16_OSC_CAL_CNT;
    printf("R16_OSC_CAL_CNT:0x%04x\n", rtc_cal_counter);

    uint8_t osc_cal_control = R8_OSC_CAL_CTRL;
    printf("R8_OSC_CAL_CTRL:0x%02x\n", osc_cal_control);

    uint8_t rtc_control = R8_RTC_FLAG_CTRL;
    printf("R8_RTC_FLAG_CTRL:0x%02x\n", rtc_control);

    uint8_t rtc_mode_control =  R8_RTC_MODE_CTRL;
    printf("R8_RTC_MODE_CTRL:0x%02x\n", rtc_mode_control);

    uint32_t rtc_trigger_value = R32_RTC_TRIG;
    printf("R32_RTC_TRIG:0x%08x\n", rtc_trigger_value);
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
    rtc_regs_print();

    struct Timer0Delay reset_delay;
    struct Timer0Delay info_print;
    timer0_init_wait_10ms(&info_print, 1000);

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
        if( timer0_check_wait(&info_print))
        {
            uint16_t rtc_32k_val = R16_RTC_CNT_32K;
            uint16_t rtc_2s_val = R16_RTC_CNT_2S;
            uint16_t rtc_day_val = R32_RTC_CNT_DAY;
            printf("RTC: %d 2s:%d day: %d\n", rtc_32k_val, rtc_2s_val, rtc_day_val);
        }
        lwip_pkt_handle();
        lwip_periodic_handle();
        sys_check_timeouts();	
    }
}

