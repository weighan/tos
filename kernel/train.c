#include <kernel.h>

//run the train application

void reverse_dir(){
  send_com("L20S0\015");
  sleep(sleep_time);
  send_com("L20D\015");
  sleep(sleep_time);
  send_com("L20S5\015");
}

void wait_for_track(int trak){
  while(!poll_track(trak)){
    sleep(2);
  }
}

void set_switch(char* comd){
  char real_com[5];
  real_com[0] = 'M';
  real_com[1] = comd[0];
  real_com[2] = comd[1];
  real_com[3] = '\015';
  real_com[4] = 0;
  send_com(real_com);
  sleep(sleep_time);
}

int poll_track(int t){
  COM_Message msg_command;
  char result[4];
  char track_num[5];
  send_com("R\015");
  track_num[0] = 'C';
  if(t < 10){
    track_num[1] = t + 48;
    track_num[2] = '\015';
    track_num[3] = 0;
  }
  else{
    track_num[1] = '1';
    track_num[2] = t + 38;
    track_num[3] = '\015';
    track_num[4] = 0;
  }

  msg_command.output_buffer = track_num;
  msg_command.input_buffer = result;
  msg_command.len_input_buffer = 3;
  send(com_port, &msg_command);
  kprintf("poll reads: %s\n", result);
  return result[1] - 48;
}

int first_check_dir(){
  sleep(130);
  kprintf("polling\n");
  return poll_track(3);
}

void track_1(){
  //set switches so zamboni will run in a loop
  set_switch("5G");
  set_switch("9R");
  set_switch("8R");
  set_switch("7R");
  set_switch("2R");
  set_switch("1R");
  set_switch("6R");
  wait_for_track(10);
  send_com("L20S5\015");
  set_switch("4R");
  set_switch("3G");
  set_switch("5R");
  wait_for_track(1);
  reverse_dir();
  wait_for_track(8);
  send_com("L20S0\015");
}

void track_2(){
  //set switches so zamboni will run in a loop
  set_switch("9R");
  set_switch("8R");
  set_switch("7R");
  set_switch("2R");
  set_switch("1R");
  //move the train and change switches to go to wagon
  send_com("L20S5\015");
  sleep(sleep_time);
  set_switch("4R");
  set_switch("3G");
  set_switch("5R");
  set_switch("6R");
  //wait until something is detected on track 1
  wait_for_track(1);
  //once train is on track 1, it also must have wagon w/ it
  reverse_dir();
  //wait until train gets back to track 8, then stop
  wait_for_track(8);
  send_com("L20S0\015");
}

void track_3(){
  set_switch("5G");
  send_com("L20S5\015");
  set_switch("9R");
  set_switch("1G");
  wait_for_track(10);
  set_switch("5R");
  set_switch("6G");
  set_switch("7R");
  set_switch("8G");
  wait_for_track(12);
  reverse_dir();
  set_switch("4R");
  set_switch("3R");
  set_switch("8R");
  wait_for_track(7);
  wait_for_track(6);
  reverse_dir();
  wait_for_track(5);
  send_com("L20S0\015");
}

void track_4(){
  //set switches so zamboni will run in a loop
  set_switch("8G");
  send_com("L20S5\015");
  wait_for_track(6);
  reverse_dir();
  set_switch("3G");
  set_switch("4R");
  set_switch("2G");
  set_switch("1R");
  set_switch("9G");
  sleep(200);
  set_switch("4G");
  //wait until train gets to track 14
  wait_for_track(14);
  reverse_dir();
  //backup to get wagon
  // may need to change
  sleep(200);
  reverse_dir();
  set_switch("4R");
  set_switch("3R");
  wait_for_track(10);
  set_switch("8R");
  wait_for_track(5);
  send_com("L20S0\015");
}
void train_process(PROCESS self, PARAM param){
  if (first_check_dir()){
    //if train is going ccw, must be config 2 or 4
    if(poll_track(2)){
      kprintf("config 2\n");
      track_2();
    }
    else{
      kprintf("config 4\n");
      track_4();
    }
  }
  else{
    if(poll_track(2)){
      kprintf("config 1\n");
      track_1();
    }
    else{
      kprintf("config 3\n");
      track_3();
    }
  }
  kprintf("Wagon Retrieved\n");
  while(1);
}


void init_train(WINDOW* wnd){
  create_process(train_process, 5, 0, "Train Process");
}
