#include <stdio.h> //printf(), fprintf(), perror(), getc()
#include <sys/socket.h> //socket(), sendto(), recvfrom()
#include <arpa/inet.h> // struct sockaddr_in, struct sockaddr, inet_ntoa(), inet_aton()
#include <stdlib.h> //atoi(), exit(), EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> //memset(), strcmp()
#include <unistd.h> //close()
#include <signal.h> //sigatcion()
#include <errno.h> //erron, EINTR

#define MSG_FAILURE -1
#define MAX_MSGSIZE 1024
#define MAX_BUFSIZE (MAX_MSGSIZE + 1)
#define MAX_TRIES 5
#define TIMEOUT_SECOND 2

int  get_socket(const char *);
void sockaddr_init (const char *, unsigned short, struct sockaddr *);
int  udp_send(int, const char *, int, struct sockaddr *);
int  udp_receive(int, char *, int, struct sockaddr *);
void socket_close(int);
int  input(char *, int);
void remove_lf(char *, int);
void sigaction_init(struct sigaction *, void (*)(int));
void catchAlarm(int);
//int  udp_receive (int, struct sockaddr *, struct sockaddr *, char *);
int  check_correct_server (struct sockaddr *, struct sockaddr *);

int intTries = 0;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "argument count mismatch error.\n");
        exit(EXIT_FAILURE);
    }

    const char *address = argv[1];
    unsigned short port = (unsigned short)atoi(argv[2]);
    struct sockaddr servSockAddr, clitSockAddr;
    struct sigaction action;

    int server_sock = get_socket("udp");
    sockaddr_init(address, port, &servSockAddr);

    sigaction_init(&action, catchAlarm);
    if (sigaction(SIGALRM, &action, NULL) < 0) {
        perror("sigaction() failure");
        exit(EXIT_FAILURE);
    }

    if(fork() == 0) {
        while(1){
            char receiveBuffer[MAX_BUFSIZE];

            memset(receiveBuffer,0,MAX_BUFSIZE); // S.ODA
            int receivedSize = udp_receive(server_sock, receiveBuffer, MAX_BUFSIZE,& clitSockAddr);

            if (check_correct_server(&servSockAddr, &clitSockAddr) == -1) {

                continue;
            }

            remove_lf(receiveBuffer, receivedSize);

            printf("server return: %s\n", receiveBuffer);
        }
    } else {
        while(1) {
            char sendBuffer[MAX_BUFSIZE];
            int inputSize = input(sendBuffer, MAX_BUFSIZE);

            if (strcmp(sendBuffer, "quit\n") == 0) {
                socket_close(server_sock);
                break;
            }
            int sendedSize = udp_send(server_sock, sendBuffer, inputSize, &servSockAddr);

        }
    }

    return EXIT_SUCCESS;
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

int input(char *buffer, int size) {
    printf("please enter the characters:");

    if (fgets(buffer, size, stdin) == NULL){
        fprintf(stderr, "invalid input string.\n");
        exit(EXIT_FAILURE);
    }

    //flush the stdin buffer
    if (buffer[strlen(buffer)-1] != '\n') {
        int c;
        while((c = getc(stdin) != '\n') && (c != EOF)){}
    }
 
    return strlen(buffer);
}

void remove_lf(char *buffer, int bufferSize) {
    buffer[bufferSize-1] = '\0';
}

void catchAlarm (int ignored) {
    intTries += 1;
}

void sigaction_init(struct sigaction *action, void (*handler)(int) ) {
    action->sa_handler = handler;
    if (sigfillset(&(action->sa_mask)) < 0) {
        perror("sigfillset() failure");
        exit(EXIT_FAILURE);
    }
    action->sa_flags = 0;
}
/*
int udp_receive (int sock, struct sockaddr *servSockAddr, struct sockaddr *clitSockAddr,  char *sendBuffer, char *receiveBuffer) {

    int receivedSize;
    while (1) {
        alarm(TIMEOUT_SECOND);
        receivedSize = udp_receive(sock, receiveBuffer, MAX_BUFSIZE, clitSockAddr);
        if (receivedSize == MSG_FAILURE) {
            if (errno == EINTR) {
                if (intTries <= MAX_TRIES) {
                    printf("timed out %d.\n", intTries);
                    sendedSize = udp_send(sock, sendBuffer, sendSize, servSockAddr);
                    if (sendedSize == MSG_FAILURE) break;
                    alarm(TIMEOUT_SECOND);
                    continue;
                } else {
                    printf("total timed out %d.\n", MAX_TRIES);
                    exit(EXIT_FAILURE);
                }
            } else {
                exit(EXIT_FAILURE);
            }
        }
        break;
    }
    alarm(0);

    return receivedSize;
}*/

int check_correct_server (struct sockaddr *sockaddr_1, struct sockaddr *sockaddr_2) {
    if( ((struct sockaddr_in *)sockaddr_1)->sin_addr.s_addr != ((struct sockaddr_in *)sockaddr_2)->sin_addr.s_addr ) {
        fprintf(stderr, "reveiceid from unknown server.\n");
    } else if (ntohs(((struct sockaddr_in *)sockaddr_1)->sin_port) != ntohs(((struct sockaddr_in *)sockaddr_2)->sin_port)) {
        fprintf(stderr, "reveiceid from unknown port.\n");
    } else {
        return EXIT_SUCCESS;
    }

    return MSG_FAILURE;
}
