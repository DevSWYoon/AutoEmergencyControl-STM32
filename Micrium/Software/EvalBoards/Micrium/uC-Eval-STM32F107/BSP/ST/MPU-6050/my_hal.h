#ifndef __MY_HAL_H
#define __MY_HAL_H

#include "includes.h"
#include "USR_INIT.h"
#include <stdint.h>

void BuzzerInit(void);

void BuzzerDisInit(void);

void RelayInit(void);

void RelayCutOff(void);

void RelayTurnOn(void);

bool GetRelayStat(void);

#endif // __MY_HAL_H