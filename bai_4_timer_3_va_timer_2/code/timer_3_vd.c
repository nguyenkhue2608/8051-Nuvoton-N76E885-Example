#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



#define     LED1        P04
// #define     LED2        P03
#define     ON_LED      0
#define     OFF_LED     1



void delay100ms(uint16_t duration) {
    T3CON |= SET_BIT2 | SET_BIT1 | SET_BIT0;    // prescaler = 1/128
    RH3 = HIBYTE(65536 - 17280);
    RL3 = LOBYTE(65536 - 17280);
    T3CON |= SET_BIT3;  // run
    for (; duration > 0; duration--) {
        while (!(T3CON & SET_BIT4));    // while (TF3 == 0);
        T3CON &= CLR_BIT4;  // xoá cờ tràn TF3
    }
    T3CON &= CLR_BIT3;  // stop
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
        delay100ms(5);
    }
}
