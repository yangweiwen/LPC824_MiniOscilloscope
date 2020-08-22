#include "delay.h"

uint32_t Delay_us(uint32_t DelayUS)
{
	uint32_t ticks,countFlag;
	if(SysTick_CTRL_CLKSOURCE)
	{
		ticks=SystemCoreClock/1000000*DelayUS;
		
	}else{
		ticks=SystemCoreClock/2/1000000*DelayUS;
	}
	if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);                                                   /* Reload value impossible */
  }
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = (SysTick_CTRL_CLKSOURCE<<SysTick_CTRL_CLKSOURCE_Pos)	|
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
	do
	{
		countFlag=SysTick->CTRL;
		
	}while(countFlag&1UL&&!(countFlag&(1<<16)));
	
	SysTick->CTRL  = 0UL;
	SysTick->VAL   = 0UL;   
	return 0;
}

uint32_t Delay_ms(uint32_t DelayMS)
{
	uint32_t ticks,countFlag;
	if(SysTick_CTRL_CLKSOURCE)
	{
		ticks=SystemCoreClock/1000*DelayMS;
		
	}else{
		ticks=SystemCoreClock/2/1000*DelayMS;
	}
	if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
  {
    return (1UL);                                                   /* Reload value impossible */
  }
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = (SysTick_CTRL_CLKSOURCE<<SysTick_CTRL_CLKSOURCE_Pos)	|    
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
	do
	{
		countFlag=SysTick->CTRL;
		
	}while(countFlag&1UL&&!(countFlag&(1<<16)));
	
	SysTick->CTRL  = 0UL;
	SysTick->VAL   = 0UL;   
	return 0;
}

void Delay_S(uint32_t DelayS)
{
	uint32_t i;
	for(i=0;i<DelayS;i++)
	{
		Delay_ms(1000);
	}
	
}


/*********************************************************************
**延时函数不使用SysTick
*
*********************************************************************/
void delay_us(uint32_t delayNUM)
{
	uint8_t i;
	while(delayNUM--)
	{
		i=2;
		while(i--);
		//__NOP();
	}
}

void delay_ms(uint32_t delayNUM)
{
	while(delayNUM--)
	{
		delay_us(1000);
	}
}
