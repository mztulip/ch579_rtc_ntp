#include "CH57x_common.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include"rtc.h"


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