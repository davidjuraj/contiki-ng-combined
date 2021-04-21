#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


#include "net/ipv6/multicast/uip-mcast6-engines.h"

/* Change this to switch engines. Engine codes in uip-mcast6-engines.h */
#define UIP_MCAST6_CONF_ENGINE	UIP_MCAST6_ENGINE_BMRF
#define BMRF_CONF_MODE	BMRF_UNICAST_MODE
//#define BMRF_CONF_MODE	BMRF_BROADCAST_MODE
//#define BMRF_CONF_MODE	BMRF_MIXED_MODE
//#define BMRF_CONF_BROADCAST_THRESHOLD	1


#define UIP_MCAST6_ROUTE_CONF_ROUTES 15
#define CSMA_CONF_MAX_NEIGHBOR_QUEUES UIP_MCAST6_ROUTE_CONF_ROUTES

/* Code/RAM footprint savings so that things will fit on our device */
#ifndef NETSTACK_MAX_ROUTE_ENTRIES
#define NETSTACK_MAX_ROUTE_ENTRIES   15
#endif

#ifndef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 15
#endif

#define UIP_CONF_BUFFER_SIZE 128



#endif /* PROJECT_CONF_H_ */

/** @} */
