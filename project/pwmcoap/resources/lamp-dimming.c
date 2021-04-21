/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
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
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include "contiki.h"
#include "coap-engine.h"
#include "dev/leds.h"

#include "pwm.h"

#include <string.h>

static void pwm_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void pwm_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
void pwm_init_handler();


static uint16_t dutycycle; //in promille
static pwm_t ledDriverPWM;

/* A simple actuator example. Toggles the led */
RESOURCE(pwm_light,
         "title=\"LED\";rt=\"Control\"",
         pwm_get_handler,
         pwm_post_put_handler,
         pwm_post_put_handler,
         NULL);



static void
pwm_post_put_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  if(request->payload_len > 0 && request->payload_len <= 3){
    uint16_t newDutycycle = atoi((const char *)request->payload)*10;
    if(newDutycycle <= 1000) {
      dutycycle = newDutycycle;
      pwm_startdutycycle(&ledDriverPWM, dutycycle);
      return;
    } 
  }
  coap_set_status_code(response, BAD_REQUEST_4_00); //Something wrong if we get here...
}


static void
pwm_get_handler(coap_message_t *request, coap_message_t *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

  int length = sprintf((char *)buffer, "%u", dutycycle/10);

  coap_set_header_content_format(response, TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
  coap_set_header_etag(response, (uint8_t *)&length, 1);
  coap_set_payload(response, buffer, length);
}

void pwm_init_handler(){
  //actual led in PoC on C3, red LED on PCB on D5
  pwm_configure(&ledDriverPWM, PWM_TIMER_2, PWM_TIMER_B, 1024, GPIO_C_NUM, 3, PWM_ON_WHEN_STOP);
  //PRINTF("\nConfiguring PWM on timer %u/%d to frequency %lu Hz\n", ledDriverPWM.timer, ledDriverPWM.ab, ledDriverPWM.freq);

  dutycycle = 700;
  pwm_startdutycycle(&ledDriverPWM, dutycycle);
  //PRINTF("Set PWM on timer %u/%u to duty cycle %u promille\n",ledDriverPWM.timer, ledDriverPWM.ab, dutycycle);

}
