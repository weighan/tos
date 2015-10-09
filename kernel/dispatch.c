
#include <kernel.h>

#include "disptable.c"


PROCESS active_proc;


/*
 * Ready queues for all eight priorities.
 */
PCB *ready_queue [MAX_READY_QUEUES];




/*
 * add_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is put the ready queue.
 * The appropiate ready queue is determined by p->priority.
 */

void add_ready_queue (PROCESS proc){
  if(ready_queue[proc->priority] == NULL){
    ready_queue[proc->priority] = proc;
    proc->next = proc;
    proc->prev = proc;
  }

  else{
    proc->prev = ready_queue[proc->priority]->prev;
    proc->next = ready_queue[proc->priority];
    ready_queue[proc->priority]->prev->next = proc;
    ready_queue[proc->priority]->prev = proc;
  }

}



/*
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */

void remove_ready_queue (PROCESS proc){

  if(proc->next->name == proc->name){
    ready_queue[proc->priority] == NULL;
  }
  else{
    proc->prev->next = proc->next;
    proc->next->prev = proc->prev;
  }
}



/*
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */

PROCESS dispatcher(){
  int pri = 7;
  while(ready_queue[pri] == NULL){
    pri--;
  }
  if(pri == active_proc->priority){
    return active_proc->next;
  }
  else{
    return ready_queue[pri];
  }
}



/*
 * resign
 *----------------------------------------------------------------------------
 * The current process gives up the CPU voluntarily. The
 * next running process is determined via dispatcher().
 * The stack of the calling process is setup such that it
 * looks like an interrupt.
 */
void resign(){

}



/*
 * init_dispatcher
 *----------------------------------------------------------------------------
 * Initializes the necessary data structures.
 */

void init_dispatcher(){
  int i = 0;
  while(i < MAX_READY_QUEUES){
    ready_queue[i] = NULL;
    i++;
  }
}
