#include "serial.h"   //serial.hの中に含まれているファイル

// textviewにデータを表示  
void set_text(GtkTextView *textview,const gchar *text){  
  GtkTextBuffer *buffer;
  GtkTextIter start, end;
  gchar *keep,*textnew,*textnew1,*temp[255];

  textnew1=strcpy(temp,text);  //新しく入ってくるデータを消さないように一時的に保管
  //textviewで表示される文字列をbufferに入れる
  buffer = gtk_text_view_get_buffer(textview);

  //開始行のGtkTextIterを取得（行番号は0から開始）
  gtk_text_buffer_get_start_iter(buffer, &start);

  //最終行のGtkTextIterを取得
  gtk_text_buffer_get_end_iter(buffer, &end);
  //startからendまでの行のテキストを取得
  keep=gtk_text_buffer_get_text(buffer,&start,&end,TRUE);

  //新しいデータと古いデータを連結させる
  textnew=strcat(keep,textnew1);  //char型,const char型
  //bufferを削除し、textを入れている　つまり文字列をセットしている
  gtk_text_buffer_set_text(buffer,textnew, -1);  //新しい受信データ
}
//シリアル通信をスタート
void serial_start(void){
  stop = TRUE;  //stopをさせている ここはboolean 
  //100ミリ秒ごとにserial()関数をdialog.windowに表示
  g_timeout_add(100, (GSourceFunc)serial, dialog.window);
}
//シリアル通信を終わらせる
void serial_end(void){
  stop = FALSE;
}

int main(int argc, char** argv){
  port = 0;  //送受信を分ける
  stop = TRUE;  //シリアル通信スタート
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