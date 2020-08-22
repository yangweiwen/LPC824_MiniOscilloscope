#include "ADC.h"

#define ADC_CHANNEL_MIC 0
#define ADC_CHANNEL_VOLT 1
#define ADC_CHANNEL (ADC_CHANNEL_VOLT)

bool sequenceComplete, thresholdCrossed;
extern uint8_t DMA_IntA_Flag, DMA_IntB_Flag;
/*****************************************************************************
 * Board ADC functions
 ****************************************************************************/

//1 -- 1.2Msps -- 10  us
//2 -- 0.6Msps -- 20  us
//5 --         -- 50  us
//10           -- 100 us
//100          -- 1   ms
//200          -- 2   ms
//250          -- 2.5 ms
void Board_ADC1_SetSampleRate(uint32_t div)
{
    Chip_ADC_SetClockRateByDiv(LPC_ADC, div);
}

/**
*	ADC Configuration
**/
void Board_ADC1_Init(uint8_t channelSelect)
{
    Chip_ADC_Init(LPC_ADC, 0);
	/* Need to do a calibration after initialization and trim */
	Chip_ADC_StartCalibration(LPC_ADC);
	while (!(Chip_ADC_IsCalibrationDone(LPC_ADC))) {}

	/* Setup for maximum ADC clock rate using sycnchronous clocking */
	Chip_ADC_SetClockRate(LPC_ADC, ADC_MAX_SAMPLE_RATE);

	/* Optionally, you can setup the ADC to use asycnchronous clocking mode.
	   To enable this, mode use 'LPC_ADC->CTRL |= ADC_CR_ASYNMODE;'.
	   In asycnchronous clocking mode mode, the following functions are
	   used to set and determine ADC rates:
	   Chip_Clock_SetADCASYNCSource();
	   Chip_Clock_SetADCASYNCClockDiv();
	   Chip_Clock_GetADCASYNCRate();
	   clkRate = Chip_Clock_GetADCASYNCRate() / Chip_Clock_GetADCASYNCClockDiv; */

	/* Setup sequencer A for ADC channel 2, EOS interrupt */
	/* Setup a sequencer to do the following:
	   Perform ADC conversion of ADC channels 2 only */
    if(channelSelect == 0)
    {
    	Chip_ADC_SetupSequencer(LPC_ADC, ADC_SEQA_IDX,
    							(ADC_SEQ_CTRL_CHANSEL(BOARD_ADC_CH1) 
                                 |ADC_SEQ_CTRL_BURST));    
        /* Enable the clock to the Switch Matrix */
        Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
        /* Configure the SWM for P0-6 as the input for the ADC2 */
        Chip_SWM_EnableFixedPin(SWM_FIXED_ADC2);
    }
    else
    {
        Chip_ADC_SetupSequencer(LPC_ADC, ADC_SEQA_IDX,
    							(ADC_SEQ_CTRL_CHANSEL(BOARD_ADC_CH2) 
                                 |ADC_SEQ_CTRL_BURST));  
        /* Enable the clock to the Switch Matrix */
        Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);
        /* Configure the SWM for P0-6 as the input for the ADC2 */
        Chip_SWM_EnableFixedPin(SWM_FIXED_ADC9/*SWM_FIXED_ADC9*/);
    }

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);

	/* Clear all pending interrupts */
	Chip_ADC_ClearFlags(LPC_ADC, Chip_ADC_GetFlags(LPC_ADC));

	/* Enable ADC overrun and sequence A completion interrupts */
	Chip_ADC_EnableInt(LPC_ADC, ADC_INTEN_SEQA_ENABLE|ADC_INTEN_OVRRUN_ENABLE);

	/* Enable sequencer */
	Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX);
    Chip_ADC_StartSequencer(LPC_ADC, ADC_SEQA_IDX);
}

void Board_ADC1_Stop(void)
{
   // Chip_ADC_DeInit(LPC_ADC);
    Chip_ADC_DisableSequencer(LPC_ADC, ADC_SEQA_IDX);
    Chip_DMA_Disable(LPC_DMA);
}
void Board_ADC1_Start(void)
{
    Chip_DMA_Enable(LPC_DMA);
    Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX);
    Chip_ADC_StartSequencer(LPC_ADC, ADC_SEQA_IDX);
}
void Board_ADC2_Stop(void)
{
  //  Chip_ADC_DeInit(LPC_ADC);
    Chip_ADC_DisableSequencer(LPC_ADC, ADC_SEQA_IDX);
}
void Board_ADC2_Start(void)
{
    Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQA_IDX);
    Chip_ADC_StartSequencer(LPC_ADC, ADC_SEQA_IDX);
}

