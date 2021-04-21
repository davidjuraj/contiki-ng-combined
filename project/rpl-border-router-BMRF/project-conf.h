/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
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
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

//#define ROUTING_CONF_RPL_CLASSIC 1


//Use Zolertia Re-Mote second USB port for direct USB communication instead of serial
//#define SLIP_ARCH_CONF_USB 1
//#define DBG_CONF_USB 1

//#define LOG_CONF_LEVEL_RPL LOG_LEVEL_DBG
//#define LOG_CONF_LEVEL_IPV6 LOG_LEVEL_DBG


#include "net/ipv6/multicast/slip-multicast.h"  //Enables custom slip input handler
#include "net/ipv6/multicast/uip-mcast6-engines.h"

//#define BMRF_NETSTACK_SNIFFER 1

/* Change this to switch engines. Engine codes in uip-mcast6-engines.h */
#define UIP_MCAST6_CONF_ENGINE	UIP_MCAST6_ENGINE_BMRF
#define BMRF_CONF_MODE	BMRF_UNICAST_MODE

//#define BMRF_CONF_MODE	BMRF_BROADCAST_MODE
//#define BMRF_CONF_MODE	BMRF_MIXED_MODE
//#define BMRF_CONF_BROADCAST_THRESHOLD	1

#define UIP_MCAST6_ROUTE_CONF_ROUTES 20
#define CSMA_CONF_MAX_NEIGHBOR_QUEUES UIP_MCAST6_ROUTE_CONF_ROUTES

/* Code/RAM footprint savings so that things will fit on our device */
#ifndef NETSTACK_MAX_ROUTE_ENTRIES
#define NETSTACK_MAX_ROUTE_ENTRIES   20
#endif

#ifndef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 20
#endif

//#define LOG_CONF_LEVEL_MAC                         LOG_LEVEL_DBG
//#define CC2538_RF_CONF_TX_POWER 		0xFF  //Default 0xd5: +3dBm,  0xff: +7dBm


//#define SICSLOWPAN_CONF_FRAG 0
#define QUEUEBUF_CONF_NUM 20

//#define UIP_CONF_BUFFER_SIZE 128

//#define TSCH_CONF_ASSOCIATION_POLL_FREQUENCY 20
//#define TSCH_CONF_CHANNEL_SCAN_DURATION CLOCK_SECOND*5



#define LPM_CONF_ENABLE 0  //Absolute must for enabling 32KB RAM!

//#define SLIP_CONF_WITH_STATS 0


#ifndef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2
#endif

#ifndef BORDER_ROUTER_CONF_WEBSERVER
#define BORDER_ROUTER_CONF_WEBSERVER 1
#endif

#if BORDER_ROUTER_CONF_WEBSERVER
#define UIP_CONF_TCP 1
#endif


#define CC2538_RF_CONF_TX_POWER 		0xFF  //Default 0xd5: +3dBm,  0xff: +7dBm

//#define CC2538_RF_CONF_CHANNEL              26

#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED          1

#define CSMA_CONF_LLSEC_DEFAULT_KEY0 { 0xAA , 0xBB , 0xCC , 0xDD , 0xEE , 0xFF , 0x00 , 0x11 , 0x22 , 0x33 , 0x44 , 0x55 , 0x66 , 0x77 , 0x88 , 0x99 }


#endif /* PROJECT_CONF_H_ */
