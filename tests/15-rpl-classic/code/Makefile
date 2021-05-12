all: sender-node receiver-node root-node
CONTIKI=../../..

MODULES += os/net/ipv6/multicast #edited
MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC

MAKE_MAC = MAKE_MAC_TSCH


# Activate orchestra
MODULES += os/services/orchestra

include $(CONTIKI)/Makefile.include
