#include <kernel.h>
/*
WINDOW shell_wnd = {0, 9, 61, 16, 0, 0, 0xDC};

void shell_process(PROCESS self, PARAM param){
  char cmd[20];
  char ch;
  Keyb_Message msg;
  int i = 0;
  while (1) {
    msg.key_buffer = &ch;
    send(keyb_port, &msg);
    if(ch == "enter key"){
      //check cmd, if its a valid command, run it
      //reset cmd afterward
      cmd[i+1] = '\0';
      if(cmd == "clear"){
        clear_window(shell_wnd);
      }
      else if(cmd == "listprocs"){
        print_all_processes(shell_wnd);
      }
      i = 0;
    }
    else{
      //not enter, so add the char to the end of cmd, increment i
      cmd[i] = ch;
      i++;
    }

  }
}
*/
void init_shell(){
  //create_process(shell_process, 5, 0, "Shell Process");
}
