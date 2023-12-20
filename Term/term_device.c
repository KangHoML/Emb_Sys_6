#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

char lcd_buffer[17];

void lcd_control(int lcd_dev, int flag) {
    if (flag == 1 || flag == 2 || flag == 3) {
        strcpy(lcd_buffer, "Recognized");
    } else if (flag == 0) {
        strcpy(lcd_buffer, "Unrecognized");
    } else {
        strcpy(lcd_buffer, "CLEAR_LCD");
    }

    write(lcd_dev, lcd_buffer, strlen(lcd_buffer));
}

void motor_control(int motor_dev, int flag, int client_fd) {
    char ch = '1';
    if (flag == 1 || flag == 2 || flag == 3) {
        write(motor_dev, &ch, 1);
    }
}

int main(int argc, char **argv) {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addr_len = sizeof(address);
    char buffer[BUFFER_SIZE];
    int flag;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    int lcd_dev = open("/dev/lcd_driver", O_WRONLY);
    int motor_dev = open("/dev/motor_driver", O_WRONLY);
    int count = 0;

    while(1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addr_len);
        if (client_fd < 0) {
            perror("accept");
	    sleep(1);
            continue;
        }

	// printf("Current: %d, Prev: %d\n", flag, prev_flag);

        while(1) {
	    memset(buffer, 0, sizeof(buffer));
            int read_bytes = read(client_fd, buffer, BUFFER_SIZE);

            if (read_bytes <= 0) {
		// printf("Current: %d, Prev: %d\n\n", flag, prev_flag);
                break;
            }

            sscanf(buffer, "%d", &flag);
	    count += 1;
	    if (count == 2) {
		    printf("flag: %d\n", flag);
		    lcd_control(lcd_dev, flag);
		    motor_control(motor_dev, flag, client_fd);
		    flag = -1;
		    count = 0;
	    }
        }

        close(client_fd);
    }

    close(motor_dev);
    close(lcd_dev);
    close(server_fd);

    return 0;
}
