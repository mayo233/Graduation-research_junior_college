#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<wiringPiI2C.h>

#define DEVID 0x69
#define X 0x02
#define Y 0x04
#define Z 0x06

#define GYROPARAM (2000.0 / 65536)

double gyro[3];

double *getGyro() {
  int fd;
  short datax,datay,dataz;

  // I2Cデバイスの初期化
  fd = wiringPiI2CSetup(DEVID);  

  // 16ビットデータの読み込み
  datax = wiringPiI2CReadReg16(fd,X);  
  datay = wiringPiI2CReadReg16(fd,Y);
  dataz = wiringPiI2CReadReg16(fd,Z);

  // 実数に変換
  gyro[0] = datax * GYROPARAM;
  gyro[1] = datay * GYROPARAM;
  gyro[2] = dataz * GYROPARAM; 

  close(fd);

  return gyro;
}
