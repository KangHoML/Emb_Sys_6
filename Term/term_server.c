#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

/*
* led를 통해 현재 파일의 전송 유무를 체크 (즉, 등록되지 않은 경우 led on)
* keyboard 또는 button을 통해 모터를 제어할 수 있도록 설계
*/

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
    int flag = 0;
    
    if (flag == 3) {
        int led_dev = open("dev/led_driver", O_WRONLY);
        if(led_dev == -1) {
            printf("Opening led device was not possible!\n");
            return -1;
        }
        printf("Openig led device was successful!\n");

        write(led_dev, '1', 1);
    }

}