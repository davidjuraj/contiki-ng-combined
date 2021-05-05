#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#include "stdint.h"
#include "sys/rtimer.h"

//#define ZOUL_CONF_USE_CC1200_RADIO 1
#undef IEEE802154_CONF_DEFAULT_CHANNEL
#define IEEE802154_CONF_DEFAULT_CHANNEL 17

#define ENERGEST_FREQ 100       // every x messages a message is sent to the energest sink
#define IO_WIDTH 11

// UART pins are used for parallel communication, serial comm over UART overwrites some pins
#ifdef UART_CONF_ENABLE
#undef UART_CONF_ENABLE
#endif
#define UART_CONF_ENABLE 0

#include "stdint.h"

#endif // PROJECT_CONF_H_
