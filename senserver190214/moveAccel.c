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

#define XYSL 0.5
#define XYOFS 0.0

int accelFd;
double accel[3];

void moveAccel(int server_sock, struct sockaddr *clitSockAddr ) {
  short datax,datay,dataz;
  char sendBuffer[MAX_BUFSIZE];

  int counter = 0;

  int flagXP=0,flagXM=0;
  int flagYP=0,flagYM=0;

  // I2Cデバイスの初期化
  accelFd = wiringPiI2CSetup(DEVID);

  while(1){
    // 16ビットデータの読み込み(加速度)
    datax = wiringPiI2CReadReg16(accelFd,X);
    datay = wiringPiI2CReadReg16(accelFd,Y);
    dataz = wiringPiI2CReadReg16(accelFd,Z);

    // 12ビットデータに変換(加速度)
    datax = datax >> 4;
    datay = datay >> 4;
    dataz = dataz >> 4;

    // 実数に変換(加速度)
    accel[0] = datax * AXELPARAM;
    accel[1] = datay * AXELPARAM;
    accel[2] = dataz * AXELPARAM;

    if(accel[0] > XYSL) { //前後
        if(flagXP==0) {
            sprintf(sendBuffer,"%04d:XP",counter);
            flagXM = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
        flagXP++;
    } else if(accel[0] < - XYSL) {
        if(flagXM==0) {
            sprintf(sendBuffer,"%04d:XM",counter);
            flagXP = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
        flagXM++;
    } else {
        if(flagXP && accel[0] < XYOFS) {
            sprintf(sendBuffer,"%04d:X0",counter);
            flagXP = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
        if(flagXM && accel[0] > XYOFS) {
            sprintf(sendBuffer,"%04d:X0",counter);
            flagXM = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
    }

    if(accel[1] > XYSL) { //左右
        if(flagYP==0) {
            sprintf(sendBuffer,"%04d:YP",counter);
            flagYM = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
        flagYP++;
    } else if(accel[1] < - XYSL) {
        if(flagYM==0) {
            sprintf(sendBuffer,"%04d:YM",counter);
            flagYP = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
        flagYM++;
    } else {
        if(flagYP && accel[1] < XYOFS) {
            sprintf(sendBuffer,"%04d:Y0",counter);
            flagYP = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
        if(flagYM && accel[1] > XYOFS) {
            sprintf(sendBuffer,"%04d:Y0",counter);
            flagYM = 0;
            udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr);
        }
    }
 
    counter++;   
  }

  close(accelFd);
  accelFd=0;
 
}