#include <kernel.h>

PORT_DEF port[MAX_PORTS];

PORT check_block_list();
void add_block_list(PORT dest_port);
void remove_block_list(PORT dest_port, PROCESS rm_this);

PORT create_port(){
  int i;
  for(i =0; i < MAX_PORTS; i++){
    if(port[i].used == FALSE){
      port[i].used = TRUE;
      port[i].owner = active_proc;
      //this is the first port for active_proc
      if(active_proc->first_port == NULL){
        active_proc->first_port = &port[i];
      }
      //this is not active_proc's first port
      else{
        PORT temp = active_proc->first_port;
        while(temp->next != NULL){
          temp = temp->next;
        }
        temp->next = &port[i];
      }
      return &port[i];
    }
  }
  return NULL;
}

PORT create_new_port (PROCESS owner){
  int i;
  for(i =0; i < MAX_PORTS; i++){
    if(port[i].used == FALSE){
      port[i].used = TRUE;
      port[i].owner = owner;
      //this is the first port for owner
      if(owner->first_port == NULL){
        owner->first_port = &port[i];
      }
      //this is not owner's first port
      else{
        PORT temp = owner->first_port;
        while(temp->next != NULL){
          temp = temp->next;
        }
        temp->next = &port[i];
      }

      return &port[i];
    }
  }
  return NULL;
}

void send (PORT dest_port, void* data){
  active_proc->param_data = data;
  //sender goes first
  if(dest_port->owner->state != STATE_RECEIVE_BLOCKED || !(dest_port->open)){
    add_block_list(dest_port);

    active_proc->state = STATE_SEND_BLOCKED;
    remove_ready_queue(active_proc);
    resign();
    //remove_block_list(dest_port);
  }
  //sender goes second
  else {
    add_block_list(dest_port);
    active_proc->state = STATE_REPLY_BLOCKED;
    remove_ready_queue(active_proc);
    add_ready_queue(dest_port->owner);
    resign();
    //remove_block_list(dest_port);
  }
}

void message (PORT dest_port, void* data){
  active_proc->param_data = data;
  //msg goes first
  if(dest_port->owner->state != STATE_RECEIVE_BLOCKED || !(dest_port->open)){
    add_block_list(dest_port);

    active_proc->state = STATE_MESSAGE_BLOCKED;
    remove_ready_queue(active_proc);
    resign();
    //remove_block_list(dest_port);
  }
  //msg goes second
  else{
    add_block_list(dest_port);
    add_ready_queue(dest_port->owner);
    resign();
    //remove_block_list(dest_port);
  }
  dest_port->owner->param_data = data;
}

void* receive (PROCESS* sender){
  PORT p = active_proc->first_port;
  PROCESS temp;
  void* dat;

  p = check_block_list();
  //if blocked list is empty or receive goes first
  if(p == NULL){
    active_proc->state = STATE_RECEIVE_BLOCKED;
    remove_ready_queue(active_proc);
    resign();
    p = check_block_list();
  }
  //if blocked list is not empty or receive goes second
  temp = p->blocked_list_head;
  *sender = temp;
  dat = temp->param_data;
  if(temp->state == STATE_MESSAGE_BLOCKED){
    add_ready_queue(temp);
  }
  else if(temp->state == STATE_SEND_BLOCKED){
    temp->state = STATE_REPLY_BLOCKED;
  }

  remove_block_list(p,temp);
  return dat;
  /*
  while(p != NULL){
    if(p->open && p->blocked_list_head != NULL){
      temp = p->blocked_list_head;
      *sender = temp;
      kprintf("sender is: %s\n", temp->name);
      dat = temp->param_data;
      if(temp->state == STATE_MESSAGE_BLOCKED){
        add_ready_queue(temp);
      }
      else if(temp->state == STATE_SEND_BLOCKED){
        temp->state = STATE_REPLY_BLOCKED;
      }
      remove_block_list(p,temp);
      return dat;
    }
    p = p->next;
  }
  return NULL;
  */
}

//checks the block list for the first open port that has a process
//associated with it and returns that process, returns NULL if empty
PORT check_block_list(){
  PORT p = active_proc->first_port;
  while(p != NULL){
    if(p->open && p->blocked_list_head != NULL){
      return p;
    }
    p = p->next;
  }
  return NULL;
}

void reply (PROCESS sender){
  if(sender->state == STATE_REPLY_BLOCKED){
    add_ready_queue(sender);
    resign();
  }
}

void open_port (PORT port){
  port->open = TRUE;
}

void close_port (PORT port){
  port->open = FALSE;
}

void init_ipc(){
  int i;
  for(i = 0; i< MAX_PORTS; i++){
    port[i].magic =             MAGIC_PORT;
    port[i].used  =             FALSE;
    port[i].open =              TRUE;
    port[i].owner =             NULL;
    port[i].blocked_list_head = NULL;
    port[i].blocked_list_tail = NULL;
    port[i].next =              NULL;

  }
}

//helper functions

void add_block_list(PORT dest_port){
  if(dest_port->blocked_list_head == NULL){
    dest_port->blocked_list_head = active_proc;
    dest_port->blocked_list_tail = active_proc;
  }
  else{
    dest_port->blocked_list_tail->next_blocked = active_proc;
    dest_port->blocked_list_tail = active_proc;
  }
}

void remove_block_list(PORT dest_port, PROCESS rm_this){
  PROCESS temp, prev_temp, proc_to_rm;
  proc_to_rm = rm_this;
  temp = dest_port->blocked_list_head;
  //if there is only 1 process on the blocked list
  if(temp == proc_to_rm && dest_port->blocked_list_tail == proc_to_rm){
    dest_port->blocked_list_head = NULL;
    dest_port->blocked_list_tail = NULL;
    proc_to_rm->next_blocked = NULL;
    return;
  }

  while(temp->next_blocked != NULL){
    if(temp == proc_to_rm){
      if(temp == dest_port->blocked_list_head){
        dest_port->blocked_list_head = temp->next_blocked;
      }
      else if(temp == dest_port->blocked_list_tail){
        dest_port->blocked_list_tail = prev_temp;
      }
      else{
        prev_temp->next_blocked = temp->next_blocked;
      }
      proc_to_rm->next_blocked = NULL;
      return;
    }
    prev_temp = temp;
    temp = temp->next_blocked;
  }
}
/*
typedef struct _PORT_DEF {
    unsigned  magic;
    unsigned  used;               Port slot used?
    unsigned  open;               Port open?
    PROCESS   owner;              Owner of this port
    PROCESS   blocked_list_head;  First local blocked process
    PROCESS   blocked_list_tail;  Last local blocked process
    struct _PORT_DEF *next;       Next port
} PORT_DEF;
*/