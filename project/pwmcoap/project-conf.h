#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

#define LOG_LEVEL_APP LOG_LEVEL_DBG

//#define LOG_CONF_LEVEL_RPL LOG_LEVEL_DBG
//#define LOG_CONF_LEVEL_IPV6 LOG_LEVEL_DBG

#include "net/ipv6/multicast/uip-mcast6-engines.h"
#define UIP_MCAST6_CONF_ENGINE	UIP_MCAST6_ENGINE_BMRF
#define BMRF_CONF_MODE	BMRF_UNICAST_MODE

#define LPM_CONF_ENABLE 0  //Absolute must for enabling 32KB RAM!


#define CC2538_RF_CONF_TX_POWER 		0xFF  //Default 0xd5: +3dBm,  0xff: +7dBm

//#define CC2538_RF_CONF_CHANNEL              26

#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED          1

#define CSMA_CONF_LLSEC_DEFAULT_KEY0 { 0xAA , 0xBB , 0xCC , 0xDD , 0xEE , 0xFF , 0x00 , 0x11 , 0x22 , 0x33 , 0x44 , 0x55 , 0x66 , 0x77 , 0x88 , 0x99 }



#endif /* PROJECT_CONF_H_ */

/** @} */
