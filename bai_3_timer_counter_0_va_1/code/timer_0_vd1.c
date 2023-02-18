#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



#define     LED1        P04
// #define     LED2        P03
#define     ON_LED      0
#define     OFF_LED     1



void delay1sec(uint16_t duration) {
    TMOD &= CLR_BIT3 & CLR_BIT2 & CLR_BIT1 & CLR_BIT0;  // xoá 4 bit thấp của TMOD
    TMOD |= SET_BIT0;   // timer 0: GATE = 0, C/T = 0, mode 1
    AUXR1 |= SET_BIT4;  // clock thạch anh 32.768 kHz
    for (; duration > 0; duration--) {
        TH0 = HIBYTE(32768);
        TL0 = LOBYTE(32768);
        TR0 = 1;        // run
        while (!TF0);   // chờ đến khi tràn
        TR0 = 0;        // stop
        TF0 = 0;        // xoá cờ báo tràn
    }
}



void main(void) {

    // Insert code

    //
    // P0.4 (LED1) quasi-bidiretional mode
    //
    P0M1 &= CLR_BIT4;
    P0M2 &= CLR_BIT4;

    //
    // khởi tạo giá trị mặc định
    //
    LED1 = OFF_LED;

    //
    // enable thạch anh 32.768 kHz
    //
    // __bit bit_tmp = EA;
    // EA = 0;
    uint8_t tmp = CKEN;
    tmp &= CLR_BIT7;
    tmp |= SET_BIT6;
    TA = 0xaa;
    TA = 0x55;
    CKEN = tmp;
    // EA = bit_tmp;



    while (1) {
        LED1 = !LED1;
        delay1sec(2);
    }
}
