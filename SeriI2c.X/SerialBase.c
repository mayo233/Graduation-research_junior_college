// PIC16F1705 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTOSC       // Oscillator Selection (oscillator: I/O function on CLKIN pin)
#pragma config WDTE = ON        // Watchdog Timer Disable /Enable (WDT enabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
//#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF        // Internal/External Switchover (Internal/External Switchover mode is enabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
#pragma config ZCDDIS = ON      // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR)
#pragma config PLLEN = ON       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "I2CMaster.h"
//#include "BMX055.h"

#define _XTAL_FREQ 32000000

#include "serial.h"
// #include "aqm1602.h"
/*
#define MaxSize 32

// ??????????????????
void copyBuffer(void); // ????????????????????????????????????????????????????????????????????????????????????????????????????????????

void Send(unsigned char),doSend(void),SendStr(void unsigned char );
void copyConstStr(const unsigned char*,unsigned char*);
void SendHex(unsigned char);
char QueIn(unsigned char);
unsigned char QueOut(void);
void trySend();
void encodeBase64(unsigned char *src,unsigned char n,unsigned char *dst );
Byte cmdCheck(unsigned char *b, const unsigned char *c);
Byte *skip2blank(Byte *b);
void perfGetCmd(Byte *x);

void perfSetCmd(Byte *x);

//void interrupt isr(void);
*/
//const unsigned char msga[] = "A:";
//const unsigned char msgc[] = "C:";
const unsigned char msghi[] = "Hi!\r\n";
//unsigned char msghi[] = "Hi!\r\n";

//const unsigned char msgi2c[] = "I2C:";
//const unsigned char msgAcc[] = "ACC:";
//const unsigned char msgGyr[] = "GYR:";
//const unsigned char msgCOM[] = "CPS:";



// 
unsigned char Index,Tindex,Sending;// ????????????????????????????????????????????????????????????????????????
unsigned char RcvFlag,TxFlag;
unsigned char RcvBuf[MaxSize]; // ??????????????????
unsigned char SndBuf[MaxSize]; // ??????????????????
unsigned char SBuff[MaxSize] ; // ???????????????
unsigned char Buffer[MaxSize]; // ??????????????????
// unsigned char i2cBuf[MaxSize];
unsigned char echoFLG;

// #define doSend()  ;
void parseCommand();

void __interrupt() isr(void) {
    unsigned char data;
    // int i;
    if(PIR1bits.RCIF) {
        PIR1bits.RCIF = 0;
        if((RC1STAbits.OERR) || (RC1STAbits.FERR)){ // ???????????????
            data = RC1REG;
            RC1STA = 0;
            RC1STA = 0x90;
        } else {
            data = RC1REG;
            // for debug 
            if(Index < MaxSize) {
                RcvBuf[Index] = data;
                Index++;
            } else {
                Index = 0;
                RcvBuf[Index] = data;
                Index++;
            }
			if(echoFLG) {
				Send(data);
				if(data == '\b' && Index != 0) {
					if(Index == 1) Index = 0;
					else Index -= 2;
				}
			}
            if(data == '\r' || data == '\n' ) {
				RcvFlag = 1;
				if(echoFLG) SendCRLF();
			}
        }
    }
}

// void rn4020Setup();

int main(void) {
    char bring = 1;
    unsigned int loop=30000;
    
    OSCCON = 0xF0;     // ?????????MHz, PLL on 32MHz
    // APFCON = 0x00;    // TX,RX=RC4,RC5 ????????????????????????
    ANSELA = 0x00;     // PORTA ????????????????????????
    ANSELC = 0x00;     // PORTC ????????????????????????
    TRISA  = 0xFF;     // PORTA ??????????????????
    TRISC  = 0xF7;     // PORTC RC3?????????????????????????????????
    WPUA   = 0x3F;     // PORTA ????????????????????????
    WPUC   = 0x0F;     // PORTC ?????????????????????RC0-RC3?????????

    // PPS
    RC4PPS = 0b00010100; // RC4 is TX/CK
    RXPPS  = 0b00010101; // RX is RC5
    RC0PPS = 0b00010000; // RC0 is i2c SCL
    RC1PPS = 0b00010001; // RC1 is i2c SDA

    // USART??????
    TX1STA  = 0x24;     // ??????????????????Async,
    RC1STA  = 0x90;     // ??????????????????
    BAUD1CON= 0x08;     // BRG16 ON
    SP1BRG  = 68;       //115kbps 

    Tindex = 0;
    Sending = 0;
    TxFlag = 0;
    // WDT 
    // __delaywdt_ms(1000); // ??????????????????????????????????????????
    WDTCON = 0x16;       // ??????????????????????????????2???
    CLRWDT();            // ?????????????????????????????????????????????

    Index = 0;
    RcvFlag = 0;
    //UART ???????????? 
    PIR1bits.RCIF = 0;
    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
      initI2C(0);  // I2C????????????????????? 400kHz; 
//      initI2C(0);  // I2C????????????????????? 100kHz; 
    
    __delay_ms(100);
	echoFLG = 0;    // echo OFF
//	echoFLG = 1;    // echo ON

//     aqm1602Setup();
    __delay_ms(100);
    // sendI2Cbyte(DISPLAY_ADDR,DISPLAY_DAAD,'X');
    // acm_displayCharacter('A');
    // displayString("AQM1602 Start!\n");
    // rn4020Setup();
    
    SendConstStr(msghi);
    
    while(1) {
        CLRWDT();            // ?????????????????????????????????????????????
        if(++loop >= 30000) {
            PORTCbits.RC3 = bring;  // LED ????????????
            bring = 1 - bring;
            loop=0;
        }
        trySend(); // ???????????????????????????doSend(); doSend();
        if(RcvFlag) {
			copyBuffer();
			parseCommand();
			RcvFlag = 0;
        }
    }
    return 0;
}

