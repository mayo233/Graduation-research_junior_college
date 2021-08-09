#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "serial.h"
#include "I2CMaster.h"
#define MODE_DIGIT (Byte)0
#define MODE_HEX   (Byte)1
#define NCOMMANDS 4

// typedef unsigned char Byte;

Byte *lineBuff;
Byte convMode,convData,convErr;
unsigned char myUpper(Byte c);
Byte stobyte(Byte *s);
int stoint(Byte *s);

extern const unsigned char *cmdArray[];
//extern unsigned char *Buffer;

const unsigned char cmdEcho[] = "ECHO";
const unsigned char cmdGet[]  = "GET";
const unsigned char cmdSet[]  = "SET";
const unsigned char cmdX[]  = "X";
/*
 * コマンド解析および実行関数 
 */
void prefEcho(Byte *a[]);
void prefGet(Byte *a[]);
void prefSet(Byte *a[]);
void prefX(Byte *a[]);

const unsigned char *cmdArray[] = {cmdEcho,cmdGet,cmdSet,cmdX};
void (*prefArray[])(Byte *args[]) = {prefEcho,prefGet,prefSet,prefX};

const unsigned char errMess[] = "Unknown command: ";
const unsigned char ivaMess[] = "Invalid ARGUMENT: ";

void setConvMode(Byte m) {
	convData = 0;
	convMode = m;
	convErr  = 0;
}

void newConvLine(Byte *p) {
	lineBuff = p;
	setConvMode(MODE_DIGIT);
}

//空白を読み飛ばす。
void skipblankLine() {
    while(isspace(*lineBuff)) lineBuff++;
}

// 読み取るべきデータがあるかどうか、ただしエラーが起きている場合は常に真
Byte endofLine(Byte flag) {  // flag が真の時、Black を読み飛ばしてから判断
	if(convErr) return convErr;
	if(flag) skipblankLine();
	return *lineBuff == '\0';
}

Byte *nextItem() {
	Byte *p;
	if(convErr) return NULL;
	if(endofLine((Byte)1) ) return NULL;
	p = lineBuff;
	while(*lineBuff && !isspace(*lineBuff)) lineBuff++;
	if(isspace(*lineBuff)) {
		*lineBuff = '\0';
		lineBuff++;
	}
	return p;
}


// 小文字を大文字にする。
unsigned char myUpper(Byte c) {
    if('a' <= c && c <= 'z') return c - 'a' + 'A';
    return c;
} 


Byte cmdCheck(unsigned char *b, const unsigned char *c) {
    Byte n = 0;
    while(*b && (*b == *c || myUpper(*b) == *c)) {
        b++; c++; n++;
    }
    if(*b == '\0') return n;
	return 0;
}


// Buffer 上のコマンドラインの解析と実行
#define MAXARGS 10
void parseCommand() {
    Byte i,*p,*q;
	Byte *args[MAXARGS];
	newConvLine(Buffer);
	p = nextItem();
	if(p == NULL) return; // 有効なデータなし
//	sndStr(p);
//    SendCRLF();
	for(i=0;i<MAXARGS;i++) {
		args[i] = NULL;
	}
	i = 0;
	while(i < MAXARGS && (q = nextItem())) { // 引数をセットする
		args[i] = q;
		i++;
	} 
    for(i=0; i < NCOMMANDS; i++) {
		if(cmdCheck(p,cmdArray[i])>0) {
			(*prefArray[i])(args);
			return;
		}
	}
	SendConstStr(errMess); 
	sndStr(p);
	//strcpy(&SndBuf[0],p); sendStr();
	SendCRLF();
}
// 文字列変換
// 1バイト数
Byte stobyte(Byte *s) {
	//Byte base = 10;
	Byte ans = 0;
    if(*s=='0') { //8進数か16進数
		s++;
		//base = 8;
		if(*s == 'x' || *s == 'X') {
			s++;
			while(*s != '\0') { 
				ans *= 16;
				if(isdigit(*s)) // 数字
					ans += *s -'0';
				else if(isupper(*s)) // 大文字
					ans += *s - 'A' + 10;
				else // 小文字
					ans += *s - 'a' + 10;
				s++;
			}
			return ans;
		}
	}
	while(*s != '\0') {
		ans *= 10;
		ans += *s - '0';
		s++;
	}
	return ans;
}

