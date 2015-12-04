#include <kernel.h>

void null_proc(PROCESS self, PARAM param){
  while(1){}
}

void init_null_process(){
  create_process(null_proc, 0, 0, "Null Process");
}