#include "timer0.h"
#include "CH57x_common.h"
#include "cc.h"

static uint32_t  time_10ms = 0;
 

/*************************************************************************
Timer0    10ms
*************************************************************************/
void InitTimer0(void)
{
  TMR0_TimerInit((10000*32));  //10ms 
  TMR0_ITCfg(ENABLE, TMR0_3_IT_CYC_END); 
  NVIC_EnableIRQ(TMR0_IRQn);
}

uint32_t get_time_10ms(void)
{
    return time_10ms;
}

void TMR0_IRQHandler( void )        // TMR0
{
    TMR0_ClearITFlag( TMR0_3_IT_CYC_END );
    time_10ms++;
}

bool timer0_init_wait_10ms(struct Timer0Delay *delay, uint32_t delay10ms)
{
  delay->delay_10ms = delay10ms;
  delay->started = time_10ms;
  delay->initialised = true;
}

void timer0_init_empty(struct Timer0Delay *delay)
{
  delay->initialised = false;
}

bool timer0_check_wait(struct Timer0Delay *delay)
{
  if(delay->initialised == false) {return false;}
  uint32_t finish_time = delay->started + delay->delay_10ms;
  if(time_10ms >= finish_time)
  {
    delay->started = time_10ms;
    return true;
  }
  return false;
}

//Should return time in miliseconds
u32_t sys_now()
{
  	return time_10ms*10;
}
