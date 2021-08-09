#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include "serial.h"

// シリアル通信
int serial(GtkWidget *widget){
  int rec, trans;    // 受信側、送信側

  // ウィンドウが閉じられたとき
  if (gtk_widget_get_window(widget) == NULL)
  {
    return FALSE;
  }

  // 受信、送信の設定
  if(port == 0){  //受信、送信が何もなかったら
    rec = pic;  //picのデータをrecに入れる
    trans = slave;  //slavetransに入れる
    printf("受信：pic, 送信：slave\n"); 
  }else{  //portが０じゃないとき
    rec = slave;
    trans = pic;
    printf("受信：slave, 送信：pic\n");
  }

  // 受信
  len = read(rec, buf, sizeof(buf));  // 受信データを配列に格納

  if (0 < len) { //受信データがあったら
    if(port == 0) set_text(GTK_TEXT_VIEW(dialog.textview),buf);  // textviewに受信データを表示
    else set_text(GTK_TEXT_VIEW(dialog.textview1),buf);  
  }

  // 送信
  if(port == 0){
    write(trans, buf, len);    // データ送信
    port = 1;    // 送受信入れ替え
  }else{
    write(trans, buf, len);   
    port = 0;
  }
   
  return stop;  
}
