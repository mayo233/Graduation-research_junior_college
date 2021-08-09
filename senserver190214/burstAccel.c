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

#define DEVID 0x19
#define X 0x02
#define Y 0x04
#define Z 0x06

#define AXELPARAM (2.0 / 4096)

int accelFd;
double accel[3];

void burstAccel(double timing, int server_sock, struct sockaddr *clitSockAddr ) {
    short datax,datay,dataz;
    char sendBuffer[MAX_BUFSIZE];

    int tim_sec = (int)timing;
    int tim_usec = (int)((timing - tim_sec) * 1000000);

    int counter = 0;

    // I2Cデバイスの初期化
    accelFd = wiringPiI2CSetup(DEVID);  

    while(1) {
        // 16ビットデータの読み込み(加速度)
        datax = wiringPiI2CReadReg16(accelFd,X);  
        datay = wiringPiI2CReadReg16(accelFd,Y);
        dataz = wiringPiI2CReadReg16(accelFd,Z);

        // 12ビットデータに変換(加速度)
        datax = datax >> 4;
        datay = datay >> 4;
        dataz = dataz >> 4;

        accel[0] = datax * AXELPARAM;
        accel[1] = datay * AXELPARAM;
        accel[2] = dataz * AXELPARAM;  

        sprintf(sendBuffer,"accel:%d:%.3f,%.3f,%.3f",counter,accel[0],accel[1],accel[2]);

        udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
        if(tim_sec>0) sleep(tim_sec);
        else usleep(tim_usec);
        counter++;
    }

    close(accelFd);
    accelFd=0;
}
