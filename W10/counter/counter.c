#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static struct termios init_setting, new_setting;

char seg_num[10] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8, 0x80, 0x90};
char seg_dnum[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x58, 0x00, 0x10};

#define D1 0x01
#define D2 0x02
#define D3 0x04
#define D4 0x08

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
    printf("[u] : count up\n");
    printf("[d] : count down\n");
	printf("[p] : set counter\n");
	printf("[q] : program exit\n");
	printf("------------------------\n\n");
}

void print_seg(unsigned short *data, int count) {
    int index0 = count / 1000;
    int index1 = (count % 1000) / 100;
    int index2 = (count % 100) / 10;
    int index3 = (count % 100) % 10;

    data[0] = (seg_num[index0] << 4) | D1;
    data[1] = (seg_num[index1] << 4) | D2;
	data[2] = (seg_num[index2] << 4) | D3;
	data[3] = (seg_num[index3] << 4) | D4;
}

int main(int argc, char **argv) {
    // variable values for reading button
    int button_dev = open("/dev/my_button", O_RDONLY);
    char buff[2];
    char prev[2] = {'r', 'r'};
    
    // variable values for writing segment
    int seg_dev = open("/dev/my_segment", O_WRONLY);
    
    // Verify to state of device opened
    if(button_dev == -1) {
        printf("Opening button device was not possible!\n");
        return -1;
    }
    printf("Openig button device was successful!\n");

    if(seg_dev == -1) {
        printf("Opening segment device was not possible!\n");
        return -1;
    }
    printf("Openig segment device was successful!\n");

    // variable values for counter
    int state = 1;
    int digit_num = 0;
    unsigned short data[4];
    char command;
    int count = 0;
    int delay_time = 1000;

    // initialize the setting
    init_keyboard();
    print_menu();
    
    while(state == 1) {
        command = get_key();
        switch(command) {
            case 'u':
                count = (count + 1) % 10000;
                break;

            case 'd':
                count = (count - 1 + 10000) % 10000;
                break;

            case 'p':
                {
                    unsigned short off_data[4] = {0};

                    printf("input of couter value: ");
                    char keys[4];
                    int i = 0;
                    char key;

                    while((key = get_key()) != '\n') {
                        keys[i] = key;
                        i++;
                    }
                    
                    count = 0;
                    for (int j = 0; j < 4; j++) {
                        count = count * 10 + (keys[j] - '0');
                    }

                    write(seg_dev, off_data, sizeof(off_data));
                    break;
                }

            case 'q':
                state = 0;
                break;
        }

        read(button_dev, &buff, sizeof(buff));

        if (buff[0] != prev[0]) {
            if (buff[0] == '1') {
                count = (count + 1) % 10000;
            }
        }
        prev[0] = buff[0];

        if (buff[1] != prev[1]) {
            if (buff[1] == '1') {
                count = (count - 1 + 10000) % 10000;
            }
        }
        prev[1] = buff[1];

        print_seg(data, count);
        write(seg_dev, &data[digit_num], sizeof(data));
        usleep(delay_time);
        
        digit_num++;
        if (digit_num > 3) {
            digit_num = 0;
        }
    }

    close_keyboard();
    write(seg_dev, 0x0000, 2);
    close(button_dev);
    close(seg_dev);
    return 0;
}