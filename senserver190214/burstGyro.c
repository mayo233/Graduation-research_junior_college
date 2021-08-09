#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<wiringPiI2C.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include "senserver.h"


#define MAX_MSGSIZE 1024
#define MAX_BUFSIZE (MAX_MSGSIZE + 1)

#define DEVID 0x69
#define X 0x02
#define Y 0x04
#define Z 0x06

#define GYROPARAM (2000.0 / 65536)

int gyroFd;
double gyro[3];

void burstGyro(double timing, int server_sock, struct sockaddr *clitSockAddr ) {
    short datax,datay,dataz;
    char sendBuffer[MAX_BUFSIZE];

    int tim_sec = (int)timing;
    int tim_usec = (int)((timing - tim_sec) * 1000000);

    int counter = 0;

    // I2Cデバイスの初期化
    gyroFd = wiringPiI2CSetup(DEVID);  

    while(1) {
        // 16ビットデータの読み込み(gyro)
        datax = wiringPiI2CReadReg16(gyroFd,X);  
        datay = wiringPiI2CReadReg16(gyroFd,Y);
        dataz = wiringPiI2CReadReg16(gyroFd,Z);

        gyro[0] = datax * GYROPARAM;
        gyro[1] = datay * GYROPARAM;
        gyro[2] = dataz * GYROPARAM;  

        sprintf(sendBuffer,"gyro:%d:%.3f,%.3f,%.3f",counter,gyro[0],gyro[1],gyro[2]);

        udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
        if(tim_sec>0) sleep(tim_sec);
        else usleep(tim_usec);
        counter++;
    }

    close(gyroFd);
    gyroFd=0;
}
