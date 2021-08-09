#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PIC "/dev/ttyUSB1"
#define SERIAL_SRAVE "/dev/ttyUSB0"


int main(int argc, char *argv[])
{
    //unsigned char msg[] = "serial port open...\n";
    unsigned char buf[255];             // バッファ
    int fd;                             // ファイルディスクリプタ
    struct termios tio;                 // シリアル通信設定
    int baudRate = B115200;
    int i;
    int len;
    int ret;
    int size;
    //cuコマンドの実行
    //char *command="cu -l /dev/ttyUSB1 -s 115200";
    //int res;

    fd = open(SERIAL_PIC, O_RDWR);     // デバイスをオープンする
    //res=system(command);
    //printf("%d\n",WIFEXITED(res));
    //printf("%d\n",WEXITSTATUS(res));
    
    if (fd < 0) {
        printf("open error\n");
        return -1;
    }

    tio.c_cflag += CREAD;               // 受信有効
    tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;                 // データビット:8bit
    tio.c_cflag += 0;                   // ストップビット:1bit
    tio.c_cflag += 0;                   // パリティ:None

    cfsetispeed( &tio, baudRate );
    cfsetospeed( &tio, baudRate );

    cfmakeraw(&tio);                    // RAWモード

    tcsetattr( fd, TCSANOW, &tio );     // デバイスに設定を行う

    ioctl(fd, TCSETS, &tio);            // ポートの設定を有効にする

    // 送受信処理ループ
    while(1) {
        len = read(fd, buf, sizeof(buf));
        if (0 < len) {
            for(i = 0; i < len; i++) {
                printf("%c", buf[i]);
            }
           
            printf("\n");
            //__delay_ms(5000);
        }

        // エコーバック
        write(fd, buf, len);
    }

    close(fd);
                             // デバイスのクローズ

    return 0;
     
}