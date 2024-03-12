#ifndef 	__TIMER0_H__
#define   __TIMER0_H__

#include <stdint.h>
#include <stdbool.h>

struct Timer0Delay {
    uint32_t started;
    uint32_t delay_10ms;
};

void InitTimer0(void);
uint32_t get_time_10ms(void);
bool timer0_init_wait_10ms(struct Timer0Delay *delay, uint32_t delay10ms);
bool timer0_check_wait(struct Timer0Delay *delay);


#endif 
