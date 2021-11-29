#include "contiki.h"
#include "nn.h"

#include <stdio.h> /* For printf() */

/* override the default debug log in the tensorflow lite micro library: may not work on all MCU */
void DebugLog(const char* s) {
  if(s) {
    printf("%s", s);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(node_process, "Node process");
AUTOSTART_PROCESSES(&node_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  if(nn_setup() >= 0) {

    /* Setup a periodic timer that expires after 1 second. */
    etimer_set(&timer, CLOCK_SECOND * 1);

    while(1) {
      printf("running inference...\n");
      if(nn_run_inference() < 0) {
        printf("nn_run_inference() failed\n");
      }
      /* Wait for the periodic timer to expire and then restart the timer. */
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
      etimer_reset(&timer);
    }
  } else {
    printf("nn_setup failed\n");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
