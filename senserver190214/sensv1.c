#include "senserver.h" //S.ODA

#define MSG_FAILURE -1

#define MAX_MSGSIZE 1024
#define MAX_BUFSIZE (MAX_MSGSIZE + 1)

#define PORTNUM 8181

const char* cmds[] = {"get","proc_simple", /* toriaezu */
         "burst","stop","move","ip","blueget","bluemove","restart"}; // COMMAND

// senser's list
const char* sensors[] = {"accel", "gyro", "compass", "temperature", "humidity", "pressure"};

#define NSENSORS (sizeof(sensors)/sizeof(char*))

int main(int argc, char* argv[]) {

    const char *address = "";
    unsigned short port = PORTNUM; // (unsigned short)atoi(argv[1]);
    struct sockaddr servSockAddr, clitSockAddr;
    char recvBuffer[MAX_BUFSIZE];

    int server_sock = get_socket("udp");
    sockaddr_init(address, port, &servSockAddr);

    if (bind(server_sock, &servSockAddr, sizeof(servSockAddr)) < 0) {
        perror("bind() failed.");
        exit(EXIT_FAILURE);
    }

    while(1) {
		int nCmd;
        memset(recvBuffer, 0, MAX_BUFSIZE);
        int recvMsgSize = udp_receive(server_sock, recvBuffer, MAX_BUFSIZE, &clitSockAddr);
        if (recvMsgSize == MSG_FAILURE) continue;

      // printf("RECV: %s\n",recvBuffer);

		// Parse command
		for(nCmd = 0; nCmd < sizeof(cmds)/sizeof(char*); nCmd++) {
			if(strncmp(recvBuffer,cmds[nCmd],strlen(cmds[nCmd])) == 0 &&
				(isspace(recvBuffer[strlen(cmds[nCmd])]) || recvBuffer[strlen(cmds[nCmd])]== '\0')) {
            // printf("Success %s Command\n",cmds[nCmd]);
				switch(nCmd) {
				case 0: // get command
					procGet(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;

/*				case 1: // proc_simple ここはとりあえず作っている。
					procSimple(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;
*/
				case 2: // burst
					procBurst(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;

             case 3: // stop
                procStop(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
                break;

             case 4: // move
					procMove(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;

             case 5: // ip
					procIp(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;

             case 6:
                procblueGet(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;

             case 7: // move
					procblueMove(server_sock, recvBuffer, recvMsgSize, &clitSockAddr);
					break;

				default: // implemented but not work
					sprintf(recvBuffer,"NOT WORK:%s command!",cmds[nCmd]);
					udp_send(server_sock, recvBuffer, strlen(recvBuffer), &clitSockAddr); //
				}
				break;
			}
		}
		if(nCmd == sizeof(cmds)/sizeof(char*)) { // undefind command
			sprintf(recvBuffer,"UNKNOWN command!");
			udp_send(server_sock, recvBuffer, strlen(recvBuffer), &clitSockAddr); //
		}
    }
}

void procGet(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];

    if(buf[1] != NULL) {
        sprintf(sendBuffer,"get command: %s ",buf[1]);
    } else {
        sprintf(sendBuffer,"get command: any sensor not selected!");
    }
    if(buf[2] != NULL) {
        strcat(sendBuffer, buf[2]);
    }
    if(buf[1] != NULL && strncmp(buf[1],"accel",5) == 0) {
	    double *data = getAccel();
       sprintf(sendBuffer,"get accel %8.7f %8.7f %8.7f",data[0],data[1],data[2]);
	 }
    if(buf[1] != NULL && strncmp(buf[1],"gyro",5) == 0) {
	    double *data = getGyro();
       sprintf(sendBuffer,"get gyro %8.7f %8.7f %8.7f",data[0],data[1],data[2]);
	 }
    if(buf[1] != NULL && strncmp(buf[1],"compass",7) == 0) {
	    double *data = getCompass();
       sprintf(sendBuffer,"get compass %8.7f %8.7f %8.7f",data[0],data[1],data[2]);
	 }
    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

void procBurst(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];
    // check senser types

    int n = searchString(buf[1],sensors,NSENSORS);
    if(n < 0) {
        sprintf(sendBuffer,"Sensor %s not found!",buf[1]);
    } else { // fork & go
        double t;
        if(buf[2] != NULL && (t = atof(buf[2])) > 0.0) {
            pid_t pid;
            if((pid = fork()) == 0) { // CHILD
                switch(n) {
                case 0: //accel
                    burstAccel(t,server_sock,clitSockAddr);
                    break;

                case 1: //gyro
                    burstGyro(t,server_sock,clitSockAddr);
                    break;

                case 2: //compass
                    burstCompass(t,server_sock,clitSockAddr);
                    break;

                default:
                    break;
                }
                exit(0);
            } else { // parent
                if(pid < 0) {
                    sprintf(sendBuffer,"fork faild!!!");
                } else {
                    int status = 0;
                    addProcList(newProcList(pid));
                    sprintf(sendBuffer,"burst process [%d] started .",pid);
                }
            }
        } else {
            sprintf(sendBuffer,"burst mode needs 2nd parameter");
        }
    }
    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

void procStop(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];
    int status;

    siginfo_t siginfo;

    // check senser types

    pid_t id = atol(buf[1]);
    if(id <= 0 || searchProcList(id) == NULL) {
        sprintf(sendBuffer,"Illegal pid or pid[%d] not found.\n",id);
        udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
        free(buf[0]);
        free(buf);
        return;
    }
    kill(id,SIGHUP); // 
    waitpid(id,&status,0); //
    removeProcList(id);

    sprintf(sendBuffer,"Process [%d] terminated.\n",id);    
    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

void procMove(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];
    // check senser types

    int n = searchString(buf[1],sensors,NSENSORS);
    if(n < 0) {
        sprintf(sendBuffer,"Sensor %s not found!",buf[1]);
    } else { // fork & go
            pid_t pid;
            if((pid = fork()) == 0) { // CHILD
                switch(n) {
                case 0: //accel
                    moveAccel(server_sock,clitSockAddr);
                    break;

                default:
                    break;
                }
                exit(0);
            } else { // parent
                if(pid < 0) {
                    sprintf(sendBuffer,"fork faild!!!");
                } else {
                    int status = 0;
                    addProcList(newProcList(pid));
                    sprintf(sendBuffer,"burst process [%d] started .",pid);
                }
            }
    }
    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

void procIp(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];

    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;  // IPv4

    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);  // eth0

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    sprintf(sendBuffer,"%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

void procblueGet(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];

    if(buf[1] != NULL) {
        sprintf(sendBuffer,"get command: %s ",buf[1]);
    } else {
        sprintf(sendBuffer,"get command: any sensor not selected!");
    }
    if(buf[2] != NULL) {
        strcat(sendBuffer, buf[2]);
    }
    if(buf[1] != NULL && strncmp(buf[1],"accel",5) == 0) {
	    double *data = bluegetAccel();
       sprintf(sendBuffer,"blueget accel %8.7f %8.7f %8.7f",data[0],data[1],data[2]);
	 }
    
    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

void procblueMove(int server_sock, char *recvBuffer, int recvMsgSize, struct sockaddr *clitSockAddr) {
    char **buf = splitString(recvBuffer,10);
    char sendBuffer[MAX_BUFSIZE];
    // check senser types

    int n = searchString(buf[1],sensors,NSENSORS);
    if(n < 0) {
        sprintf(sendBuffer,"Sensor %s not found!",buf[1]);
    } else { // fork & go
            pid_t pid;
            if((pid = fork()) == 0) { // CHILD
                switch(n) {
                case 0: //accel
                    bluemoveAccel(server_sock,clitSockAddr);
                    break;

                default:
                    break;
                }
                exit(0);
            } else { // parent
                if(pid < 0) {
                    sprintf(sendBuffer,"fork faild!!!");
                } else {
                    int status = 0;
                    addProcList(newProcList(pid));
                    sprintf(sendBuffer,"burst process [%d] started .",pid);
                }
            }
    }
    udp_send(server_sock, sendBuffer, strlen(sendBuffer)+1, clitSockAddr); //
    free(buf[0]);
    free(buf);
}

// searchString 
int searchString(char *key,const char **lst, int lstLen) {
    int i = 0;
    while(i < lstLen) {
        if(strcmp(lst[i], key) == 0) return i;
        i++;
    }
    return -1;
}

//スペースもしくはナル文字で文字列を区切る。
char **splitString(char *s, int nMax) {
    if(s == NULL || *s == '\0') return NULL; // check null data

    char* s2 = (char*)malloc(MAX_BUFSIZE);
    strcpy(s2,s); // copy
    int n = 0;
    char **buf = (char **)calloc(nMax,sizeof(char*));

    while(*s2 != '\0') {
        buf[n] = s2;
        for(; *s2 != '\0' && !isspace(*s2); s2++) {}
        while(isspace(*s2)) {
            *s2 = '\0'; s2++;
        }
        n++;
        if(n >= nMax) return buf; // Over;
	}
//    printf("Success splitString %d items\n",n);
    return buf;
}
	


int get_socket(const char *type) {
    int sock;

    if (strcmp(type, "udp") == 0) {
        sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    } else if(strcmp(type, "tcp") == 0) {
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    }

    if (sock < 0){
        perror("socket() failed.");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void sockaddr_init (const char *address, unsigned short port, struct sockaddr *sockaddr) {

    struct sockaddr_in sockaddr_in;
    sockaddr_in.sin_family = AF_INET;

    if (inet_aton(address, &sockaddr_in.sin_addr) == 0) {
        if (strcmp(address, "") == 0 ) {
            sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            fprintf(stderr, "Invalid IP Address.\n");
            exit(EXIT_FAILURE);
        }
    }

    if (port == 0) {
        fprintf(stderr, "invalid port number.\n");
        exit(EXIT_FAILURE);
    }
    sockaddr_in.sin_port = htons(port);

    *sockaddr = *((struct sockaddr *)&sockaddr_in);
}

int udp_send(int sock, const char *data, int size, struct sockaddr *sockaddr) {
    int sendSize;
    sendSize = sendto(sock, data, size, 0, sockaddr, sizeof(*sockaddr));
    if (sendSize != size) {
        perror("sendto() failed.");
        return MSG_FAILURE;
    }
    return sendSize;
}

int udp_receive(int sock, char *buffer, int size, struct sockaddr *sockaddr) {
    unsigned int sockaddrLen = sizeof(*sockaddr);
    int receivedSize = recvfrom(sock, buffer, MAX_BUFSIZE, 0, sockaddr, &sockaddrLen);
    if (receivedSize < 0) {
        perror("recvfrom() failed.");
        return MSG_FAILURE;
    }

    return receivedSize;
}

void socket_close(int server) {
    if (close(server) < 0) {
        perror("close() failed.");
        exit(EXIT_FAILURE);
    }
}