void copyBuffer() { // ??????????????????
    INTCONbits.GIE = 0;
    unsigned char *p = RcvBuf;
    unsigned char *q = Buffer;
    while(Index) {
        *q++ = *p++;
        Index --;
    }
    *q = '\0';
    INTCONbits.GIE = 1; // ????????????OK
}

void doSend() {
    if(TxFlag) {
        while(!TX1STAbits.TRMT); // ??????????????????
        TX1REG = QueOut();       // Que ???????????????1??????????????????
    }
}

void trySend() { // ???????????????????????????????????????
    if(TxFlag && TX1STAbits.TRMT) TX1REG = QueOut();
}

char QueIn(unsigned char c) {
    if(TxFlag >= MaxSize) return 0; // Que???????????????
    SBuff[Tindex++] = c;
    if(Tindex >= MaxSize) Tindex = 0;
    TxFlag++;
    return 1;
}

unsigned char QueOut() { // TxFlag > 0 ?????????
    unsigned char c = SBuff[Sending++];
    if(Sending >= MaxSize) Sending = 0;
    TxFlag --;
    return c;
}

void Send(unsigned char txchar) {
    if(TxFlag == 0) {
        if(TX1STAbits.TRMT) TX1REG = txchar;
        else {
            Sending = Tindex = 0;
            SBuff[Tindex++] = txchar;
            TxFlag = 1;
        }
    } else {
        if(! QueIn(txchar)) { // ???????????????????????????????????????????????????
            while(!TX1STAbits.TRMT); // ??????????????????
            TX1REG = QueOut(); // ?????????????????????????????????
            QueIn(txchar);
        }
        if(TX1STAbits.TRMT) TX1REG = QueOut();
    }
    // for DEBUG
    //    acm_displayCharacter(txchar);
}

void SendStr() {
    unsigned char *str=SndBuf;
    char i = MaxSize;
    while(i > 0 && *str != '\0') {
        Send(*str);
        str++;
        i--;
    }
}

void sndStr(unsigned char *p) {
	unsigned char *q = SndBuf;
	while(*p) *q++ = *p++;
	*q = '\0';
	SendStr();
}

void SendHex(unsigned char b) {
    unsigned char c = b >> 4;
    if(c <= 9) c += '0';
    else c += 'A' - 10;
    Send(c);
    c = b & 0x0f;
    if(c <= 9) c += '0';
    else c += 'A' - 10;
    Send(c);
}

void copyConstStr(const unsigned char *src, unsigned char * dst) {
    while(*src) {
        *dst++ = *src++;
    }
    *dst = '\0';
}

const unsigned char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 

void encodeBase64(unsigned char *src,unsigned char n,unsigned char *dst ) {
    unsigned char ph = 0;
    unsigned char c;
    unsigned short d = 0;
    while(n > 0) {
        d |= *src++;
        n--;
        switch(ph) {
        case 0:
            c = d >> 2;
            d <<= 8;
            d &= 0x0300;
            ph++;
            break;
        case 1:
            c = d >> 4;
            d <<= 8;
            d &= 0x0f00;
            ph++;
            break;
        case 2:
            c = d >> 6;
            d &= 0x003f;
            *dst++ = base64chars[c];
            c = d;
            d  = 0;
            ph = 0;
            break;
        default:
            break;
        }
        *dst++ = base64chars[c];
    }
    switch(ph) {
    case 0:
        break;
    case 1:
        c = d >> 4;
        *dst++ = base64chars[c];
        *dst++ = '=';
        *dst++ = '=';
        break;
        
    case 2:
        c = d >> 6;
        *dst++ = base64chars[c];
        *dst++ = '=';
        break;
    default:    
        break;
    }
    *dst++ = '\0';
}


