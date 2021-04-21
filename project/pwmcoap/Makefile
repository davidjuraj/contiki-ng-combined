CONTIKI_PROJECT = pwmcoap

all: $(CONTIKI_PROJECT)

CONTIKI = ../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

# Multicast and RPL classic required for joining BR advertised network
MODULES += os/net/ipv6/multicast
MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC

# Include the CoAP implementation
MODULES += os/net/app-layer/coap

# Include CoAP resources
MODULES_REL += ./resources

#MAKE_MAC = MAKE_MAC_TSCH


include $(CONTIKI)/Makefile.include
