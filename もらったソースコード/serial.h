#ifndef SERIAL_H //二重でincludeされることを防ぐ
#define SERIAL_H

#define SERIAL_PIC "/dev/ttyUSB1"     // PICのシリアルポート
#define SERIAL_SRAVE "/dev/ttyUSB0"   // スレーブのシリアルポート
#include <glib.h>
// ウィジット
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

MainDialog dialog;

int fd;             //フィールド
gchar buf[255];    // 受信データ格納
int len;           // 受信データの長さ

// プロトタイプ宣言
void set_text(GtkTextView *textview, const gchar *text);
int serial(GtkWidget *button, gpointer user_data);
int serial_init(void);
void widget_init(void);
int serial(GtkWidget *button,gpointer user_data);

#endif
