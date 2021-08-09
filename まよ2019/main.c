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
#include <glib.h>
#include "serial.h"

// textviewにデータを表示  
void set_text(GtkTextView *textview, const gchar *text){  
  GtkTextBuffer *buffer;
  
  //textviewで表示されている文字列をbufferに入れる
  buffer = gtk_text_view_get_buffer(textview);
  
  //bufferを削除し、textを入れている　つまりセットしている
  gtk_text_buffer_set_text(buffer, text, -1);  
}

//シリアル通信をスタート
void serial_start(void){
  stop = TRUE;  //stopをさせている
  
  //100ミリ秒ごとにserial()関数をdialog.windowに表示
  g_timeout_add(100, (GSourceFunc)serial, dialog.window);
}
//シリアル通信を終わらせる
void serial_end(void){
  stop = FALSE;
}

int main(int argc, char** argv){
  port = 0;
  stop = TRUE;
  gtk_init(&argc, &argv);
  widget_init();   // ウィンドウの設定
  serial_init();   // シリアル通信の設定
  //ボタンを押したらシリアル通信開始
  g_signal_connect(G_OBJECT(dialog.button_start), "clicked", G_CALLBACK(serial_start), NULL); 
  g_signal_connect(G_OBJECT(dialog.button_end), "clicked", G_CALLBACK(serial_end), NULL);

  gtk_main();  //main関数のループ
  close(pic);    // ポートを閉じる
  close(slave);
  return 0;
}
