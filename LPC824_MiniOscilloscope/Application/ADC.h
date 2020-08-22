#ifndef _ADC_H
#define _ADC_H
#include "chip.h"

#define BOARD_ADC_CH1 2
#define BOARD_ADC_CH2 9
#define BOARD_ADC_CH3 10

void Board_ADC1_Init(unsigned char channelSelect);
void Board_ADC2_Init(unsigned char channelSelect);
void DMA_ADC_Setup(uint32_t *buf,uint32_t length);

void Board_ADC1_Stop(void);
void Board_ADC1_Start(void);
void Board_ADC2_Stop(void);
void Board_ADC2_Start(void);
void Board_ADC1_SetSampleRate(uint32_t div);

#endif
