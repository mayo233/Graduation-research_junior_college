#include <gtk/gtk.h>
#include <glib.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include "serial.h"

// シリアル通信の初期設定
int serial_init(void){
  struct termios tio;     // シリアル通信設定
  int baudRate = B115200;   // ボーレートは115200

  pic = open(SERIAL_PIC, O_RDWR);     // デバイスをオープンする
  slave = open(SERIAL_SLAVE, O_RDWR);
 
  // シリアルオープンできなかった
  if (pic < 0 && slave < 0) {
    printf("open error\n");
    return -1;
  }else{
    if(pic < 0){    // picのみオープンできなかった
      printf("open error,pic\n");
      set_text(GTK_TEXT_VIEW(dialog.textview), "open error,pic\n");
      return -1;
    }
    if(slave < 0){   // slaveのみオープンできなかった
      printf("open error,slave\n");
      set_text(GTK_TEXT_VIEW(dialog.textview1), "open error,slave\n");
      return -1;
    }
  }

  tio.c_cflag += CREAD;               // 受信有効
  tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
  tio.c_cflag += CS8;                 // データビット:8bit
  tio.c_cflag += 0;                   // ストップビット:1bit
  tio.c_cflag += 0;                   // パリティ:None

  cfsetispeed(&tio, baudRate);    // ボーレートの設定
  cfsetospeed(&tio, baudRate);

  cfmakeraw(&tio);                   // RAWモード
  tcsetattr(pic, TCSANOW, &tio);     // デバイスに設定を行う
  tcsetattr(slave, TCSANOW, &tio);   // デバイスに設定を行う

  ioctl(pic, TCSETS, &tio);          // ポートの設定を有効にする
  ioctl(slave, TCSETS, &tio);  

  return 0;
}


void widget_init(void){
  //ウィンドウの設定
  dialog.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(dialog.window,500,400);
  gtk_window_set_title(GTK_WINDOW(dialog.window), "シリアル通信モニタ");
  gtk_window_set_position(GTK_WINDOW(dialog.window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(dialog.window), 10);
  g_signal_connect(G_OBJECT(dialog.window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  //垂直レイアウト
  dialog.vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(dialog.window), dialog.vbox);

  // スクロールバーの設定
  dialog.scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(dialog.scrolledwindow), GTK_SHADOW_ETCHED_OUT);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(dialog.scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  //水平レイアウト
  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.scrolledwindow, TRUE, TRUE, 0);

  //PICのテキストビューとラベル
  dialog.textview = gtk_text_view_new();
  gtk_container_add(GTK_CONTAINER(dialog.scrolledwindow), dialog.textview);
  dialog.label = gtk_label_new("PIC");
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.label, TRUE, TRUE, 0);

  //水平レイアウト
  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  //テキストを縦にする
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);

  dialog.scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(dialog.scrolledwindow), GTK_SHADOW_ETCHED_OUT);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(dialog.scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.scrolledwindow, TRUE, TRUE, 0);

  //スレーブのテキストビューとラベル
  dialog.textview1 = gtk_text_view_new();
  gtk_container_add(GTK_CONTAINER(dialog.scrolledwindow), dialog.textview1);
  dialog.label2 = gtk_label_new("スレーブ");
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.label2, TRUE, TRUE, 0);

  //ボタン
  dialog.button_start = gtk_button_new_with_label("        開始        ");
  dialog.button_end = gtk_button_new_with_label("        終了        ");

  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  //テキストを縦にする
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);
  //ボタンを配置
  gtk_box_pack_start(GTK_BOX(dialog.hbox),dialog.button_start, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(dialog.hbox),dialog.button_end, FALSE, FALSE, 0);

  gtk_widget_show_all(dialog.window);  //ウィンドウ表示
}
