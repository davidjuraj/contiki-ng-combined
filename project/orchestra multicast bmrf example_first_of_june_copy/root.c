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
 *         This node is part of the RPL multicast example. It is an RPL root
 *         and sends a multicast message periodically. For the example to work,
 *         we need one of those nodes.
 *
 * \author
 *         George Oikonomou - <oikonomou@users.sourceforge.net>
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ipv6/multicast/uip-mcast6.h"

#include "net/ipv6/multicast/bmrf.h"

#include <string.h>
#include <inttypes.h>

#define DEBUG DEBUG_PRINT
#include "net/ipv6/uip-debug.h"

#include "net/routing/routing.h"

#include "lib/random.h"
#include <math.h>

#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-debug.h"
#include "project-conf.h"

#include "simple-udp.h"

#include "dev/gpio.h"
#include "dev/ioc.h"

#ifdef MCAST_CONF_SEND_INTERVAL
#define SEND_INTERVAL MCAST_CONF_SEND_INTERVAL * CLOCK_SECOND /* clock ticks */
#else
#define SEND_INTERVAL 3*CLOCK_SECOND
#endif
#ifdef MCAST_CONF_MESSAGES
#define ITERATIONS MCAST_CONF_MESSAGES /* messages */
#else
#define ITERATIONS 100
#endif

#define MAX_PAYLOAD_LEN 120
#define MCAST_SINK_UDP_PORT 3001 /* Host byte order */

/* Start sending messages START_DELAY seconds after we start so that routing can converge */
#ifdef MCAST_CONF_START_DELAY
#define START_DELAY MCAST_CONF_START_DELAY
#else
#define START_DELAY 180
#endif

static struct uip_udp_conn * mcast_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint32_t seq_id;
uint16_t seqno=0;

#if !NETSTACK_CONF_WITH_IPV6 || !UIP_CONF_ROUTER || !UIP_IPV6_MULTICAST || !UIP_CONF_IPV6_RPL
#error "This example can not work with the current contiki configuration"
#error "Check the values of: NETSTACK_CONF_WITH_IPV6, UIP_CONF_ROUTER, UIP_CONF_IPV6_RPL"
#endif
/*---------------------------------------------------------------------------*/
PROCESS(rpl_root_process, "RPL ROOT, Multicast Sender");
AUTOSTART_PROCESSES(&rpl_root_process);

