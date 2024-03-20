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
#include <string.h>

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

void http_parse_key_value(const char *params)
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

    http_parse_key_value(params);
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

void rtc_set_counter(uint32_t count)
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R32_RTC_TRIG = count;
    // Load main counter from R32_RTC_TRIG
    // 32 bits are loaded, what gives maximum 4294967295 counts of 32768Hz crystal
    // it rotates every 24h=2831155200 counts
    R8_RTC_MODE_CTRL|= RB_RTC_LOAD_LO;
    R8_SAFE_ACCESS_SIG = 0;
}

void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    uint32_t count = 32768*(seconds+60*minutes+3600*hours);
    rtc_set_counter(count);
}

int _gettimeofday(struct timeval *tv, void *tz)
{
    uint32_t rtc_cnt = R32_RTC_CNT_32K;
    uint32_t seconds = rtc_cnt/32768;
    tv->tv_sec = seconds;  
    tv->tv_usec = ((rtc_cnt%32768)*1000)/32768;  // get remaining microseconds
    return 0; 
}

void rtc_print_time()
{
    uint32_t seconds = time(NULL);
    uint32_t minutes = seconds/60;
    seconds = seconds - minutes*60;
    uint32_t hours = minutes/60;
    minutes -= hours*60;
    printf("Time: %02d:%02d:%02d\n", hours, minutes, seconds);
}

void rtc_set_day_counter(uint16_t day)
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R32_RTC_TRIG = (uint32_t)day;
    // Load day counter from R32_RTC_TRIG
    // Only 14 bits are loaded, what gives maximum 16383 days
    R8_RTC_MODE_CTRL|= RB_RTC_LOAD_HI;
    R8_SAFE_ACCESS_SIG = 0;
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

void rtc_counters_print(void)
{
    uint16_t rtc_32k_val = R16_RTC_CNT_32K;
    uint16_t rtc_2s_val = R16_RTC_CNT_2S;
    uint32_t rtc_cnt = R32_RTC_CNT_32K;
    uint16_t rtc_day_val = R32_RTC_CNT_DAY;
    printf("RTC 32bit:%u CNT: %d 2s:%d day: %d\n", rtc_cnt, rtc_32k_val, rtc_2s_val, rtc_day_val);
}

void rtc_setup_timing_mode(void)
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    //0x03=>1S every 1 second
    //0x04=>2S timing cycle about 8 seconds
    R8_RTC_MODE_CTRL|=RB_RTC_TMR_EN | 0x05; 
    R8_SAFE_ACCESS_SIG = 0;
    NVIC_EnableIRQ(RTC_IRQn); 
}

void RTC_IRQHandler(void)
{
    uint8_t status = R8_RTC_FLAG_CTRL;
    printf("R8_RTC_FLAG_CTRL: %x\n", status);
    if(status&RB_RTC_TMR_FLAG)
    {
        R8_RTC_FLAG_CTRL|=RB_RTC_TMR_CLR;
        printf("RTC timing\n");
    }

    if(status&RB_RTC_TRIG_FLAG)
    {
        R8_RTC_FLAG_CTRL|=RB_RTC_TRIG_CLR;
        printf("RTC trigger\n");
    }
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
    timer0_init_wait_10ms(&info_print, 100);
    const char * ssi_tags[] = {
        "test",
        "time"
    };
    http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));

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
            rtc_print_time();
        }
        lwip_pkt_handle();
        lwip_periodic_handle();
        sys_check_timeouts();	
    }
}

