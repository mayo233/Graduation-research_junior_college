//このファイルはシリアル通信の初期化かつウィジェットのレイアウト設定
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


int serial_init(void){
  struct termios tio;     // シリアル通信設定
  int baudRate = B115200;
  int i;

  fd = open(SERIAL_PIC, O_RDWR);     // デバイスをオープンする
 
  // シリアルオープンできなかった
  if (fd < 0) {
    printf("open error\n");
    set_text(GTK_TEXT_VIEW(dialog.textview), "open error\n");
    return -1;
  }

  tio.c_cflag += CREAD;               // 受信有効
  tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
  tio.c_cflag += CS8;                 // データビット:8bit
  tio.c_cflag += 0;                   // ストップビット:1bit
  tio.c_cflag += 0;                   // パリティ:None

  cfsetispeed(&tio, baudRate);
  cfsetospeed(&tio, baudRate);
  cfmakeraw(&tio);                    // RAWモード
  tcsetattr(fd, TCSANOW, &tio);     // デバイスに設定を行う
  ioctl(fd, TCSETS, &tio);            // ポートの設定を有効にする

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

  //水平レイアウト
  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);

  //PICのテキストビューとラベル
  dialog.textview = gtk_text_view_new();
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.textview, TRUE, TRUE, 0); 
  dialog.label = gtk_label_new("PIC");
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.label, TRUE, TRUE, 0);

  //水平レイアウト
  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  //テキストを縦にする
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);

  //スレーブのテキストビューとラベル
  dialog.textview1 = gtk_text_view_new();
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.textview1, TRUE, TRUE, 0);
  dialog.label2 = gtk_label_new("スレーブ");
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.label2, TRUE, TRUE, 0);

  //ボタン作成 入力されたのを加える
  dialog.button = gtk_button_new_with_label("押せ");

  //ボタンを配置
  gtk_box_pack_start(GTK_BOX(dialog.vbox),dialog.button, FALSE, FALSE, 0);
  //ボタンを押したらシリアル通信開始

  gtk_widget_show_all(dialog.window);  //すべての部品が配置されている状態で見る
}
