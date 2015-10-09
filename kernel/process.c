
#include <kernel.h>


PCB pcb[MAX_PROCS];

PORT create_process (void (*ptr_to_new_proc) (PROCESS, PARAM),
int prio,
PARAM param,
char *name){


  return NULL;
}

PROCESS fork(){
    // Dummy return to make gcc happy
    return (PROCESS) NULL;
}

void print_process(WINDOW* wnd, PROCESS p){
  char* str;
  output_string(wnd, "State: ");

  if(p->state == STATE_READY){
    str = "RDY\n";
  }
  else if(p->state == STATE_SEND_BLOCKED){
    str = "SEND BLOCKD\n";
  }
  else if(p->state == STATE_REPLY_BLOCKED){
    str = "RPLY BLOCKD\n";
  }
  else if(p->state == STATE_RECEIVE_BLOCKED){
    str = "RECV BLOCKD\n";
  }
  else if(p->state == STATE_MESSAGE_BLOCKED){
    str = "MSG BLOCKD\n";
  }
  else if(p->state == STATE_INTR_BLOCKED){
    str = "INTR BLOCKD\n";
  }
  else{
    str = "undefined1\n";
  }
  output_string(wnd, (const)str);
  output_string(wnd, "Prio:  ");
  str = p->priority;
  output_string(wnd, str);
  output_char(wnd, '\n');
  output_string(wnd, "Name:  ");
  str = p->name;
  output_string(wnd, str);
  output_char(wnd, '\n');
}

void print_all_processes(WINDOW* wnd){
  int i;
  char* str;
  char c;
  output_string(wnd, "State          Active  Prio.   Name\n");
  for(i = 0; i < wnd->width;i++){
    output_char(wnd,0xcd);
  }
  output_string(wnd, "\n");
  for(i = 0; i < MAX_PROCS; i++){
    if(pcb[i].used){
      if(pcb[i].state == STATE_READY){
        str = "RDY               ";
      }
      else if(pcb[i].state == STATE_SEND_BLOCKED){
        str = "SEND BLOCKD       ";
      }
      else if(pcb[i].state == STATE_REPLY_BLOCKED){
        str = "RPLY BLOCKD       ";
      }
      else if(pcb[i].state == STATE_RECEIVE_BLOCKED){
        str = "RECV BLOCKD       ";
      }
      else if(pcb[i].state == STATE_MESSAGE_BLOCKED){
        str = "MSG BLOCKD        ";
      }
      else if(pcb[i].state == STATE_INTR_BLOCKED){
        str = "INTR BLOCKD       ";
      }
      else{
        str = "undefined2        ";
      }
      output_string(wnd, str);

      if(pcb[i].esp == active_proc->esp){
        output_string(wnd, "X      ");
      }
      else{
        output_string(wnd, "       ");
      }
      c = pcb[i].priority + '0';
      output_char(wnd, c);
      output_string(wnd, "     ");
      output_string(wnd, pcb[i].name);
      output_char(wnd, '\n');
    }
  }
}

void init_process(){
  pcb[0].magic = MAGIC_PCB;
  pcb[0].used = TRUE;
  pcb[0].state = STATE_READY;
  pcb[0].priority = 1;
  pcb[0].first_port = NULL;
  pcb[0].name = "Boot process";
}
