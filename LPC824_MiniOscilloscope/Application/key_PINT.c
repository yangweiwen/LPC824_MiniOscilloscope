#include "key_PINT.h"
#include "board.h"

uint8_t PININT_KEY[5]={10,16,26,27,11};  
bool keyIntState[5];

void PinINTConfig(void)
{
    uint8_t i;
	Chip_PININT_Init(LPC_PININT);
    for(i=0;i<5;i++)
    {
        Chip_SYSCTL_SetPinInterrupt(i, PININT_KEY[i]);  
        /* Configure GPIO pin as input pin */
        Chip_GPIO_PinSetDIR(LPC_GPIO_PORT, 0, PININT_KEY[i], 0);   

    }

	/* Configure channel 7 interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH0);
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH0);
 
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH1);
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH1);
    
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH2);
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH2);
    
	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH3);
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH3);    

	Chip_PININT_SetPinModeEdge(LPC_PININT, PININTCH4);
	Chip_PININT_EnableIntLow(LPC_PININT, PININTCH4);      
    
	/* Enable interrupt in the NVIC */
	NVIC_EnableIRQ(PININT0_IRQn);
    NVIC_EnableIRQ(PININT1_IRQn);
    NVIC_EnableIRQ(PININT2_IRQn);
    NVIC_EnableIRQ(PININT3_IRQn);
    NVIC_EnableIRQ(PININT4_IRQn);
    
	// <<<
    for(i=0;i<5;i++)
    {    
        keyIntState[i]=0;
    }
}

void PININT0_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH0);
	keyIntState[0] = 1;
}

void PININT1_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH1);
	keyIntState[1] = 1;
}
void PININT2_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH2);
	keyIntState[2] = 1;
}
void PININT3_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH3);
	keyIntState[3] = 1;
}
void PININT4_IRQHandler(void)
{
	Chip_PININT_ClearIntStatus(LPC_PININT, PININTCH4);
	keyIntState[4] = 1;
}

void PinINT_PatternMatchConfig(void)
{
	Chip_PININT_EnablePatternMatch(LPC_PININT);
	
	Chip_PININT_SetPatternMatchSrc(LPC_PININT, 1, PININTBITSLICE0);
	Chip_PININT_SetPatternMatchSrc(LPC_PININT, 3, PININTBITSLICE1);
	Chip_PININT_SetPatternMatchSrc(LPC_PININT, 3, PININTBITSLICE2);
	Chip_PININT_SetPatternMatchSrc(LPC_PININT, 5, PININTBITSLICE3);
	Chip_PININT_SetPatternMatchSrc(LPC_PININT, 5, PININTBITSLICE4);
	Chip_PININT_SetPatternMatchSrc(LPC_PININT, 1, PININTBITSLICE5);

	Chip_PININT_SetPatternMatchConfig(LPC_PININT, PININTBITSLICE1, PININT_PATTERNHIGH, false);
	Chip_PININT_SetPatternMatchConfig(LPC_PININT, PININTBITSLICE3, PININT_PATTERNHIGH, true);
	Chip_PININT_SetPatternMatchConfig(LPC_PININT, PININTBITSLICE3, PININT_PATTERNHIGH, false);
	Chip_PININT_SetPatternMatchConfig(LPC_PININT, PININTBITSLICE5, PININT_PATTERNHIGH, true);
	Chip_PININT_SetPatternMatchConfig(LPC_PININT, PININTBITSLICE5, PININT_PATTERNHIGH, false);
	Chip_PININT_SetPatternMatchConfig(LPC_PININT, PININTBITSLICE1, PININT_PATTERNHIGH, true);	
}

uint8_t PinGetButtonState(uint8_t index)
{
    if(index >= 5)
    {
        return 0xFF;
    }

    return Chip_GPIO_PinGetState(LPC_GPIO_PORT, 0, PININT_KEY[index]);
}

