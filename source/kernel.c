#include "headers/kernel.h"
#include "headers/utils.h"
#include "headers/char.h"

// index for video buffer array
uint32 vga_index;

// counter to store new lines
static uint32 next_line_index = 1;

// fore & back color values
uint8 g_fore_color = WHITE, g_back_color = BLUE;

// ascii code integers
int digit_ascii_codes[10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

/*
16 bit video buffer elements(register ax)
8 bits(ah) higher : 
  lower 4 bits - forecolor
  higher 4 bits - backcolor

8 bits(al) lower :
  8 bits : ASCII character to print
*/

uint16 vga_entry(unsigned char ch, uint8 fore_color, uint8 back_color)
{
    uint16 ax = 0;
    uint8 ah = 0, al = 0;

    ah = back_color;
    ah <<= 4;
    ah |= fore_color;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

// clear video buffer array
void clear_vga_buffer(uint16 **buffer, uint8 fore_color, uint8 back_color)
{
    uint32 i;
    for (i = 0; i < BUFFSIZE; i++) {
        (*buffer)[i] = vga_entry(NULL, fore_color, back_color);
    }
    next_line_index = 1;
    vga_index = 0;
}

// initialize vga buffer
void init_vga(uint8 fore_color, uint8 back_color)
{
    vga_buffer = (uint16*)VGA_ADRESS; // point vga_buffer to VGA_ADRESS
    clear_vga_buffer(&vga_buffer, fore_color, back_color); // clear buffer
    g_fore_color = fore_color;
    g_back_color = back_color;
}

// print functions
void print_new_line()
{
    if (next_line_index >= 55) {
        next_line_index = 0;
        clear_vga_buffer(&vga_buffer, g_fore_color, g_back_color);
    }
    vga_index = 80 * next_line_index;
    next_line_index++;
}

// assign ascii char to video buffer
void print_char(char ch)
{
    vga_buffer[vga_index] = vga_entry(ch, g_fore_color, g_back_color);
    vga_index++;
}

void delete_char()
{
    vga_buffer[vga_index - 1] = vga_entry(NULL, g_fore_color, g_back_color);
    vga_index--;
}

// print string by calling print_char
void print_string(char *str)
{
    uint32 index = 0;
    while (str[index]) {
        print_char(str[index]);
        index++;
    }
}

// print integer by converting it to a string
void print_int(int num)
{
    char str_num[digit_count(num + 1)];
    itoa(num, str_num);
    print_string(str_num);
}

// keyboard input functions
// get byte from port
uint8 inb(uint16 port)
{
    uint8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

void outb(uint16 port, uint8 data)
{
    asm volatile("outb %0, %1" : "=a"(data) : "d"(port));
}

char get_input_keycode()
{
    char ch = 0;
    while ((ch = inb(KEYBOARD_PORT)) != 0) {
        if (ch > 0)
            return ch;
    }
    return ch;
}

/*
keep the cpu busy for doing nothing(nop)
so that io port will not be processed by cpu
here timer can also be used, but lets do this in looping counter
*/

void wait_for_io(uint32 timer_count)
{
    while(1) {
        asm volatile ("nop");
        timer_count--;
        if (timer_count <= 0)
            break;
    }
}

void sleep(uint32 timer_count)
{
    wait_for_io(timer_count);
}

void control_input()
{
    char ch = 0;
    char keycode = 0;
    do {
        keycode = get_input_keycode();
        if (keycode == KEY_ENTER) {
            print_new_line();
        } 
        if (keycode == KEY_BACKSPACE){
            delete_char();
        } else {
            ch = get_ascii_char(keycode);
            print_char(ch);
        }
        sleep(0x02FFFFFF);
    } while (ch > 0);
}

void kernel_entry()
{
    // first init vga with fore & back colors
    init_vga(WHITE, RED);

    print_string("Digite algo:");
    print_new_line();
    control_input();
}
