#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define AXELPARAM (2.0 / 4096)

void Number(char *nptrx1,char *nptrx2,char *nptry1,char *nptry2,char *nptrz1,char *nptrz2,int *hairetu);
int xyz[6];

unsigned char xtmp[6];
double accel[3];

unsigned char *sub(){
	char buff[1024];
	char data[17];
	char dataX1[3];
	char dataX2[3];
	char dataY1[3];
	char dataY2[3];
	char dataZ1[3];
	char dataZ2[3];
	int x1,x2,y1,y2,z1,z2;
	int sec =1;
	int i=0;
	int res;
	
	
	FILE *fp = popen("gatttool -b 00:1e:c0:55:40:b5 --char-read -a 0x0018","r");
	while(fgets(buff,sizeof(buff),fp) != NULL){ 
	}
	

	sscanf(buff+33,"%x %x %x %x %x %x",&x1,&x2,&y1,&y2,&z1,&z2);

	xtmp[0] = x1;
	xtmp[1] = x2;
	xtmp[2] = y1;
	xtmp[3] = y2;
	xtmp[4] = z1;
	xtmp[5] = z2;
	
	pclose(fp);

	return xtmp;
}

void Number(char *nptrx1,char *nptrx2,char *nptry1,char *nptry2,char *nptrz1,char *nptrz2,int *hairetu){
	 int x1,x2,y1,y2,z1,z2;

	sscanf(nptrx1,"%x",&x1);
	hairetu[0]=x1;
	
	sscanf(nptrx2,"%x",&x2);
	hairetu[1]=x2;
	
	sscanf(nptry1,"%x",&y1);
	hairetu[2]=y1;
	
	sscanf(nptry2,"%x",&y2);
	hairetu[3]=y2;
	
	sscanf(nptrz1,"%x",&z1);
	hairetu[4]=z1;
	
	sscanf(nptrz2,"%x",&z2);
	hairetu[5]=z2;
}


double *bluegetAccel(){
    unsigned char *xt;
    xt = sub();
    short data[5];
    int i;

    for(i=0;i<6;i++){
        data[i] = (unsigned char)xt[i];
    }
    
    accel[0] = ((data[1] * 256) + (data[0] & 0xF0)) / 16;
    if(accel[0] > 2047) accel[0] -= 4096;
    accel[1] = ((data[3] * 256) + (data[2] & 0xF0)) / 16;
    if(accel[1] > 2047) accel[1] -= 4096;
    accel[2] = ((data[5] * 256) + (data[4] & 0xF0)) / 16;
    if(accel[2] > 2047) accel[2] -= 4096;
 	
    accel[0] = accel[0] * AXELPARAM;
    accel[1] = accel[1] * AXELPARAM;
    accel[2] = accel[2] * AXELPARAM; 


    return accel;	
}


