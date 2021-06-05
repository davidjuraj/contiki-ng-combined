/*
 * Copyright (c) 2010, Loughborough University - Computer Science
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
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *         Project specific configuration defines for the RPl multicast
 *         example.
 *
 * \author
 *         George Oikonomou - <oikonomou@users.sourceforge.net>
 */
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#include "stdint.h"
#include "net/ipv6/multicast/uip-mcast6-engines.h"

/* Change this to switch engines. Engine codes in uip-mcast6-engines.h */
#define UIP_MCAST6_CONF_ENGINE	UIP_MCAST6_ENGINE_BMRF

/* Change this to switch between unicast mode and broadcast mode*/
//#define BMRF_CONF_MODE	BMRF_UNICAST_MODE
#define BMRF_CONF_MODE	BMRF_BROADCAST_MODE
//#define BMRF_CONF_MODE	BMRF_MIXED_MODE
//#define BMRF_CONF_BROADCAST_THRESHOLD	1

/* Enable the orchestra DOWNWARDS BROADCAST instead of general orchestra broadcast */
//#define ORCHESTRA_DOWNWARDS_BROADCAST

/* Change the length of the broadcast slotframe */
//#define ORCHESTRA_CONF_BROADCAST_PERIOD 23

/* Change the length of the unicast slotframe */
//#define ORCHESTRA_CONF_UNICAST_PERIOD 17

/* Code/RAM footprint savings so that things will fit on our device*/
#ifndef NETSTACK_MAX_ROUTE_ENTRIES
#define NETSTACK_MAX_ROUTE_ENTRIES   15
#endif

#ifndef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 15
#endif


#define MCAST_CONF_SEND_INTERVAL 3
#define MCAST_CONF_MESSAGES 100
#define MCAST_CONF_START_DELAY 10 //240

#define IO_WIDTH 11

// UART pins are used for parallel communication, serial comm over UART overwrites some pins
#define UART_CONF_ENABLE 1
#define USB_SERIAL_CONF_ENABLE 1

/* Get some extra RAM */
#define LPM_CONF_MAX_PM             1

#define LOG_CONF_LEVEL_MAC LOG_LEVEL_DBG
#define LOG_CONF_LEVEL_RPL LOG_LEVEL_DBG



/* Data structure of messages sent from sender
 *
 */
struct testmsg {       
	uint16_t  seqno;
	uint16_t  timestamp_app;
    uint16_t  timestamp_mac;
};

#endif /* PROJECT_CONF_H_ */
