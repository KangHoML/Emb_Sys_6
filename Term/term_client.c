#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

char lcd_buffer[17];

void servo_run(int servo_dev, int flag) {
    char servo_buffer;
    
    if (flag == 0 || flag == 1 || flag == 2) {
        servo_buffer = 'X'; // degree : 90
        write(servo_dev, &servo_buffer, 1);

        sleep(5);

        servo_buffer = 'Y'; // degree : 0
        write(servo_dev, &servo_buffer, 1);   
    }
}

void lcd_print(int lcd_dev, int flag) {
    if (flag == 0 || flag == 1 || flag == 2) {
        strcpy(lcd_buffer, "Recognized");
    }
    else if (flag == 3) {
        strcpy(lcd_buffer, "Unrecognized");
    }
    else {
        strcpy(lcd_buffer, "CLEAR_LCD");
    }

    write(lcd_dev, lcd_buffer, sizeof(lcd_buffer));
}

int main(int argc, char **argv) {
    /*
    * 카메라와 opencv를 통해 일정 거리 이상일 경우 인식 X -> flag = 아무 값(9)
    * 인식한 경우 tensorflowlite모델을 통해 얼굴 인식 후 flag = 0, 1, 2, 3(Unknown)

    * flag == 아무 값(9)인 경우) lcd_clear
    * flag == 0, 1, 2인 경우) "Recognized" 출력, motor on (일정 시간 후 다시 off)
    * flag == 3인 경우) "Unrecognized" 출력, motor off
    *                   사진 찍고 현재 시간으로 파일명 저장하여 server pc에 전송
    */
    int flag = 0;
    
    int lcd_dev = open("dev/lcd_driver", O_WRONLY);
    if(lcd_dev == -1) {
        printf("Opening lcd device was not possible!\n");
        return -1;
    }
    printf("Openig lcd device was successful!\n");

    int servo_dev = open("dev/pwm_driver", O_WRONLY);
    if (servo_dev == -1) {
        printf("Opening pwm device was not possible!\n");
    }
    printf("Openig pwm device was successful!\n");

    while(1) {
        lcd_print(lcd_dev, flag);
        servo_run(servo_dev, flag);
    }

    close(lcd_dev);
    close(servo_dev);
    return 0;
}