/**
 * Add this header to project-conf.h to customize the SLIP input handler for determining the
 * source of the multicast message. It does this by setting the Link Layer sender and receiver to
 * predefined values.
 */

#ifndef SLIP_MULTICAST_H
#define SLIP_MULTICAST_H


#define SLIP_MULTICAST 1


//Following define makes SLIP use our custom input function handler which sets LLsender to all FF's
//This is done to be able to detect the source of the multicast in BMRF
#define SLIP_CONF_TCPIP_INPUT slip_input_multicast
void slip_input_multicast(void);
void slip_output_multicast(void);

#endif /*SLIP_MULTICAST_H*/
