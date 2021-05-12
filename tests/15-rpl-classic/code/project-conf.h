/*
 * Copyright (c) 2016, Inria.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

// #define TCPIP_CONF_ANNOTATE_TRANSMISSIONS 1 //hereeeeeeeeeeee comment
#define LOG_LEVEL_CONF_MAC LOG_LEVEL_DBG

//#define ZOUL_CONF_USE_CC1200_RADIO 1
#undef IEEE802154_CONF_DEFAULT_CHANNEL
#define IEEE802154_CONF_DEFAULT_CHANNEL 17

#include "stdint.h"

//#define LOG_CONF_LEVEL_NULLNET LOG_LEVEL_INFO
#define ENERGEST_CONF_ON 0      // set this to 0 to also not sned to the energest-sink
#define ENERGEST_FREQ 100       // every x messages a message is sent to the energest sink

#define BC 0 // Broadcast test or not

#define IO_WIDTH 11

// UART pins are used for parallel communication, serial comm over UART overwrites some pins
#define UART_CONF_ENABLE 0
#define USB_SERIAL_CONF_ENABLE 0

#define LOG_CONF_LEVEL_RPL LOG_LEVEL_DBG

/* Data structure of messages sent from sender
 *
 */
struct testmsg {       
	uint16_t  seqno;
	uint16_t  timestamp_app;
    uint16_t  timestamp_mac;
};

struct energestmsg {
    uint32_t    cpu;
    uint32_t    lpm;
    uint32_t    transmit;
    uint32_t    listen;
    uint16_t    seqno;
    uint32_t    totaltime;
};

#endif //PROJECT_CONF_H_
