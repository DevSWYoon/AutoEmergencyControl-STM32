#include "includes.h"
#include <stdint.h>

void BuzzerInit(void);

void BuzzerTmrCallback(void *p_tmr, void *p_arg);

void TimeoutPendingTmrCallback(void *p_tmr, void *p_arg);

void RelayInit(void);

void RelayCutOff(void);
