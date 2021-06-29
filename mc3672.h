#ifndef __MC3672_H_
#define __MC3672_H_

#include "transfer_handler.h"

typedef enum
{
    MC36XX_MODE_SLEEP      = 0,
    MC36XX_MODE_STANDBY    = 1,
    MC36XX_MODE_SNIFF      = 2,
    MC36XX_MODE_CWAKE      = 5,
    MC36XX_MODE_TRIG       = 7
}   MC36XX_mode_t;

typedef enum
{
    MC36XX_RANGE_2G    = 0,
    MC36XX_RANGE_4G    = 1,
    MC36XX_RANGE_8G    = 2,
    MC36XX_RANGE_16G   = 3,
    MC36XX_RANGE_12G   = 4,
    MC36XX_RANGE_END
}   MC36XX_range_t;

typedef enum
{
    MC36XX_RESOLUTION_6BIT    = 0,
    MC36XX_RESOLUTION_7BIT    = 1,
    MC36XX_RESOLUTION_8BIT    = 2,
    MC36XX_RESOLUTION_10BIT   = 3,
    MC36XX_RESOLUTION_12BIT   = 4,
    MC36XX_RESOLUTION_14BIT   = 5,  //(Do not select if FIFO enabled)
    MC36XX_RESOLUTION_END
}   MC36XX_resolution_t;

typedef enum
{
    MC36XX_CWAKE_SR_DEFAULT_54Hz = 0,
    MC36XX_CWAKE_SR_14Hz         = 5,
    MC36XX_CWAKE_SR_28Hz         = 6,
    MC36XX_CWAKE_SR_54Hz         = 7,
    MC36XX_CWAKE_SR_105Hz        = 8,
    MC36XX_CWAKE_SR_210Hz        = 9,
    MC36XX_CWAKE_SR_400Hz        = 10,
    MC36XX_CWAKE_SR_600Hz        = 11,
    MC36XX_CWAKE_SR_END
}   MC36XX_cwake_sr_t;

bool mc3672_begin(void);
void mc3672_sleep(void);

int16_t mc3672_readXAccel(void);
int16_t mc3672_readYAccel(void);
int16_t mc3672_readZAccel(void);

#endif
