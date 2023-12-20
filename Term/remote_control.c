#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios init_setting, new_setting;

void init_keyboard()
{
	tcgetattr(STDIN_FILENO, &init_setting);
	new_setting = init_setting;
	new_setting.c_lflag &= ~ICANON;
	new_setting.c_lflag &= ~ECHO;
	new_setting.c_cc[VMIN] = 0;
	new_setting.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &new_setting);
}

void close_keyboard()
{
	tcsetattr(0, TCSANOW, &init_setting);
}

char get_key()
{
	char ch = -1;

	if(read(STDIN_FILENO, &ch, 1) != 1)
		ch = -1;
	return ch;
}

void print_menu()
{
	printf("\n----------menu----------\n");
    printf("[o] : door open\n");
	printf("[q] : program exit\n");
	printf("------------------------\n\n");
}

int main(int argc, char **argv) {
    char command;
    int state = 1;

	int motor_dev = open("/dev/motor_driver", O_WRONLY);
    if (motor_dev == -1) {
        printf("Opening motor device was not possible!\n");
    }
    printf("Openig motor device was successful!\n");

	init_keyboard();
	print_menu();
	
    while(state == 1) {
        command = get_key();
        switch(command) {
            case 'o':
                write(motor_dev, '1', 1);
                break;
            
            case 'q':
                state = 0;
                break;
        }
    }
	close_keyboard();
}