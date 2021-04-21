CONTIKI_PROJECT = pwmmulticast

all: $(CONTIKI_PROJECT)

CONTIKI = ../..
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"


MODULES += os/net/ipv6/multicast
MAKE_ROUTING = MAKE_ROUTING_RPL_CLASSIC


MAKE_MAC = MAKE_MAC_TSCH


# Activate orchestra
MODULES += os/services/orchestra


include $(CONTIKI)/Makefile.include
