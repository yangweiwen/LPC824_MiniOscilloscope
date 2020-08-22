#ifndef _KEY_PINT_H
#define _KEY_PINT_H
#include "chip.h"



extern bool keyIntState[5];
void PinINTConfig(void);
void PinINT_PatternMatchConfig(void);
uint8_t PinGetButtonState(uint8_t index);

#endif
