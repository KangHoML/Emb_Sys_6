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

void led_controll(int led_dev, int flag) {
	char data;
	if (flag == 3) {
		data = '1';
	}
	else {
		data = '0';
	}
	write(led_dev, &data, 1);
}

void servo_controll(int servo_dev) {
    char servo_buffer;
    
	servo_buffer = 'X'; // degree : 90
	write(servo_dev, &servo_buffer, 1);

	sleep(5);

	servo_buffer = 'Y'; // degree : 0
	write(servo_dev, &servo_buffer, 1);   
}

int main(int argc, char **argv) {
    int flag = 0;
    char command;

	int led_dev = open("dev/led_driver", O_WRONLY);
    if(led_dev == -1) {
		printf("Opening led device was not possible!\n");
		return -1;
	}
	printf("Openig led device was successful!\n");

	int servo_dev = open("dev/pwm_driver", O_WRONLY);
    if (servo_dev == -1) {
        printf("Opening pwm device was not possible!\n");
    }
    printf("Openig pwm device was successful!\n");

	while(1) {
		led_controll(led_dev, flag);

		if (flag == 3) {
			init_keyboard();
			print_menu();

			command = get_key();
			if (command == 'o') {
				servo_controll(servo_dev);
			}

			close_keyboard();
		}
	}
}