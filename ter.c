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

#define SERIAL_PIC "/dev/ttyUSB1"
#define SERIAL_SRAVE "/dev/ttyUSB0"

//構造体でウィジェットを指定する
typedef struct main_dialog_type {
  GtkWidget *window;
  GtkWidget *textview;
  GtkWidget *textview1;
  GtkWidget *label;
  GtkWidget *label2;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *button;
  
} MainDialog;


  //構造体変数名をdialogと宣言
  MainDialog dialog;
  int fd;  //フィールド
  gchar buf[255];         // バッファ
  int len;

  //出力する前の準備段階　つまりメモリ確保　　textは表示させたい内容
static void set_text(GtkTextView *textview,const gchar *text) //textviewは表示させる場所
{
  GtkTextBuffer *buffer;
  
  gtk_text_buffer_set_text(buffer,text,-1);
  buf=
  printf("%c",text);
  gtk_text_view_set_buffer(textview,buffer);
  
/*
  GtkTextIter start, end;
  gchar *utf8_text;

  //GtkTextViewからGtkTextBufferを取得する
  buffer = gtk_text_view_get_buffer(textview); 
  //開始行のGtkTextIterを取得（行番号は0から開始）
  gtk_text_buffer_get_start_iter(buffer, &start);
  //最終行のGtkTextIterを取得
  gtk_text_buffer_get_end_iter(buffer, &end);
  //startからendまでの行のテキストを取得（UTF8でエンコーディングされ、新しく領域を確保）
  utf8_text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE); 
  gtk_text_buffer_set_text(buffer, text, -1); 
  //標準出力にテキストを表示
  g_print( "%s\n", utf8_text);
  g_free(utf8_text);
*/
}

//シリアル通信させる関数
static int serial(GtkWidget *button,gpointer user_data){
//バッファからデータを取れるようにする　バッファに値を入れるようにする    

    // 送受信処理ループ
    //while(1) {
        len = read(fd, buf, sizeof(buf));  
        //printf("%lf",buf);
        if (0 < len) {
            //for(i = 0; i < len; i++) {//bufは文字列（受信データ）
                set_text(GTK_TEXT_VIEW(dialog.textview),buf);  //textviewは受信したデータを表示させる
            //}
            printf("\n");
            //__delay_ms(5000);
        }

        // エコーバック
        //write(fd, buf, len);  //これ書いたときに送られるのがUnknown command
        
    //}
   
    return 0;  
}
//シリアル通信を初期化する関数
int serial_init(void){
    //int fd;               // ファイルディスクリプタ
    struct termios tio;     // シリアル通信設定
    int baudRate = B115200;
    int i;
    fd = open(SERIAL_PIC, O_RDWR);     // デバイスをオープンする
 
    if (fd < 0) {
        printf("open error\n");
        return -1;
    }

    tio.c_cflag += CREAD;               // 受信有効
    tio.c_cflag += CLOCAL;              // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;                 // データビット:8bit
    tio.c_cflag += 0;                   // ストップビット:1bit
    tio.c_cflag += 0;                   // パリティ:None

    cfsetispeed( &tio, baudRate );
    cfsetospeed( &tio, baudRate );

    cfmakeraw(&tio);                    // RAWモード

    tcsetattr( fd, TCSANOW, &tio );     // デバイスに設定を行う

    ioctl(fd, TCSETS, &tio);            // ポートの設定を有効にする

    return 0;
}
 
//メイン関数
int main(int argc, char **argv)
{
  serial_init();  //最初にシリアル通信を初期化する
  gtk_init(&argc, &argv);
  
  //構造体変数名.メンバ名で記載
  dialog.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request(dialog.window,300,200);
  gtk_window_set_title(GTK_WINDOW(dialog.window), "モニタリング");
  gtk_window_set_position(GTK_WINDOW(dialog.window), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(dialog.window), 10);

  dialog.vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(dialog.window), dialog.vbox);
 
  //textviewとLabelを配置
  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);
  dialog.textview = gtk_text_view_new();  //テキストビュー作成
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.textview, TRUE, TRUE, 0);
  
  dialog.label = gtk_label_new("PIC");
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.label, TRUE, TRUE, 0);
  
  //スレーブのテキスト、スレーブのラベルを下の行に貼る
  dialog.hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);  //テキストを縦にする
  gtk_box_pack_start(GTK_BOX(dialog.vbox), dialog.hbox, TRUE, TRUE, 0);

  //テキストビューもう一つ作成 スレーブの
  dialog.textview1 = gtk_text_view_new();
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.textview1, TRUE, TRUE, 0);
  dialog.label2 = gtk_label_new("スレーブ");
  gtk_box_pack_start(GTK_BOX(dialog.hbox), dialog.label2, TRUE, TRUE, 0);

  //ボタン作成 入力されたのを加える
  dialog.button = gtk_button_new_with_label("押せ");

  //ボタンを配置
  gtk_box_pack_start(GTK_BOX(dialog.vbox),dialog.button, FALSE, FALSE, 0);
  //ボタンを押したらシリアル通信開始
  g_signal_connect(G_OBJECT(dialog.button), "clicked", G_CALLBACK(serial), NULL); 
   
  gtk_widget_show_all(dialog.window);  //すべての部品が配置されている状態で見る 
  
  gtk_main();  //mainメソッドを繰り返している
  
  close(fd);
 
  return 0;
}