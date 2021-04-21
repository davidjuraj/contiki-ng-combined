#include "contiki.h"
#include "contiki-net.h"
#include "coap-engine.h"

/*---------------------------------------------------------------------------*/
#define DEBUG DEBUG_NONE
#include "net/ipv6/uip-debug.h"
/*---------------------------------------------------------------------------*/

#if !NETSTACK_CONF_WITH_IPV6 || !UIP_CONF_ROUTER || !UIP_IPV6_MULTICAST || !UIP_CONF_IPV6_RPL
#error "This example can not work with the current contiki configuration"
#error "Check the values of: NETSTACK_CONF_WITH_IPV6, UIP_CONF_ROUTER, UIP_CONF_IPV6_RPL"
#endif

extern coap_resource_t pwm_light;
void pwm_init_handler();

PROCESS(pwm_multicast, "pwm multicast");
AUTOSTART_PROCESSES(&pwm_multicast);
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

  //Initialize lamp PWM control
  pwm_init_handler();

  //Join multicast group for CoAP group communication
  if(join_mcast_group() == NULL) {
    PRINTF("Failed to join multicast group\n");
    PROCESS_EXIT();
  }

  //Activate CoAP resource
  coap_activate_resource(&pwm_light, "lamp/dimming");

  PROCESS_END();

}
