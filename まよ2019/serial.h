#ifndef SERIAL_H //二重でincludeされることを防ぐ
#define SERIAL_H

#include <glib.h>
#define SERIAL_PIC "/dev/ttyUSB1"     // PICのシリアルポート
#define SERIAL_SLAVE "/dev/ttyUSB0"   // スレーブのシリアルポート

// ウィジット
typedef struct main_dialog_type {
  GtkWidget *window;
  GtkWidget *textview;
  GtkWidget *textview1;
  GtkWidget *label;
  GtkWidget *label2;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *button_start;
  GtkWidget *button_end;
  GtkWidget *scrolledwindow;
} MainDialog;

MainDialog dialog;

int pic, slave;             //フィールド
gchar buf[255];    // 受信データ格納
int len;           // 受信データの長さ
int port;      // 受信側or送信側
gboolean stop;   //gtkでのboolean stop

// プロトタイプ宣言
void set_text(GtkTextView *textview, const gchar *text);  //main.c
int serial(GtkWidget *widget);  //serial.c
int serial_init(void);  //init.c
void widget_init(void);  //init.c

#endif
