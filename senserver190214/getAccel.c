#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<wiringPiI2C.h>

#define DEVID 0x19
#define X 0x02
#define Y 0x04
#define Z 0x06

#define AXELPARAM (2.0 / 4096)

double accel[3];

double *getAccel() {
  int fd;
  short datax,datay,dataz;

  // I2Cデバイスの初期化
  fd = wiringPiI2CSetup(DEVID);  

  // 16ビットデータの読み込み(加速度)
  datax = wiringPiI2CReadReg16(fd,X);  
  datay = wiringPiI2CReadReg16(fd,Y);
  dataz = wiringPiI2CReadReg16(fd,Z);

  // 12ビットデータに変換(加速度)
  datax = datax >> 4;
  datay = datay >> 4;
  dataz = dataz >> 4;

  // 実数に変換(加速度)
  accel[0] = datax * AXELPARAM;
  accel[1] = datay * AXELPARAM;
  accel[2] = dataz * AXELPARAM; 

  close(fd);

  return accel;
}
