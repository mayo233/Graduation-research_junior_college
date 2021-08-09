#ifndef __SERIAL_H__
#define __SERIAL_H__
#define MaxSize 32

extern unsigned char Index,Tindex,Sending;// 受信バッファ位置、送信バッファ位置、送信文字位置
extern unsigned char RcvFlag,TxFlag;
extern unsigned char RcvBuf[MaxSize]; // 受信バッファ
extern unsigned char SndBuf[MaxSize]; // 送信バッファ
extern unsigned char SBuff[MaxSize] ; // 送信キュー
extern unsigned char Buffer[MaxSize]; // 汎用バッファ
// unsigned char i2cBuf[MaxSize];
extern unsigned char echoFLG;
// プロトタイプ
void copyBuffer(void); // 受信バッファのデータを汎用バッファにコピーする。受信バッファは空になる。

void Send(unsigned char),doSend(void),SendStr(void /*unsigned char */);
void sndStr(unsigned char *p);
void copyConstStr(const unsigned char*,unsigned char*);
void SendHex(unsigned char);
char QueIn(unsigned char);
unsigned char QueOut(void);
void trySend();
void encodeBase64(unsigned char *src,unsigned char n,unsigned char *dst );
unsigned char cmdCheck(unsigned char *b, const unsigned char *c);
unsigned char *skip2blank(unsigned char *b);
void perfGetCmd(unsigned char *x);

void perfSetCmd(unsigned char *x);

#define SendConstStr(str) (copyConstStr(str,SndBuf),SendStr())
#define SendCRLF()        (Send('\r'),Send('\n'))
// #define sndStr(p) (strcpy(SndBuf,p),SendStr())
#endif
