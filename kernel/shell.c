#include <kernel.h>

WINDOW shell_window = {60, 0, 40, 25, 0, 0, 0x5F};
WINDOW* shell_wnd = &shell_window;
int sleep_time = 5;

char* get_input();
char get_char();
void clear_cmd();
void listprocs();
void train_stop();
void train_go();
void train_reverse();
void switch_track();
void pacman();
void halp();
void start_train();
void send_com(const char* cmd);

void shell_process(PROCESS self, PARAM param){

  char* command;
  while(1){
    show_cursor(shell_wnd);
    command = get_input();
    //wprintf(shell_wnd, "got %s",command);
    //print_all_processes(kernel_window);
    if(!k_memcmp(command, "clear", 5)){
      clear_cmd();
    }
    else if(!k_memcmp(command, "listprocs", 9)){
      list_procs();
    }
    else if(!k_memcmp(command, "str", 3)){
      train_stop();
    }
    else if(!k_memcmp(command, "gtr", 3)){
      train_go();
    }
    else if(!k_memcmp(command, "revtr", 5)){
      train_reverse();
    }
    else if(!k_memcmp(command, "switch", 6)){
      switch_track();
    }
    else if(!k_memcmp(command, "pac", 3)){
      pacman();
    }
    else if(!k_memcmp(command, "help", 4)){
      halp();
    }
    else if(!k_memcmp(command, "start", 5)){
      start_train();
    }
    else{
      kprintf("unrecognized command\n");
    }
    command = NULL;
  }
}

void halp(){
  kprintf("list of commands\n");
  kprintf("clear\n");
  kprintf("listprocs\n");
  kprintf("str\n");
  kprintf("gtr\n");
  kprintf("revtr\n");
  kprintf("switch\n");
  kprintf("pac\n");
  kprintf("start\n");
  kprintf("help\n");
}

void start_train(){
  init_train(kernel_window);
}

void clear_cmd(){
  kprintf("clear detected\n");
  clear_window(kernel_window);
}

void list_procs(){
  print_all_processes(shell_wnd);
}

void train_stop(){
  send_com("L20S0\015");
}

void train_go(){
  send_com("L20S5\015");
}

void train_reverse(){
  send_com("L20S0\015");
  sleep(sleep_time);
  send_com("L20D\015");
}

void switch_track(){
  char track_com[5];
  track_com[0] = 'M';
  wprintf(shell_wnd, "select switch: ");
  track_com[1] = get_char();
  wprintf(shell_wnd, "\n(R)ed or (G)reen? ");
  //converts to uppercase
  track_com[2] = get_char() - 32;
  track_com[3] = '\015';
  wprintf(shell_wnd, "\n");
  track_com[4] = 0;
  send_com(track_com);
}

void send_com(const char* cmd){
  COM_Message msg_command;
  msg_command.output_buffer = cmd;
  msg_command.len_input_buffer = 0;
  send(com_port, &msg_command);
}

char* get_input(){
  char ch =' ';
  Keyb_Message msg;
  int i = 0;
  int len = 20;
  char cmd[len];
  while(ch != '\r' && i < len){
    msg.key_buffer = &ch;
    send(keyb_port, &msg);
    if(ch != 13){
      cmd[i++] = ch;
      wprintf(shell_wnd, "%c", ch);
      show_cursor(shell_wnd);
    }
  }
  remove_cursor(shell_wnd);
  wprintf(shell_wnd, "\n");
  cmd[i] = '\0';
  return cmd;
}

char get_char(){
  char ch =' ';
  Keyb_Message msg;
  msg.key_buffer = &ch;
  send(keyb_port, &msg);

  wprintf(shell_wnd, "%c", ch);
  return ch;
}

void init_shell(){
  create_process(shell_process, 5, 0, "Shell Process");
}

void pacman(){
  kprintf("DLC not purchased\n");
  kprintf("If you would like to unlock this content\n");
  kprintf("please purchase the \"WAKKA WAKKA\" DLC ($29.99)\n");
}