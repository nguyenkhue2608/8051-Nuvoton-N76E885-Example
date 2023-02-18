#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



#define     LED1        P04
// #define     LED2        P03
#define     ON_LED      0
#define     OFF_LED     1



void delay1ms(uint16_t duration) {
    CM_RL2 = 1;         // mode so sánh
    T2MOD &= CLR_BIT6 & CLR_BIT5 & CLR_BIT4;    // prescaler = 1/32
    T2MOD |= SET_BIT2;  // tự đông xoá khi "compare match"
    RCMP2H = HIBYTE(22118);
    RCMP2L = LOBYTE(22118);
    TR2 = 1;
    for (; duration > 0; duration--) {
        while (!TF2);   // chờ đến khi "compare match"
        TF2 = 0;
    }
    TR2 = 0;
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



    while (1) {
        LED1 = !LED1;
        delay1ms(500);
    }
}
