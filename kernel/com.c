/* Internet ressources:
 * http://workforce.cup.edu/little/serial.html
 * http://www.lammertbies.nl/comm/info/RS-232.html
 */
#include <kernel.h>

PORT com_port;

void com_reader_process(PROCESS sef, PARAM param){
  COM_Message* msg;
  PROCESS com_proc;
  int i;
  while(1){
    msg = receive(&com_proc);
    kprintf("msg from: %s\n", com_proc->name);
    //kprintf("len is %d\n", msg->len_input_buffer);
    for(i = 0; i < msg->len_input_buffer; i++){
      wait_for_interrupt(COM1_IRQ);
      //msg->input_buffer[i] = inportb(COM1_PORT);
      //kprintf("%c\n",msg->input_buffer[i]);
      kprintf("%c",inportb(COM1_PORT));
    }
    msg->input_buffer[i+1] = '\0';
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
    kprintf("msg sent\n");

    while(msg->output_buffer[i] != '\0'){
      //kprintf("%c\n", msg->output_buffer[i]);
      while (!(inportb(COM1_PORT+5) & (1<<5)));
      outportb(COM1_PORT,(unsigned char) msg->output_buffer[i]);
      i++;
    }
/*
    while(i <100){
      wait_for_interrupt(COM1_IRQ);
      kprintf("%c",inportb(COM1_PORT));
      i++;
    }
*/
    receive(&reader);
    kprintf("client is: %s\n", client_proc->name);
    //print_all_processes(kernel_window);
    reply(client_proc);
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
