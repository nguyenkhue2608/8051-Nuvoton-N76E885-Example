#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



// #define     LED1        P04
// #define     LED2        P03
// #define     ON_LED      0
// #define     OFF_LED     1



void main(void) {

    // Insert code

    //
    // P2.0 (T0) quasi-bidirectional mode
    //
    P2M1 &= CLR_BIT0;
    P2M2 &= CLR_BIT0;

    //
    // nhường mức logic cho timer 0
    //
    P20 = 1;

    //
    // cho phép toggle pin P2.0 (T0) khi tràn timer 0
    //
    P1M1 |= SET_BIT2;

    //
    // cấu hình timer 0
    //
    TMOD &= CLR_BIT3 & CLR_BIT2 & CLR_BIT1 & CLR_BIT0;  // xoá 4 bit thấp của TMOD
    TMOD |= SET_BIT1;   // timer 0: GATE = 0, C/T = 0, mode 2
    CKCON |= SET_BIT3;  // clock Fsys, prescaler = 1/1
    TH0 = 145;          // giá trị nạp lại
    TR0 = 1;            // run



    while (1) {
        // do nothing
    }
}
