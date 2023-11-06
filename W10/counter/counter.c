#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

int main(int argc, char **argv) {
    bool state = true;

    // variable values for reading button
    int button_dev = open("/dev/my_button", O_RDONLY);
    char buff[2];
    char prev[2] = {'r', 'r'};
    
    // variable values for writing segment
    char command;
    unsigned short count = 0;
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

    // counter
    while(state) {
        switch(command) {
            case 'u':
                count = (count + 1) % 10000;
                break;
            case 'd':
                count = (count - 1 + 10000) % 10000;
                break;
            case 'p':
                printf("input of couter value: ");
                scanf("%hu", &count);
                count %= 10000;
                break;
            case 'q':
                state = false;
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

        write(seg_dev, &count, sizeof(count));
    }

    close(button_dev);
    close(seg_dev);
    return 0;
}