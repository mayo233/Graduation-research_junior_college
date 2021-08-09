#ifndef __SENSERVER_H__
#define __SENSERVER_H__
#include <stdio.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h> // S.ODA
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>

typedef struct PROCLIST {
    pid_t pid;
    struct PROCLIST *next;
} ProcList;    

extern ProcList *plist;

ProcList *newProcList(pid_t id);
void addProcList(ProcList *p);
ProcList *searchProcList(pid_t id);
ProcList *removeProcList(pid_t id);

int  get_socket(const char *);
void sockaddr_init (const char *, unsigned short, struct sockaddr *);
int  udp_send(int, const char *, int, struct sockaddr *);
int  udp_receive(int, char *, int, struct sockaddr *);
void socket_close(int);

char **splitString(char *s, int nMax);
int  searchString(char *, const char **,int );
void procGet(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procSimple(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procBurst(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procStop(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procMove(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procIp(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procblueGet(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
void procblueMove(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr);
double *getAccel();
double *getGyro();
double *getCompass();
void burstAccel(double timing, int server_sock, struct sockaddr *clitSockAddr);
void burstGyro(double timing, int server_sock, struct sockaddr *clitSockAddr);
void burstCompass(double timing, int server_sock, struct sockaddr *clitSockAddr);
void moveAccel(int server_sock, struct sockaddr *clitSockAddr);
void bluemoveAccel(int server_sock, struct sockaddr *clitSockAddr);
double *bluegetAccel();


#endif