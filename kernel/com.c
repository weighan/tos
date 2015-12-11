#include <kernel.h>

PORT com_port;

void com_reader_process(PROCESS sef, PARAM param){
  COM_Message* msg;
  PROCESS com_proc;
  int j;
  while(1){
    msg = receive(&com_proc);
    for(j = 0; j < msg->len_input_buffer; j++){
      wait_for_interrupt(COM1_IRQ);
      msg->input_buffer[j] = inportb(COM1_PORT);
    }
    message(com_port, msg);
  }
}

void com_process(PROCESS sef, PARAM param){
  COM_Message* msg;
  PROCESS client_proc, reader;
  int i;
  PORT reader_port;
  reader_port = create_process(com_reader_process, 7, 0, "COM Reader Proc");

  while(1){
    i=0;
    msg = receive(&client_proc);
    message(reader_port, msg);
    kprintf("sending %s\n",msg->output_buffer);
    while(msg->output_buffer[i] != 0){
      //kprintf("sending %s\n",);
      while (!(inportb(COM1_PORT+5) & (1<<5)));
      outportb(COM1_PORT,(unsigned char) msg->output_buffer[i]);
      i++;
    }
    receive(&reader);
    reply(client_proc);
    msg = NULL;
    client_proc = NULL;
//    resign();
  }
}

void init_uart(){
  /* LineControl disabled to set baud rate */
  outportb (COM1_PORT + 3, 0x80);
  /* lower byte of baud rate */
  outportb (COM1_PORT + 0, 0x30);
  /* upper byte of baud rate */
  outportb (COM1_PORT + 1, 0x00);
  /* 8 Bits, No Parity, 2 stop bits */
  outportb (COM1_PORT + 3, 0x07);
  /* Interrupt enable*/
  outportb (COM1_PORT + 1, 1);
  /* Modem control */
  outportb (COM1_PORT + 4, 0x0b);
  inportb (COM1_PORT);
}

void init_com (){
  init_uart();
  com_port = create_process(com_process, 6, 0, "COM Process");

}
