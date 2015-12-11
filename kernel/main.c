
#include <kernel.h>
void print_train();

void kernel_main(){
    // this turns off the VGA hardware cursor
    // otherwise we get an annoying, meaningless,
    // blinking cursor in the middle of our screen
    outportb(0x03D4, 0x0E);
    outportb(0x03D5, 0xFF);
    outportb(0x03D4, 0x0F);
    outportb(0x03D5, 0xFF);

    init_process();
    init_dispatcher();
    init_ipc();
    init_interrupts();
    init_null_process();
    init_timer();
    init_com();
    init_keyb();
    init_shell();

    clear_window(kernel_window);
    init_train(kernel_window);
    //print_train();
    //show_cursor(kernel_window);
    while (1);
}

void print_train(){
    kprintf("                 _-====-__-======-__-========-_____-============-__\n");
    kprintf("               _(                                                 _)\n");
    kprintf("            OO(                   Welcome to TOS                  )_\n");
    kprintf("           0  (_                                                   _)\n");
    kprintf("         o0     (_                                                _)\n");
    kprintf("        o         '=-___-===-_____-========-___________-===-dwb-='\n");
    kprintf("      .o                                _________\n");
    kprintf("     . ______          ______________  |         |      _____\n");
    kprintf("   _()_||__|| ________ |            |  |_________|   __||___||__\n");
    kprintf("  (         | |      | |            | __Y______00_| |_         _|\n");
    kprintf(" /-OO----OO\"\"=\"OO--OO\"=\"OO--------OO\"=\"OO-------OO\"=\"OO-------OO\"=P\n");
    kprintf("#####################################################################\n");
}