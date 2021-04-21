#include "contiki.h"
//#include "cpu.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
//#include "dev/sys-ctrl.h"
//#include "pwm.h"
//#include "lpm.h"
//#include "dev/ioc.h"
#include "contiki-net.h"
#include "net/ipv6/uip.h"
#include "net/packetbuf.h"
/*---------------------------------------------------------------------------*/
#define DEBUG DEBUG_PRINT
#include "net/ipv6/uip-debug.h"
/*---------------------------------------------------------------------------*/

#define SERVER_REPLY 0
#define UDP_CLIENT_PORT	0  		//MUST BE 0 TO ALLOW UDP PACKETS WITH RANDOM SRC PORT
#define UDP_SERVER_PORT	6000	//LISTENING ON THIS UDP DEST PORT

#define RESULTS_RETRIEVE 0
#define RESULTS_RESET 1

static struct uip_udp_conn *server_conn;
//static pwm_t ledDriverPWM;

static uint16_t dutycycle; //in promille

#define UDP_SERVER_PORT_RESULTS 666
static struct uip_udp_conn *server_conn_results;
#define uip_lladdr_cmp(addr1, addr2) (memcmp(addr1, addr2, UIP_LLADDR_LEN) == 0)

struct receivedMulticastStats {
	uint16_t rxbroadcast;
	uint16_t rxunicast;
} stats;


#if !NETSTACK_CONF_WITH_IPV6 || !UIP_CONF_ROUTER || !UIP_IPV6_MULTICAST || !UIP_CONF_IPV6_RPL
#error "This example can not work with the current contiki configuration"
#error "Check the values of: NETSTACK_CONF_WITH_IPV6, UIP_CONF_ROUTER, UIP_CONF_IPV6_RPL"
#endif

PROCESS(pwm_multicast, "pwm multicast");
AUTOSTART_PROCESSES(&pwm_multicast);
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  if(uip_newdata()) {

	if(UIP_UDP_BUF->destport  == UIP_HTONS(UDP_SERVER_PORT_RESULTS)) {

		uint8_t* status = (uint8_t *)uip_appdata;
		if(*status == RESULTS_RETRIEVE) {
			PRINTF("Return results LL broadcasts: %u , LL unicasts: %u and clear counters.\n", stats.rxbroadcast, stats.rxunicast);
			uip_udp_packet_sendto(server_conn_results, &stats, sizeof(stats), &UIP_IP_BUF->srcipaddr, UIP_UDP_BUF->srcport);
		} else if (*status == RESULTS_RESET) {
			stats.rxbroadcast = 0;
			stats.rxunicast = 0;
			PRINTF("Reset results LL broadcasts: %u , LL unicasts: %u and clear counters.\n", stats.rxbroadcast, stats.rxunicast);
			uip_udp_packet_sendto(server_conn_results, &stats, sizeof(stats), &UIP_IP_BUF->srcipaddr, UIP_UDP_BUF->srcport);
		}

	} else{
		uint16_t* newdutycycle = (uint16_t *)uip_appdata; //We use promille instead of %

		//PRINTF("Recv duty cycle %u promille from ", *newdutycycle);
		//PRINT6ADDR(&UIP_UDPIP_BUF->srcipaddr);
		//PRINTF(" on port %u\n", UIP_HTONS(UIP_UDPIP_BUF->destport));

		if(uip_lladdr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER), &linkaddr_null)) {
			stats.rxbroadcast += 1;
			PRINTF("Received LL broadcast. Counter %u\n", stats.rxbroadcast);
		} else {
			stats.rxunicast += 1;
			PRINTF("Received LL unicast. Counter %u\n", stats.rxunicast);
		}

		if(*newdutycycle <= 1000) {
			dutycycle = *newdutycycle;
			//pwm_startdutycycle(&ledDriverPWM, dutycycle);
			//PRINTF("Set PWM on timer %u/%u to duty cycle %u promille\n",ledDriverPWM.timer, ledDriverPWM.ab, dutycycle);
			PRINTF("Set PWM to duty cycle %u promille\n", dutycycle);
		}
	  }

  }
}


/*---------------------------------------------------------------------------*/
static uip_ds6_maddr_t *
join_mcast_group(void)
{
  uip_ipaddr_t addr;
  uip_ds6_maddr_t *rv;

  uip_ip6addr(&addr, 0xFF0E,0,0,0,0,0,0,0xBABE);
  rv = uip_ds6_maddr_add(&addr);
  if(rv) {
    PRINTF("Joined multicast group ");
    PRINT6ADDR(&uip_ds6_maddr_lookup(&addr)->ipaddr);
    PRINTF("\n");
  }
  return rv;
}
/*---------------------------------------------------------------------------*/
//static void
//unjoin_mcast_group(void){
//  uip_ipaddr_t addr;
//  uip_ds6_maddr_t *mcast_addrs;
//  uip_ip6addr(&addr, 0xFF0E,0,0,0,0,0,0,0xBABE);
//  mcast_addrs = uip_ds6_maddr_lookup(&addr);
//  uip_ds6_maddr_rm(mcast_addrs);
//  PRINTF("Unsubscribing from multicast group\n");
//}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(pwm_multicast, ev, data)
{
  PROCESS_BEGIN();

//  pwm_configure(&ledDriverPWM, PWM_TIMER_2, PWM_TIMER_B, 1024, GPIO_C_NUM, 3, PWM_ON_WHEN_STOP);
  //PRINTF("\nConfiguring PWM on timer %u/%d to frequency %lu Hz\n", ledDriverPWM.timer, ledDriverPWM.ab, ledDriverPWM.freq);

  dutycycle = 500;
//  pwm_startdutycycle(&ledDriverPWM, dutycycle);
  //PRINTF("Set PWM on timer %u/%u to duty cycle %u promille\n",ledDriverPWM.timer, ledDriverPWM.ab, dutycycle);

  if(join_mcast_group() == NULL) {
    PRINTF("Failed to join multicast group\n");
    PROCESS_EXIT();
  }


  PRINTF("UDP server started. nbr:%d routes:%d\n",
         NBR_TABLE_CONF_MAX_NEIGHBORS, UIP_MAX_ROUTES);

  server_conn = udp_new(NULL, UIP_HTONS(UDP_CLIENT_PORT), NULL);
  if(server_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(server_conn, UIP_HTONS(UDP_SERVER_PORT));

  PRINTF("Created a server connection with remote address ");
  PRINT6ADDR(&server_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n", UIP_HTONS(server_conn->lport),
         UIP_HTONS(server_conn->rport));


  server_conn_results = udp_new(NULL, UIP_HTONS(UDP_CLIENT_PORT), NULL);
  if(server_conn_results == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(server_conn_results, UIP_HTONS(UDP_SERVER_PORT_RESULTS));

  PRINTF("Created a server connection for results with remote address ");
  PRINT6ADDR(&server_conn_results->ripaddr);
  PRINTF(" local/remote port %u/%u\n", UIP_HTONS(server_conn_results->lport),
         UIP_HTONS(server_conn_results->rport));



  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }

  PROCESS_END();

}