int stoint(Byte *s) {
	//int base = 10;
	int ans = 0;
    if(*s=='0') { //8進数か16進数
		s++;
		// base = 8;
		if(*s == 'x' || *s == 'X') {
			s++;
			while(*s != '\0') { 
				ans *= 16;
				if(isdigit(*s)) // 数字
					ans += *s -'0';
				else if(isupper(*s)) // 大文字
					ans += *s - 'A' + 10;
				else // 小文字
					ans += *s - 'a' + 10;
				s++;
			}
			return ans;
		}
	}
	while(*s != '\0') {
		ans *= 10;
		ans += *s - '0';
		s++;
	}
	return ans;
}

// コマンド実行
void printInvArgErr(Byte *args[]);

// echo モード
void prefEcho(Byte *args[]) {
	Byte f[4];
	f[0] = 'O'; f[1] = 'f'; f[2] = 'f';f[3] = '\0';
	if(args[0] == NULL) {
		SendConstStr(cmdEcho); Send(' ');
		if(echoFLG) {
			f[1] = 'n';
			f[2] = '\0';
		}
		sndStr(f); SendCRLF();
		return;
	} else if(args[1] == NULL) {
		switch (*args[0]) {
		case '0': case 'f': case 'F':
			echoFLG = 0;
			return;
		case '1': case 't': case 'T':
			echoFLG = 1;
			return;
		case 'o': case 'O':
			if(*(args[0]+1) == 'n' || *(args[0]+1) == 'N') echoFLG = 1;
			else echoFLG = 0;
			return;
		}
	}
	printInvArgErr(args);
}

// i2cset に相当
// パラメータ: ADDR REG [DATA [...]] [MODE]] 

void prefSet(Byte *args[]) {
	Byte addr,reg,data,*p,i;
	Byte buf[MAXARGS];

	if(args[0] == NULL) goto err;
	addr = stobyte(args[0]);
	if(addr == 0) goto err;

	for(i = 1; i < MAXARGS && args[i] != NULL; i++) {
		if(isdigit(*args[i])) buf[i-1] = stobyte(args[i]);
		else break;
	}
	if(i < MAXARGS && args[i] != NULL) { // MODEがあったね
		switch(*args[i]) {
		case 'b': case 'B':
		case 'c': case 'C':
			sendI2Cbyte(addr,buf[0],buf[1]);
			return;
		case 'i': case 'I':
			sendI2C(addr,buf,i-1);
			return;
		}
	} else { // MODE なし、与えられたデータを送るのみ
		sendI2C(addr,buf,i-1);
		return;
	}

err:
	printInvArgErr(args);
}

// i2cget に相当
// パラメータ: ADDR [REG [MODE]] 

void prefGet(Byte *args[]) {
	Byte addr,reg,*p,i;
	Byte buf[16];

	if(args[0] == NULL) goto err;
	addr = stobyte(args[0]);
	if(addr == 0) goto err;

	if(args[1] != NULL) {
		reg = stobyte(args[1]);
		buf[0] = reg;
//		Send('G'); Send(':');		SendHex(addr); SendHex(reg);
//		SendCRLF();
		if(args[2] != NULL) {
			switch(*args[2]) {
			case 'b': case 'B':
			case 'c': case 'C':
				sendI2C(addr,&reg,1);
				readI2C(addr,buf,1);
				SendHex(buf[0]);
				SendCRLF();
				return;
			case 'w': case 'W':
				sendI2C(addr,&reg,1);
				readI2C(addr,buf,2);
				SendHex(buf[0]);
				SendHex(buf[1]);
				SendCRLF();
				return;
//			default:
			}
		} else { // とりあえず、レジスタは指定してね
		}
	}
err:
	printInvArgErr(args);
}

void prefX(Byte *args[]) {
	Byte j,i,addr,reg;
	Byte buf[MAXARGS];
	Byte x[] = "X:";
	for(i = 0; i < MAXARGS && args[i] != NULL; i++) {
		if(isdigit(*args[i])) buf[i] = stobyte(args[i]);
		else break;
	}
	if(i<3) return;
	addr = buf[0];
	reg  = buf[1];
	j    = buf[2];
	sendI2C(addr,&reg,1);
	readI2C(addr,buf,j);

	sndStr(x);
	for(i = 0; i < j; i++) {
		Send(' ');
		SendHex(buf[i]);
	}
	SendCRLF();
}

void printInvArgErr(Byte *args[]) {
	Byte i;
	SendConstStr(ivaMess);
	
	for(i=0; args[i] != NULL; i++) {
		Send(' ');
		sndStr(args[i]);
	}
	//strcpy(&SndBuf[0],p); sendStr();
	SendCRLF();
}

