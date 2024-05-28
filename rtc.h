#ifndef __RTC__
#define __RTC__

void rtc_switch_to_LSE(void);
void rtc_set_counter(uint32_t count);
void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
void rtc_print_time();
void rtc_set_day_counter(uint16_t day);
void rtc_regs_print(void);
void rtc_counters_print(void);
void rtc_setup_timing_mode(void);

#endif