void Board_ADC2_Init(uint8_t channelSelect)
{
//	/* Setup ADC for 12-bit mode and normal power */
//	Chip_ADC_Init(LPC_ADC, 0);

//	/* Need to do a calibration after initialization and trim */
//	Chip_ADC_StartCalibration(LPC_ADC);
//	while (!(Chip_ADC_IsCalibrationDone(LPC_ADC))) {}

//	/* Setup for maximum ADC clock rate using sycnchronous clocking */
//	Chip_ADC_SetClockRate(LPC_ADC, ADC_MAX_SAMPLE_RATE);

	/* Setup sequencer A for ADC channel 2, EOS interrupt */
	/* Setup a sequencer to do the following:
	   Perform ADC conversion of ADC channels 2 only */

    if(channelSelect == 0)
    {
    	Chip_ADC_SetupSequencer(LPC_ADC, ADC_SEQB_IDX,
    							(ADC_SEQ_CTRL_CHANSEL(BOARD_ADC_CH2) | ADC_SEQ_CTRL_MODE_EOS));

    	/* Enable the clock to the Switch Matrix */
    	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    	/* Configure the SWM for P0-6 as the input for the ADC2 */
    	Chip_SWM_EnableFixedPin(SWM_FIXED_ADC9);
    }
    else
    {
        Chip_ADC_SetupSequencer(LPC_ADC, ADC_SEQB_IDX,
    							(ADC_SEQ_CTRL_CHANSEL(BOARD_ADC_CH1) | ADC_SEQ_CTRL_MODE_EOS));

    	/* Enable the clock to the Switch Matrix */
    	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_SWM);

    	/* Configure the SWM for P0-6 as the input for the ADC2 */
    	Chip_SWM_EnableFixedPin(SWM_FIXED_ADC2);
    }

	/* Disable the clock to the Switch Matrix to save power */
	Chip_Clock_DisablePeriphClock(SYSCTL_CLOCK_SWM);


	/* Setup threshold 0 low and high values to about 25% and 75% of max */
	//Chip_ADC_SetThrLowValue(LPC_ADC, 0, ((1 * 0xFFF) / 4));
	//Chip_ADC_SetThrHighValue(LPC_ADC, 0, ((3 * 0xFFF) / 4));

	/* Clear all pending interrupts */
	Chip_ADC_ClearFlags(LPC_ADC, Chip_ADC_GetFlags(LPC_ADC));

	/* Enable ADC overrun and sequence A completion interrupts */
	Chip_ADC_EnableInt(LPC_ADC, (ADC_INTEN_SEQB_ENABLE | ADC_INTEN_OVRRUN_ENABLE));

	/* Use threshold 0 for ADC channel and enable threshold interrupt mode for
	   channel as crossing */
	//Chip_ADC_SelectTH0Channels(LPC_ADC, ADC_THRSEL_CHAN_SEL_THR1(BOARD_ADC_CH1));
	//Chip_ADC_SetThresholdInt(LPC_ADC, BOARD_ADC_CH1, ADC_INTEN_THCMP_CROSSING);

	/* Enable ADC NVIC interrupt */
	NVIC_EnableIRQ(ADC_SEQB_IRQn);

	/* Enable sequencer */
	Chip_ADC_EnableSequencer(LPC_ADC, ADC_SEQB_IDX);

}

/**
 * @brief	Handle interrupt from ADC sequencer A
 * @return	Nothing
 */
void ADC_SEQB_IRQHandler(void)
{
	uint32_t pending;

	/* Get pending interrupts */
	pending = Chip_ADC_GetFlags(LPC_ADC);

	/* Sequence A completion interrupt */
	if (pending & ADC_FLAGS_SEQB_INT_MASK) {
		sequenceComplete = true;
	}

	/* Threshold crossing interrupt on ADC input channel */
	if (pending & ADC_FLAGS_THCMP_MASK(BOARD_ADC_CH1)) {
		thresholdCrossed = true;
	}

	/* Clear any pending interrupts */
	Chip_ADC_ClearFlags(LPC_ADC, pending);
}


//void ADC_THCMP_IRQHandler(void)
//{
//	uint32_t pending;

//	/* Get pending interrupts */
//	pending = Chip_ADC_GetFlags(LPC_ADC);

////	/* Sequence A completion interrupt */
////	if (pending & ADC_FLAGS_THCMP_INT_MASK) {
////		sequenceComplete = true;
////	}

//	/* Threshold crossing interrupt on ADC input channel */
//	if (pending & ADC_FLAGS_THCMP_MASK(BOARD_ADC_CH)) {
//		thresholdCrossed = true;
//	}

//	/* Clear any pending interrupts */
//	Chip_ADC_ClearFlags(LPC_ADC, pending);
//}

/***********************************************************************************/
/*DMA 
***********************************************************************************/
/* DMA completion flag */
static volatile bool dmaDone;