/*---------------------------------------------------------------------------*/
void
GPIOS_init(void)
{

	  ioc_set_over(0, 6, IOC_OVERRIDE_OE);
    ioc_set_over(0, 7, IOC_OVERRIDE_OE);
    ioc_set_over(2, 0, IOC_OVERRIDE_OE);
    ioc_set_over(2, 1, IOC_OVERRIDE_OE);
    ioc_set_over(2, 2, IOC_OVERRIDE_OE);
    ioc_set_over(2, 3, IOC_OVERRIDE_OE);
    ioc_set_over(2, 4, IOC_OVERRIDE_OE);
    ioc_set_over(2, 5, IOC_OVERRIDE_OE);
    ioc_set_over(2, 6, IOC_OVERRIDE_OE);
    ioc_set_over(3, 0, IOC_OVERRIDE_OE);
    ioc_set_over(3, 1, IOC_OVERRIDE_OE);
    ioc_set_over(3, 2, IOC_OVERRIDE_OE);

	  GPIO_SOFTWARE_CONTROL(GPIO_A_BASE,GPIO_PIN_MASK(6));
    GPIO_SOFTWARE_CONTROL(GPIO_A_BASE,GPIO_PIN_MASK(7));
	  GPIO_SET_OUTPUT(GPIO_A_BASE,GPIO_PIN_MASK(6));		//GPIO PA6
	  GPIO_SET_OUTPUT(GPIO_A_BASE,GPIO_PIN_MASK(7));		//GPIO PA7

    GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(0));		//GPIO PC0
	  GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(1));		//GPIO PC1
    GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(2));		//GPIO PC2
    GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(3));		//GPIO PC3
	  GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(4));		//GPIO PC4
	  GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(5));		//GPIO PC5
    GPIO_SOFTWARE_CONTROL(GPIO_C_BASE,GPIO_PIN_MASK(6));		//GPIO PC6
  
 	  GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(0));		//GPIO PC0
	  GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(1));		//GPIO PC1
    GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(2));		//GPIO PC2
    GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(3));		//GPIO PC3
	  GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(4));		//GPIO PC4
	  GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(5));		//GPIO PC5
    GPIO_SET_OUTPUT(GPIO_C_BASE,GPIO_PIN_MASK(6));		//GPIO PC6

    GPIO_SET_INPUT(GPIO_D_BASE,GPIO_PIN_MASK(0));		//GPIO PD0
    GPIO_SET_INPUT(GPIO_D_BASE,GPIO_PIN_MASK(1));		//GPIO PD1
	  GPIO_SET_INPUT(GPIO_D_BASE,GPIO_PIN_MASK(2));		//GPIO PD2

	  GPIO_SET_OUTPUT(GPIO_D_BASE,GPIO_PIN_MASK(0));		//GPIO PD0
    GPIO_SET_OUTPUT(GPIO_D_BASE,GPIO_PIN_MASK(1));		//GPIO PD1
	  GPIO_SET_OUTPUT(GPIO_D_BASE,GPIO_PIN_MASK(2));		//GPIO PD2

	  GPIO_CLR_PIN(GPIO_A_BASE,GPIO_PIN_MASK(6));		//GPIO PA6
}
/*---------------------------------------------------------------------------*/
void
clear_GPIOS(void)
{
	  GPIO_CLR_PIN(GPIO_A_BASE,GPIO_PIN_MASK(6));		//GPIO PA6
	
 	  GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(0));		//GPIO PC0
	  GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(1));		//GPIO PC1
  	GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(2));		//GPIO PC2
  	GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(3));		//GPIO PC3
  	GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(4));		//GPIO PC4
	  GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(5));		//GPIO PC5
  	GPIO_CLR_PIN(GPIO_C_BASE,GPIO_PIN_MASK(6));		//GPIO PC6

  	GPIO_CLR_PIN(GPIO_D_BASE,GPIO_PIN_MASK(0));		//GPIO PD0
  	GPIO_CLR_PIN(GPIO_D_BASE,GPIO_PIN_MASK(1));		//GPIO PD1
  	GPIO_CLR_PIN(GPIO_D_BASE,GPIO_PIN_MASK(2));		//GPIO PD2
}
/*---------------------------------------------------------------------------*/
int state = 0;
static void
multicast_send(void)
{

  struct testmsg msg;

	seqno++;

	/*Set general info*/
	msg.seqno=seqno;		
	msg.timestamp_app= clock_time();
	
	static uint8_t seqno_bits[IO_WIDTH];			
	uint8_t i;
	for (i = 0; i < IO_WIDTH; i++) {
		seqno_bits[i] = msg.seqno & (1 << i) ? 1 : 0;
	}		//least significant bit in seqno_bits[0]
	
	clear_GPIOS();
	
	if ( seqno_bits[0]==1 )		GPIO_SET_PIN(GPIO_A_BASE,GPIO_PIN_MASK(6));       //  write a 1 in A6
	if ( seqno_bits[1]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(0));       //  write a 1 in C0
	if ( seqno_bits[2]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(1));       //  write a 1 in C1
	if ( seqno_bits[3]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(2));       //  write a 1 in C2
	if ( seqno_bits[4]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(3));       //  write a 1 in C3
	if ( seqno_bits[5]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(4));       //  write a 1 in C4
	if ( seqno_bits[6]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(5));       //  write a 1 in C5
	if ( seqno_bits[7]==1 )		GPIO_SET_PIN(GPIO_C_BASE,GPIO_PIN_MASK(6));       //  write a 1 in C6
	if ( seqno_bits[8]==1 )		GPIO_SET_PIN(GPIO_D_BASE,GPIO_PIN_MASK(0));       //  write a 1 in D0
	if ( seqno_bits[9]==1 )		GPIO_SET_PIN(GPIO_D_BASE,GPIO_PIN_MASK(1));       //  write a 1 in D1
	if ( seqno_bits[10]==1 )	GPIO_SET_PIN(GPIO_D_BASE,GPIO_PIN_MASK(2));       //  write a 1 in D2

	if (state == 0){
		GPIO_SET_PIN(GPIO_A_BASE,GPIO_PIN_MASK(7));
		state = 1;
	}
	else{
		GPIO_CLR_PIN(GPIO_PORT_TO_BASE(0),GPIO_PIN_MASK(7));
		state = 0;
	}

  uint32_t id;

  id = uip_htonl(seq_id);
  memset(buf, 0, MAX_PAYLOAD_LEN);
  memcpy(buf, &id, sizeof(seq_id));

  PRINTF("Send to: ");
  PRINT6ADDR(&mcast_conn->ripaddr);
  PRINTF(" Remote Port %u,", uip_ntohs(mcast_conn->rport));
  PRINTF(" (msg=0x%08"PRIx32")", uip_ntohl(*((uint32_t *)buf)));
  PRINTF(" %lu bytes\n", (unsigned long)sizeof(id));

  seq_id++;
  uip_udp_packet_send(mcast_conn, buf, sizeof(id));
}
/*---------------------------------------------------------------------------*/
static void
prepare_mcast(void)
{
  uip_ipaddr_t ipaddr;

  /*
    Construct an IPv6 address from eight 16-bit words.
    This function constructs an IPv6 address.
  */
  uip_ip6addr(&ipaddr, 0xFF1E,0,0,0,0,0,0x89,0xABCD);


  /*
    Create a new UDP connection.
    This function creates a new UDP connection with the specified remote endpoint.
    Parameters
    ripaddr	 Pointer to the IP address of the remote host.
    port	 Port number in network byte order.
    appstate Pointer to application defined data.

    Returns
    A pointer to the newly created connection, or NULL if memory could not be allocated    
  */
  mcast_conn = udp_new(&ipaddr, UIP_HTONS(MCAST_SINK_UDP_PORT), NULL);
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rpl_root_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  GPIOS_init();

  PRINTF("Multicast Engine: '%s'\n", UIP_MCAST6.name);

  /*
  Initialize DAG root
  Set the node as root and start a network.
  */
 
  NETSTACK_ROUTING.root_start();

  prepare_mcast();

  etimer_set(&et, START_DELAY * CLOCK_SECOND);

  while(1) {
    PROCESS_YIELD();
    if(etimer_expired(&et)) {
      if(seq_id == ITERATIONS) {
        etimer_stop(&et);
      } else {
        multicast_send();
        float nextTime = (-3.000) * logf((random_rand() / (float)RANDOM_RAND_MAX));
        etimer_set(&et, nextTime*CLOCK_SECOND);
      }
    }
  }
  
  PROCESS_END();
}

