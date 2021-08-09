#include<stdio.h>
#include<unistd.h>
#include<math.h>
#include<wiringPiI2C.h>

#define DEVID 0x13
#define X 0x42  
#define Y 0x44
#define Z 0x46

#define COMPASSPARAM (0.0013 / 8192)
#define COMPASSPARAM2 (0.0025 / 32768)

double compass[3];

double *getCompass() {
  int fd;
  short datax,datay,dataz;

  // I2Cデバイスの初期化
  fd = wiringPiI2CSetup(DEVID);
  
  if((wiringPiI2CWriteReg8(fd,0x4b,0x01))<0){
    printf("write error register 0x4b");
  }

  if((wiringPiI2CWriteReg8(fd,0x4c,0x00))<0){
    printf("write error register 0x4c");
  }
 
  // 16ビットデータの読み込み
  datax = wiringPiI2CReadReg16(fd,X);  
  datay = wiringPiI2CReadReg16(fd,Y);
  dataz = wiringPiI2CReadReg16(fd,Z);

  // 変換
  datax = datax >> 3;
  datay = datay >> 3;
  dataz = dataz >> 1;

  // 実数に変換
  compass[0] = datax * COMPASSPARAM;
  compass[1] = datay * COMPASSPARAM;
  compass[2] = dataz * COMPASSPARAM2; 

  close(fd);

  return compass;
}
