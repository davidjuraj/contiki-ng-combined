#include "slip-multicast.h"
#include "contiki-net.h"
#include "net/packetbuf.h"

#define DEBUG DEBUG_NONE
#include "net/ipv6/uip-debug.h"



#if LINKADDR_SIZE == 2
const linkaddr_t linkaddr_slip = { { 0xFF, 0xFF } };
#else /*LINKADDR_SIZE == 2*/
#if LINKADDR_SIZE == 8
const linkaddr_t linkaddr_slip = { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
#endif /*LINKADDR_SIZE == 8*/
#if LINKADDR_SIZE == 6
const linkaddr_t linkaddr_slip = { { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
#endif /*LINKADDR_SIZE == 6*/
#endif /*LINKADDR_SIZE == 2*/


#ifdef UIP_FALLBACK_INTERFACE
extern struct uip_fallback_interface UIP_FALLBACK_INTERFACE;

void slip_input_multicast(void){
	PRINTF("SLIPmulticast: Adjusting LL parameters on packet received from SLIP.");
	packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_slip);
	packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &linkaddr_slip);
	tcpip_input();
}

void slip_output_multicast(void){
	PRINTF("SLIPmulticast: Sending multicast from WSN to SLIP.\n");
	UIP_FALLBACK_INTERFACE.output();
}
#endif

