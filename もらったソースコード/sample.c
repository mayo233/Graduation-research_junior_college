//このファイルはter.cの初期化以外のメソッド　つまりボタン押したときとかのメソッド
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

void set_text(GtkTextView *textview, const gchar *text){ //textviewは表示させる場所
  GtkTextBuffer *buffer;
  
  buffer = gtk_text_view_get_buffer(textview);
  gtk_text_buffer_set_text(buffer, text, -1);

/*
  gtk_text_buffer_set_text(buffer, text, -1);
  //printf("%c",text);
  gtk_text_view_set_buffer(textview, buffer);
*/
}

int serial(GtkWidget *button,gpointer user_data){
  set_text(GTK_TEXT_VIEW(dialog.textview),"押されたよ\n");
  // 送受信処理ループ
  len = read(fd, buf, sizeof(buf));  
  if (0 < len) {
    set_text(GTK_TEXT_VIEW(dialog.textview),buf);  //textviewは受信したデータを表示させる
    //printf("\n");
  }

  // エコーバック
  //write(fd, buf, len);  //これ書いたときに送られるのがUnknown command    
   
    return 0;  
}

int main(int argc, char** argv){
  
  gtk_init(&argc, &argv);
  widget_init();
  serial_init();
  g_signal_connect(G_OBJECT(dialog.button), "clicked", G_CALLBACK(serial), NULL); 

  //set_text(GTK_TEXT_VIEW(dialog.textview), "open error\n");
  gtk_main();  //mainメソッドを繰り返している
  return 0;
}
