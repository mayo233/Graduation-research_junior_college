#include <xc.h>
// #include "BMX055.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#define isspace(x) (x==' ' || x== '\n' || x == '\r')
//#define isdigit(x) ('0' <= x && x <= '9')
//#define ishex(x) (isdigit(x) || ('A'<=x && x <= 'F') || ('a'<=x && x <= 'F'))
#define MODE_DIGIT (Byte)0
#define MODE_HEX   (Byte)1
typedef unsigned char Byte;

Byte str2Byte(Byte *b) { // b のバッファから1バイトのデータを読みとる
    Byte mode = MODE_DIGIT;
    Byte data  = 0;
    while(isspace(*b)) b++;
    if(*b == '0') {
        b++;
        if(*b == 'x' || *b == 'X') {
            mode = MODE_HEX;
            b++;
        } else b--;
    }
    if(mode == MODE_DIGIT) {
        while(isdigit(*b)) {
            data *=10;
            data += (*b - '0');
            b++;
        }
        return data;
    } else { // 多分１６進数
        while(isxdigit(*b)) {
            data << 4;
            if(isdigit(*b)) data += (*b - '0');
            else if('A' <= *b && *b <= 'F') data += (*b - 'A' + 10);
            else data += (*b - 'a' + 10);
            b++;
        }
        return data;
    }
    return 0;
}

Byte str2Hxs(Byte *s, Byte *r) { // s 内にある16進文字列を、16進数に変換
    Byte n = 0,d;
    while(isspace(*s)) s++;　// ブランクスキップ
    if(*s == '0' && (*(s+1) == 'x' || *(s+1) == 'X') ) { // 頭が0x なら読み飛ばす
        s += 2;
    }
    while(isxdigit(*s)) {
        n++;
        d = (isdigit(*s)?(*s - '0'):
              (('A'<= *s && *s <='F')?(*s - 'A' + 10):
                  (*s - 'a' + 10)))<<4;
        s++;
        if(isxdigit(*s)) {
            d += (isdigit(*s)?(*s - '0'):
                  (('A'<= *s && *s <='F')?(*s - 'A' + 10):
                      (*s - 'a' + 10)));
        }
		*r = d;
		s++;
        r++;
    }
    return n;
}

Byte *skip2blank(Byte *b) {
	if(!isspace(*b) && *b != '\0') b++;
	return b;
}

Byte i2cAddr, i2cReg, i2cNdata;
Byte i2cbuff[16];


