/*
 * Copyright (c) 2021, Institute of Electronics and Computer Science (EDI)
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
 *         An example that demonstrates activity recognition using ICM-20948
 *         sensor data and communication of the data to a central server using
 *         the TSCH wireless protocol.
 *
 * \author Atis Elsts <atis.elsts@edi.lv>
 */

#include "contiki.h"
#include "net/ipv6/simple-udp.h"
#include "net/mac/tsch/tsch.h"
#include "lib/random.h"
#include "sys/node-id.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_PORT	8765
#define SEND_INTERVAL		  (1 * CLOCK_SECOND)

PROCESS(node_process, "Node");
AUTOSTART_PROCESSES(&node_process);

/*
 * Note! This is not an example how to design a *good* schedule for TSCH,
 * nor this is the right place for complete beginners in TSCH.
 * We recommend using the default Orchestra schedule for a start.
 *
 * The intention of this file is instead to serve a starting point for those interested in building
 * their own schedules for TSCH that are different from Orchestra and 6TiSCH minimal.
 */

/* The recognized activities */
enum activity_code {
  ACTIVITY_WALKING,
  ACTIVITY_WALKING_UPSTAIRS,
  ACTIVITY_WALKING_DOWNSTAIRS,
  ACTIVITY_SITTING,
  ACTIVITY_STANDING,
  ACTIVITY_LAYING,
};

/* Over-the-air data */
struct application_data {
  uint32_t timestamp;
  uint16_t seqnum;
  uint16_t activity;
};

static void
rx_packet(struct simple_udp_connection *c,
          const uip_ipaddr_t *sender_addr,
          uint16_t sender_port,
          const uip_ipaddr_t *receiver_addr,
          uint16_t receiver_port,
          const uint8_t *data,
          uint16_t datalen)
{
  struct application_data appdata;

  if(datalen >= sizeof(appdata)) {
    memcpy(&appdata, data, sizeof(appdata));

    LOG_INFO("Received from ");
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_(", timestamp %" PRIu32 " activity %" PRIu16 "\n", appdata.timestamp, appdata.activity);
  }
}


static bool
sample_sensor(uint8_t accel_data[3])
{
  accel_data[0] = 1;
  accel_data[1] = 2;
  accel_data[2] = 3;
  return true;
}


static enum activity_code
infer_activity(uint8_t accel_data[3])
{
  return ACTIVITY_WALKING;
}


static bool
get_activity(struct application_data *appdata)
{
  uint8_t accel_data[3];
  if (!sample_sensor(accel_data)) {
    return false;
  }

  appdata->activity = infer_activity(accel_data);
  return true;
}


PROCESS_THREAD(node_process, ev, data)
{
  static struct simple_udp_connection udp_conn;
  static struct etimer periodic_timer;
  static uint32_t seqnum;
  uip_ipaddr_t dst;
  struct application_data appdata;

  PROCESS_BEGIN();

  /* Initialization; `rx_packet` is the function for packet reception */
  simple_udp_register(&udp_conn, UDP_PORT, NULL, UDP_PORT, rx_packet);
  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

  if(node_id == 1) {  /* Running on the root? */
    NETSTACK_ROUTING.root_start();
  } else {

    /* Main loop */
    while(1) {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

      if(get_activity(&appdata)) {
        if(NETSTACK_ROUTING.node_is_reachable()
            && NETSTACK_ROUTING.get_root_ipaddr(&dst)) {
          /* Send network uptime timestamp to the network root node */
          seqnum++;
          appdata.timestamp = (uint32_t)tsch_get_network_uptime_ticks();
          LOG_INFO("Send to ");
          LOG_INFO_6ADDR(&dst);
          LOG_INFO_(", timestamp %" PRIu32 " activity %" PRIu16 "\n", appdata.timestamp, appdata.activity);
          simple_udp_sendto(&udp_conn, &seqnum, sizeof(seqnum), &dst);
        }
      }
      etimer_set(&periodic_timer, SEND_INTERVAL);
    }
  }

  PROCESS_END();
}
