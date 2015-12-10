#include <kernel.h>

PORT timer_port;

void timer_notifier(PROCESS self, PARAM param){
  while(1){
    wait_for_interrupt(TIMER_IRQ);
    message(timer_port, NULL);
    //kprintf("beep\n");
  }
}

void timer_process(PROCESS self, PARAM param){
  int ticks_remaining[MAX_PROCS];
  PROCESS client_proc, notifier;
  PORT noti;
  Timer_Message* msg;
  int i;

  for(i = 0; i < MAX_PROCS; i++){
    ticks_remaining[i] = 0;
  }

  noti = create_process(timer_notifier, 7, 0, "Timer Notifier");
  notifier = noti->owner;
  while(1){
    //kprintf("timer\n");
    msg = (Timer_Message*) receive(&client_proc);
    //kprintf("timer got msg\n");
    if(msg == NULL){
      //timer notifier sent us something
      for(i = 0; i < MAX_PROCS; i++){
        if(ticks_remaining[i] != 0){
          ticks_remaining[i]--;
          if(ticks_remaining[i] == 0){
            reply(&pcb[i]);
          }
        }
      }
    }
    else{
      //some proc sent a sleep request
      i = client_proc - pcb;
      ticks_remaining[i] = msg->num_of_ticks;

    }
  }
}

void sleep(int ticks){
  Timer_Message msg;
  msg.num_of_ticks = ticks;
  send(timer_port, &msg);
}

void init_timer (){
 timer_port = create_process(timer_process, 6, 0, "Timer Service");

}