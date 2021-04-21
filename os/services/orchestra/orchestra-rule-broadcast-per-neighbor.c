/*
 * Copyright (c) 2015, Swedish Institute of Computer Science.
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
/**
 * \file
 *  Schedules a slotframe for LL broadcast transmission containing sender-based shared slots. 
 *  One broadcast-related transmitting slot and as many broadcast-related listening slots as the number of children.
 *
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki.h"
#include "orchestra.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/packetbuf.h"
#include "net/routing/routing.h"
#include "net/ipv6/uip-icmp6.h"

#define DEBUG DEBUG_PRINT
#include "net/ipv6/uip-debug.h"


/*
 * The body of this rule should be compiled only when "nbr_routes" is available,
 * otherwise a link error causes build failure. "nbr_routes" is compiled if
 * UIP_MAX_ROUTES != 0. See uip-ds6-route.c.
 */
#if UIP_MAX_ROUTES != 0

#if ORCHESTRA_COLLISION_FREE_HASH
#define BROADCAST_SLOT_SHARED_FLAG    ((ORCHESTRA_BROADCAST_PERIOD < (ORCHESTRA_MAX_HASH + 1)) ? LINK_OPTION_SHARED : 0)
#else
#define BROADCAST_SLOT_SHARED_FLAG      LINK_OPTION_SHARED
#endif

static uint16_t slotframe_handle = 0;
static uint16_t channel_offset = 0;
static struct tsch_slotframe *sf_broadcast;

