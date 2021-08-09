// I2Cマスターモードの基本プログラム：クロック32M仮定
#include<xc.h>
#include "I2CMaster.h"

void initI2C(unsigned char flag) { // if flag==true speed = 100khz
    unsigned char stat = 0x00; // 400kHz
    unsigned char baud = 0x13; // 400KHz時の BRGの設定値 
    if(flag) {
        stat = 0x80; // 100kHz
        baud = 0x4f; // 100kHz時のBRGの設定値
    }
    SSP1STAT = stat;
    SSP1CON1 = 0x28; // enable SCI,SDA pin & I2C Master Mode
    SSP1ADD = baud;  // スピードの設定
}

// I2Cデバイスへの書き込み
void sendI2C(unsigned char addr,unsigned char *buf, unsigned char n) {
    SSP1CON2bits.SEN = 1;
    while(SSP1CON2bits.SEN==1); // スタート出力待ち
    SSP1BUF = addr << 1;   // アドレスと書き込みモードを送信
    while(SSP1STATbits.BF==1); // アドレス出力待ち
    while(SSP1CON2bits.ACKSTAT==1); // ACK待ち：スレーブがいなかったらどうなる？
    waitIdleI2C();
    
    while(n) { // nバイト送信
        SSP1BUF = *buf++; // 一バイト送信
        n--;
        while(SSP1STATbits.BF); // アドレス出力待ち
        waitIdleI2C();
    }
    SSP1CON2bits.PEN = 1; // ストップビット出力
    while(SSP1CON2bits.PEN); // ストップビット出力待ち
}

void sendI2Cbyte(unsigned char addr,unsigned char reg, unsigned char data) {
    SSP1CON2bits.SEN = 1;
    while(SSP1CON2bits.SEN==1); // スタート出力待ち
    SSP1BUF = addr << 1;   // アドレスと書き込みモードを送信
    while(SSP1STATbits.BF==1); // アドレス出力待ち
    while(SSP1CON2bits.ACKSTAT==1); // ACK待ち：スレーブがいなかったらどうなる？
    waitIdleI2C();
 
    // レジスタ番号
    SSP1BUF = reg; // 一バイト送信    
    while(SSP1STATbits.BF); // アドレス出力待ち
    waitIdleI2C();

    // データ
    SSP1BUF = data; // 一バイト送信    
    while(SSP1STATbits.BF); // アドレス出力待ち
    waitIdleI2C();
    
    SSP1CON2bits.PEN = 1; // ストップビット出力
    while(SSP1CON2bits.PEN); // ストップビット出力待ち
}
// I2Cデバイスからの読み出し
int readI2C(unsigned char addr, unsigned char *buf,unsigned char n) {
    unsigned char num = 0;
    SSP1CON2bits.SEN = 1;
    while(SSP1CON2bits.SEN); // スタート出力待ち
    SSP1BUF = (addr << 1) | 0x01;   // アドレスと読み込みモードを送信
    while(SSP1STATbits.BF); // アドレス出力待ち
    waitIdleI2C();
    
    while(n) {
        if(n <= 1)  SSP1CON2bits.ACKDT = 1;         // NACK (最終バイト時)
        else        SSP1CON2bits.ACKDT = 0;         // ACK
        SSP1CON2bits.RCEN = 1;     // 受信可能にする。
        while(! SSP1STATbits.BF); //受信が終了するのを待つ
        SSP1CON2bits.ACKEN = 1;        // ACKを送信
        *buf++ = SSP1BUF;
        num++;
        n--;
        SSP1CON1bits.SSPOV = 0;    // エラークリア
        waitIdleI2C();
    }
    SSP1CON2bits.PEN = 1;
    while(SSP1CON2bits.PEN);
    return num;
}

void waitIdleI2C(void) {
    while(SSP1STATbits.R_nW || SSP1CON2bits.ACKEN ||
            SSP1CON2bits.RCEN || SSP1CON2bits.SEN || SSP1CON2bits.PEN);
}
