#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "senserver.h"

#define MAX_MSGSIZE 1024
#define MAX_BUFSIZE (MAX_MSGSIZE + 1)

#define AXELPARAM (2.0 / 4096)

#define XYSL 0.5
#define XYOFS 0.0

void Number2(char *nptrx1,char *nptrx2,char *nptry1,char *nptry2,char *nptrz1,char *nptrz2,int *hairetu);
int xyz[6];

unsigned char xtmp[6];
double accel[3];

unsigned char *sub2(){
	char buff[1024];
	char data[17];
	char dataX1[3];
	char dataX2[3];
	char dataY1[3];
	char dataY2[3];
	char dataZ1[3];
	char dataZ2[3];
	int x1,x2,y1,y2,z1,z2;
	int sec =1;
	int i=0;
	int res;
	
	
	FILE *fp = popen("gatttool -b 00:1e:c0:55:40:b5 --char-read -a 0x0018","r");
	while(fgets(buff,sizeof(buff),fp) != NULL){ 
	}
	

	sscanf(buff+33,"%x %x %x %x %x %x",&x1,&x2,&y1,&y2,&z1,&z2);

	xtmp[0] = x1;
	xtmp[1] = x2;
	xtmp[2] = y1;
	xtmp[3] = y2;
	xtmp[4] = z1;
	xtmp[5] = z2;
	
	pclose(fp);

	return xtmp;
}

void Number2(char *nptrx1,char *nptrx2,char *nptry1,char *nptry2,char *nptrz1,char *nptrz2,int *hairetu){
	 int x1,x2,y1,y2,z1,z2;

	sscanf(nptrx1,"%x",&x1);
	hairetu[0]=x1;
	
	sscanf(nptrx2,"%x",&x2);
	hairetu[1]=x2;
	
	sscanf(nptry1,"%x",&y1);
	hairetu[2]=y1;
	
	sscanf(nptry2,"%x",&y2);
	hairetu[3]=y2;
	
	sscanf(nptrz1,"%x",&z1);
	hairetu[4]=z1;
	
	sscanf(nptrz2,"%x",&z2);
	hairetu[5]=z2;
}


void bluemoveAccel(int server_sock, struct sockaddr *clitSockAddr){
    unsigned char *xt;
    short data[5];
    char sendBuffer[MAX_BUFSIZE];
    int i;

    int counter = 0;

    int flagXP=0,flagXM=0;
    int flagYP=0,flagYM=0;

    while(1){
        xt = sub2();

        for(i=0;i<6;i++){
            data[i] = (unsigned char)xt[i];
        }
     
        accel[0] = ((data[1] * 256) + (data[0] & 0xF0)) / 16;
        if(accel[0] > 2047) accel[0] -= 4096;
        accel[1] = ((data[3] * 256) + (data[2] & 0xF0)) / 16;
        if(accel[1] > 2047) accel[1] -= 4096;
        accel[2] = ((data[5] * 256) + (data[4] & 0xF0)) / 16;
        if(accel[2] > 2047) accel[2] -= 4096;
 	
        accel[0] = accel[0] * AXELPARAM;
        accel[1] = accel[1] * AXELPARAM;
        accel[2] = accel[2] * AXELPARAM; 

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
}	
