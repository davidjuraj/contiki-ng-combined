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
 * Schedules a slotframe for LL broadcast transmission containing sender-based shared slots. 
 *  One broadcast-related transmitting slot and as many broadcast-related listening slots as the number of children.
 *  Receive slots are no longer scheduled for broadcast traffic originating from children, so the child_added() and child_removed() 
 *  functions remain empty; 
 * \author Simon Duquennoy <simonduq@sics.se>
 */

#include "contiki.h"
#include "orchestra.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/packetbuf.h"
#include "net/routing/routing.h"
#include "net/ipv6/uip-icmp6.h"


#ifdef ORCHESTRA_DOWNWARDS_BROADCAST

#if ORCHESTRA_COLLISION_FREE_HASH
#define BROADCAST_SLOT_SHARED_FLAG    ((ORCHESTRA_BROADCAST_PERIOD < (ORCHESTRA_MAX_HASH + 1)) ? LINK_OPTION_SHARED : 0)
#else
#define BROADCAST_SLOT_SHARED_FLAG      LINK_OPTION_SHARED
#endif

static uint16_t slotframe_handle = 0;
static uint16_t local_channel_offset = 0;
static uint16_t local_timeslot_offset;
static struct tsch_slotframe *sf_broadcast;

/*---------------------------------------------------------------------------*/
static uint16_t
get_node_timeslot(const linkaddr_t *addr)
{
  if(addr != NULL && ORCHESTRA_BROADCAST_PERIOD > 0) {
    return (ORCHESTRA_LINKADDR_HASH(addr)+1) % ORCHESTRA_BROADCAST_PERIOD;
  } else {
    return 0xffff;
  }
}
/*---------------------------------------------------------------------------*/
/*static uint16_t
get_node_channel_offset(const linkaddr_t *addr)
{
  if(addr != NULL && ORCHESTRA_BROADCAST_MAX_CHANNEL_OFFSET >= ORCHESTRA_BROADCAST_MIN_CHANNEL_OFFSET) {
    return (ORCHESTRA_LINKADDR_HASH(addr)) % (ORCHESTRA_BROADCAST_MAX_CHANNEL_OFFSET - ORCHESTRA_BROADCAST_MIN_CHANNEL_OFFSET + 1)
        + ORCHESTRA_UNICAST_BROADCAST_CHANNEL_OFFSET;
  } else {
    return 0xffff;
  }
}*/
/*---------------------------------------------------------------------------*/
static void
add_bc_link(const linkaddr_t *linkaddr)
{
  if(linkaddr != NULL) {
    uint16_t timeslot = get_node_timeslot(linkaddr);
    uint8_t link_options = LINK_OPTION_RX;

    if(timeslot == get_node_timeslot(&linkaddr_node_addr)) {
      /* This is also our timeslot, add necessary flags */
      link_options |= LINK_OPTION_TX | BROADCAST_SLOT_SHARED_FLAG;
    }

    /* Add/update link.
     * Always configure the link with the local node's channel offset.
     * If this is an Rx link, that is what the node needs to use.
     * If this is a Tx link, packet's channel offset will override the link's channel offset.
     */
    tsch_schedule_add_link(sf_broadcast, link_options, LINK_TYPE_NORMAL, &tsch_broadcast_address,
          timeslot, local_channel_offset);
  }
}
/*---------------------------------------------------------------------------*/
static void
remove_bc_link(const linkaddr_t *linkaddr)
{
	//TODO!
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
              timeslot, local_channel_offset);
  } else {
    /* Remove link */
    tsch_schedule_remove_link(sf_broadcast, l);
  }
}
/*---------------------------------------------------------------------------*/
static void
child_added(const linkaddr_t *linkaddr)
{
  //downward sender based broadcast slot, the parent is not responsible for the child's slot and doesn't listen to it.
  //add_bc_link(linkaddr);
}
/*---------------------------------------------------------------------------*/
static void
child_removed(const linkaddr_t *linkaddr)
{
  //downward sender based broadcast slot, the parent is not responsible for the child's slot and doesn't listen to it.
  //remove_bc_link(linkaddr);
}
/*---------------------------------------------------------------------------*/
static int
select_packet(uint16_t *slotframe, uint16_t *timeslot)
{
  if(packetbuf_attr(PACKETBUF_ATTR_DOWNWARDSBROADCAST) == 1)
  {
	printf("ORCHESTRA: sending via downward facing BMRF broadcast!\n");
    if(slotframe != NULL) {
      *slotframe = slotframe_handle;
    }
    if(timeslot != NULL) {
      *timeslot = local_timeslot_offset;
    }
    /* set per-packet channel offset */
    //if(channel_offset != NULL) {
    //  *channel_offset = local_channel_offset;
    //}
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
new_time_source(const struct tsch_neighbor *old, const struct tsch_neighbor *new)
{
  //corresponds with (new) RPL parent = the one we want to receive broadcast from
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
  linkaddr_t *local_addr = &linkaddr_node_addr;

  slotframe_handle = sf_handle;
  local_channel_offset = sf_handle; // get_node_channel_offset(local_addr);
  /* Slotframe for downwards broadcast transmissions */
  sf_broadcast = tsch_schedule_add_slotframe(slotframe_handle, ORCHESTRA_BROADCAST_PERIOD);
  local_timeslot_offset = get_node_timeslot(local_addr);
  tsch_schedule_add_link(sf_broadcast,
            LINK_OPTION_TX | BROADCAST_SLOT_SHARED_FLAG,
            LINK_TYPE_NORMAL, &tsch_broadcast_address,
			local_timeslot_offset, local_channel_offset);
}
/*---------------------------------------------------------------------------*/
struct orchestra_rule broadcast_downward = {
  init,
  new_time_source,
  select_packet,
  child_added,
  child_removed,
  //"broadcast_downward",
};

#endif /* UIP_MAX_ROUTES */
