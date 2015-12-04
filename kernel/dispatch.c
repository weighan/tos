
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
  volatile int saved_if;
  DISABLE_INTR(saved_if);
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
  ENABLE_INTR(saved_if);
}

/*
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */

void remove_ready_queue (PROCESS proc){
  volatile int saved_if;
  DISABLE_INTR(saved_if);
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
  ENABLE_INTR(saved_if);
}

/*
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */

PROCESS dispatcher(){
  volatile int saved_if;
  DISABLE_INTR(saved_if);
  int priority = MAX_READY_QUEUES -1;
  while(ready_queue[priority] == NULL){
    priority--;
    if(priority <0){
      ENABLE_INTR(saved_if);
      return NULL;
    }
  }
  if(priority == active_proc->priority){
    ENABLE_INTR(saved_if);
    return active_proc->next;
  }
  else{
    ENABLE_INTR(saved_if);
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
  asm("pushfl\n\t"
      "cli\n\t"
      "pop %eax\n\t"
      "xchg (%esp), %eax\n\t"
      "push %cs\n\t"
      "push %eax");

  asm ("pushl %eax;pushl %ecx;pushl %edx");
  asm ("pushl %ebx;pushl %ebp;pushl %esi;pushl %edi");

  asm("movl %%esp , %0" : "=r" (active_proc->esp):);

  active_proc = dispatcher();

  //load new process' new registers from stack
  asm("movl %0, %%esp" :: "r" (active_proc->esp):);

  asm ("popl %edi;popl %esi;popl %ebp;popl %ebx");
  asm ("popl %edx;popl %ecx;popl %eax");
  asm("iret");
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