/*---------------------------------------------------------------------------*/
static int
is_RPL_traffic()
{
  if(packetbuf_attr(PACKETBUF_ATTR_NETWORK_ID) == UIP_PROTO_ICMP6
     && (packetbuf_attr(PACKETBUF_ATTR_CHANNEL) >> 8) == ICMP6_RPL) {
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static uint16_t
get_node_timeslot(const linkaddr_t *addr)
{
  if(addr != NULL && ORCHESTRA_BROADCAST_PERIOD > 0) {
    return ORCHESTRA_LINKADDR_HASH(addr) % ORCHESTRA_BROADCAST_PERIOD;
  } else {
    return 0xffff;
  }
}
/*---------------------------------------------------------------------------*/
static void
add_bc_link(const linkaddr_t *linkaddr)
{
  if(linkaddr != NULL) {
    uint16_t timeslot = get_node_timeslot(linkaddr);
    uint8_t link_options = LINK_OPTION_RX;

    PRINTF("Add child and link \n");

    if(timeslot == get_node_timeslot(&linkaddr_node_addr)) {
      /* This is also our timeslot, add necessary flags */
      PRINTF("My timeslot");
      link_options |= LINK_OPTION_TX | BROADCAST_SLOT_SHARED_FLAG;
    }

    /* Add/update link */
    struct tsch_link *link = tsch_schedule_add_link(sf_broadcast, link_options, LINK_TYPE_NORMAL, &tsch_broadcast_address, timeslot, channel_offset);

    if(link == NULL){
      PRINTF("Adding a CHILD TSCH link failed \n");
    }else {
      PRINTF("Added CHILD TSCH link -> timeslot for Rx/Tx at: %u \n", timeslot);
   }
    
  }
}
/*---------------------------------------------------------------------------*/
static void
remove_bc_link(const linkaddr_t *linkaddr)
{
  uint16_t timeslot;
  struct tsch_link *l;

  if(linkaddr == NULL) {
    return;
  }

  timeslot = get_node_timeslot(linkaddr);
  l = tsch_schedule_get_link_by_timeslot(sf_broadcast, timeslot);
  if(l == NULL) {
    return;
  }
  /* Does our current parent need this timeslot? */
  if(timeslot == get_node_timeslot(&orchestra_parent_linkaddr)) {
    /* Yes, this timeslot is being used, return */
    return;
  }
  /* Does any other child need this timeslot?
   * (lookup all route next hops) */
  nbr_table_item_t *item = nbr_table_head(nbr_routes);
  while(item != NULL) {
    linkaddr_t *addr = nbr_table_get_lladdr(nbr_routes, item);
    if(timeslot == get_node_timeslot(addr)) {
      /* Yes, this timeslot is being used, return */
      return;
    }
    item = nbr_table_next(nbr_routes, item);
  }

  /* Do we need this timeslot? */
  if(timeslot == get_node_timeslot(&linkaddr_node_addr)) {
    /* This is our link, keep it but update the link options */
    uint8_t link_options = LINK_OPTION_TX | BROADCAST_SLOT_SHARED_FLAG;
    tsch_schedule_add_link(sf_broadcast, link_options, LINK_TYPE_NORMAL, &tsch_broadcast_address,
              timeslot, channel_offset);
  } else {
    /* Remove link */
    tsch_schedule_remove_link(sf_broadcast, l);
  }
}
/*---------------------------------------------------------------------------*/
static void
child_added(const linkaddr_t *linkaddr)
{
  add_bc_link(linkaddr);
}
/*---------------------------------------------------------------------------*/
static void
child_removed(const linkaddr_t *linkaddr)
{
  remove_bc_link(linkaddr);
}
/*---------------------------------------------------------------------------*/
static int
select_packet(uint16_t *slotframe, uint16_t *timeslot)
{
 if(packetbuf_attr(PACKETBUF_ATTR_FRAME_TYPE) == FRAME802154_DATAFRAME
     && packetbuf_holds_broadcast() && !is_RPL_traffic()) {
     	
   if(slotframe != NULL) {
      *slotframe = slotframe_handle;
    }
    if(timeslot != NULL) {
      *timeslot = get_node_timeslot(&linkaddr_node_addr);
    }

	PRINTF("sending via broadcast-per-neighbor slot\n");

    return 1;
  }
  return 0;
  }

/*---------------------------------------------------------------------------*/
static void
new_time_source(const struct tsch_neighbor *old, const struct tsch_neighbor *new)
{
  if(new != old) {
    const linkaddr_t *old_addr = old != NULL ? &old->addr : NULL;
    const linkaddr_t *new_addr = new != NULL ? &new->addr : NULL;
    if(new_addr != NULL) {
      linkaddr_copy(&orchestra_parent_linkaddr, new_addr);
    } else {
      linkaddr_copy(&orchestra_parent_linkaddr, &linkaddr_null);
    }
    remove_bc_link(old_addr);
    add_bc_link(new_addr);
  }
}
/*---------------------------------------------------------------------------*/
static void
init(uint16_t sf_handle)
{
  slotframe_handle = sf_handle;
   channel_offset = sf_handle;
  /* Slotframe for broadcast transmissions */
  sf_broadcast = tsch_schedule_add_slotframe(slotframe_handle, ORCHESTRA_BROADCAST_PERIOD);

if(sf_broadcast == NULL){
  PRINTF("Adding a SLOTFRAME failed \n");
}

uint16_t timeslot = get_node_timeslot(&linkaddr_node_addr);


struct tsch_link *link = tsch_schedule_add_link(sf_broadcast,
            LINK_OPTION_TX |BROADCAST_SLOT_SHARED_FLAG,
            LINK_TYPE_NORMAL, &tsch_broadcast_address,
            timeslot, channel_offset);
if(link == NULL){
  PRINTF("Adding a TSCH link failed \n");
}else {
  PRINTF("Added TSCH link -> timeslot for TX at: %u \n", timeslot);
}
 
}
/*---------------------------------------------------------------------------*/
struct orchestra_rule broadcast_per_neighbor = {
  init,
  new_time_source,
  select_packet,
  child_added,
  child_removed,
//  "broadcast_per_neighbor",   //My outdated contiki-ng version does not yet have this field included, uncomment again on more recent versions.
};
#endif /* UIP_MAX_ROUTES */