void DMA_ADC_Setup(uint32_t *buf,uint32_t length)
{
    uint32_t xfercount;
    
    xfercount = length - 1;

    /* DMA initialization - enable DMA clocking and reset DMA if needed */
	Chip_DMA_Init(LPC_DMA);

	/* DISable DMA controller and use driver provided DMA table for current descriptors */
    Chip_DMA_Disable(LPC_DMA);
	Chip_DMA_SetSRAMBase(LPC_DMA, DMA_ADDR(Chip_DMA_Table));

	/* Setup channel 0 for the following configuration:
	   - High channel priority
	   - Interrupt A fires on descriptor completion */
	Chip_DMA_SetupChannelConfig(LPC_DMA, DMA_CH1,
								(DMA_CFG_HWTRIGEN  | DMA_CFG_TRIGPOL_HIGH |DMA_CFG_TRIGTYPE_EDGE|
								 DMA_CFG_TRIGBURST_BURST | DMA_CFG_BURSTPOWER_1 ));
    
    Chip_DMA_SetTranBits(LPC_DMA, DMA_CH1,
                         (DMA_XFERCFG_CFGVALID | DMA_XFERCFG_RELOAD | DMA_XFERCFG_CLRTRIG | 
                        DMA_XFERCFG_SETINTA | DMA_XFERCFG_WIDTH_32 | DMA_XFERCFG_DSTINC_1|
                        DMA_XFERCFG_SRCINC_0|DMA_XFERCFG_XFERCOUNT(length))   
                        );
		/* DMA descriptor for memory to memory operation - note that addresses must
	   be the END address for src and destination, not the starting address.
	     DMA operations moves from end to start. */
         
    Chip_DMA_Table[DMA_CH1].source = (uint32_t)(LPC_ADC->SEQ_GDAT);     
	Chip_DMA_Table[DMA_CH1].dest = DMA_ADDR(&buf[xfercount]);
	Chip_DMA_Table[DMA_CH1].next = (uint32_t)&Descriptor_B;; 
  
    xfercount = length - 1;
    Descriptor_B.xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_RELOAD | DMA_XFERCFG_CLRTRIG |
                            DMA_XFERCFG_SETINTB |DMA_XFERCFG_WIDTH_32 |DMA_XFERCFG_DSTINC_1 
                            |DMA_XFERCFG_XFERCOUNT(length);


    Descriptor_B.source = (uint32_t)(&LPC_ADC->SEQ_GDAT);
    Descriptor_B.dest = (uint32_t)(&buf[xfercount+length]); 
    Descriptor_B.next = (uint32_t)&Descriptor_A; 
    
    xfercount = length - 1; 
    Descriptor_A.xfercfg = DMA_XFERCFG_CFGVALID | DMA_XFERCFG_RELOAD | DMA_XFERCFG_CLRTRIG |
                            DMA_XFERCFG_SETINTA |DMA_XFERCFG_WIDTH_32 |DMA_XFERCFG_DSTINC_1 
                            |DMA_XFERCFG_XFERCOUNT(length);
    Descriptor_A.source = (uint32_t)(&LPC_ADC->SEQ_GDAT);
    Descriptor_A.dest = (uint32_t)(&buf[xfercount]); 
    Descriptor_A.next = (uint32_t)&Descriptor_B; 
    
    Chip_DMA_EnableIntChannel(LPC_DMA,DMA_CH1);
    Chip_DMA_EnableChannel(LPC_DMA,DMA_CH1);
    Chip_DMA_Enable(LPC_DMA);
    Chip_DMATRIGMUX_SetInputTrig(LPC_DMATRIGMUX,DMA_CH1,DMATRIG_ADC_SEQA_IRQ);
    
    /* Enable DMA interrupt */
	NVIC_EnableIRQ(DMA_IRQn);
	
}


/**
 * @brief	DMA Interrupt Handler
 * @return	None
 */
void DMA_IRQHandler(void)
{
	/* Error interrupt on channel 0? */
	if (Chip_DMA_GetErrorIntChannels(LPC_DMA) != 0) {
		/* This shouldn't happen for this simple DMA example, so set the LED
		   to indicate an error occurred. This is the correct method to clear
		   an abort. */
//		Chip_DMA_DisableChannel(LPC_DMA, DMA_CH1);
//		while ((Chip_DMA_GetBusyChannels(LPC_DMA) & (1 << DMA_CH1)) != 0) {}
//		Chip_DMA_AbortChannel(LPC_DMA, DMA_CH1);
		Chip_DMA_ClearErrorIntChannel(LPC_DMA, DMA_CH1);
//		Chip_DMA_EnableChannel(LPC_DMA, DMA_CH1);
	}
    if(Chip_DMA_GetActiveIntAChannels(LPC_DMA) != 0)
    {
       /* Clear DMA interrupt for the channel */
        Chip_DMA_ClearActiveIntAChannel(LPC_DMA, DMA_CH1);
        
        DMA_IntA_Flag=1;
        Chip_ADC_DisableSequencer(LPC_ADC, ADC_SEQA_IDX);
    }
    if(Chip_DMA_GetActiveIntBChannels(LPC_DMA) != 0)
    {
        Chip_DMA_ClearActiveIntBChannel(LPC_DMA, DMA_CH1);
        DMA_IntB_Flag=1;
        Chip_ADC_DisableSequencer(LPC_ADC, ADC_SEQA_IDX);
    }

	dmaDone = true;
}

