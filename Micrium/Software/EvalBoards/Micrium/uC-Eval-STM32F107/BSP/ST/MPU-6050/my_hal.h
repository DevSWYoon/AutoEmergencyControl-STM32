#ifndef __MY_HAL_H
#define __MY_HAL_H

#include "includes.h"
#include "USR_INIT.h"
#include <stdint.h>

void BuzzerInit(void);

void BuzzerTmrCallback(void *p_tmr, void *p_arg);

void TimeoutPendingTmrCallback(void *p_tmr, void *p_arg);

void RelayInit(void);

void RelayCutOff(void);

#endif // __MY_HAL_H