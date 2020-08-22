#ifndef _DELAY_H
#define	_DELAY_H

#include "chip.h"
//#define SysTick_CLKSOURCE_CORE	
#ifdef SysTick_CLKSOURCE_CORE
	#define SysTick_CTRL_CLKSOURCE 1	//SysTick时钟源 CLKSOURCE_CORE
#else
	#define SysTick_CTRL_CLKSOURCE 0	//SysTick时钟源 CLKSOURCE_CORE/2
#endif	//endif SysTick_CLKSOURCE_CORE

typedef enum _SysTick_CLKSOURCE{
	SYSTEM_CLOCK,
	SYSTEM_CLOCK_2,
}SysTickClkSource;


uint32_t Delay_ms(uint32_t DelayMS);
uint32_t Delay_us(uint32_t DelayUS);
void Delay_S(uint32_t DelayS);
void delay_us(uint32_t delayNUM);
void delay_ms(uint32_t delayNUM);

#endif //endif _DELAY_H
