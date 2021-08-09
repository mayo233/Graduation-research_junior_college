/* 
 * File:   I2CMaster.h
 * Author: oda
 *
 * Created on 2019/02/26, 15:20
 */

#ifndef I2CMASTER_H
#define	I2CMASTER_H

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

void initI2C(unsigned char flag);
void sendI2C(unsigned char addr, unsigned char *buf, unsigned char n);
void sendI2Cbyte(unsigned char addr,unsigned char reg, unsigned char data);
int readI2C(unsigned char addr, unsigned char *buf, unsigned char n);
void waitIdleI2C(void);

typedef unsigned char Byte;


#define ADT7410ADDR 0x48
#define ADT7410ADDR0 0x48
#define ADT7410ADDR1 0x49
#define ADT7410ADDR2 0x4a
#define ADT7410ADDR3 0x4b


#endif	/* I2CMASTER_H */

