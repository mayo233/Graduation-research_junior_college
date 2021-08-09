#include <xc.h>
#include "serial.h"
#define _XTAL_FREQ 32000000

const unsigned char rn4020ini1[] = "SF,1";
const unsigned char rn4020ini2[] = "SR,32000000";
const unsigned char rn4020ini3[] = "R,1";

void rn4020Setup() {
    SendConstStr(rn4020ini1); Send('\r');
    __delay_ms(100);
    SendConstStr(rn4020ini2); Send('\r');
    __delay_ms(100);
    SendConstStr(rn4020ini3); Send('\r');
    __delay_ms(100);
}
