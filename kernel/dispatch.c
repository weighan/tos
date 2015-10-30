
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
  proc->state = STATE_READY;
}

/*
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */

void remove_ready_queue (PROCESS proc){

  if(proc->next == proc){
    ready_queue[proc->priority] = NULL;
  }
  else{
    if(ready_queue[proc->priority] == proc){
      ready_queue[proc->priority] = proc->next;
    }
    proc->prev->next = proc->next;
    proc->next->prev = proc->prev;
  }
  //proc->next = NULL;
  //proc->prev = NULL;
}

/*
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */

PROCESS dispatcher(){
  int priority = MAX_READY_QUEUES -1;
  while(ready_queue[priority] == NULL){
    priority--;
    if(priority <0){return NULL;}
  }
  if(priority == active_proc->priority){
    return active_proc->next;
  }
  else{
    return ready_queue[priority];
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
  //push old process' registers to stack
  asm("pushl %eax\n\t"
      "pushl %ecx\n\t"
      "pushl %edx\n\t"
      "pushl %ebx\n\t"
      "pushl %ebp\n\t"
      "pushl %esi\n\t"
      "pushl %edi");
  asm("movl %%esp , %0" : "=r" (active_proc->esp):);

  active_proc = dispatcher();

  //load new process' new registers from stack
  asm("movl %0, %%esp" :: "r" (active_proc->esp ):);
  asm("popl %edi\n\t"
      "popl %esi\n\t"
      "popl %ebp\n\t"
      "popl %ebx\n\t"
      "popl %edx\n\t"
      "popl %ecx\n\t"
      "popl %eax");
  asm("ret");
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
  add_ready_queue(active_proc);
}