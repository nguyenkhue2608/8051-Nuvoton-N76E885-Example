#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



 #define     LED1        P04
// #define     LED2        P03
 #define     ON_LED      0
 #define     OFF_LED     1



void main(void) {

    // Insert code

    //
    // P0.4 (LED1) bi-directional mode
    //
    P0M1 &= CLR_BIT4;
    P0M2 &= CLR_BIT4;
    LED1 = OFF_LED;

    //
    // P2.4 (RXD) bi-directional mode
    //
    P2M1 &= CLR_BIT4;
    P2M2 &= CLR_BIT4;
    P24 = 1;

    //
    // P2.5 (TXD) bi-directional mode
    //
    P2M1 &= CLR_BIT5;
    P2M2 &= CLR_BIT5;
    P25 = 1;

    //
    // cấu hình serial port
    //
    // mode 1 - UART 8-bit
    SM0_1 = 0;
    SM1_1 = 1;
    //
    // cho phép nhận data
    //
    REN_1 = 1;
    //
    // khởi tạo cho các lần gửi tiếp theo
    TI_1 = 1;

    //
    // cấu hình tốc độ Baud = 19200
    //
    // sử dụng tốc bộ Baud bình thường (không double) => bit SMOD (PCON.7) = 0
    //
    T3CON &= CLR_BIT7;
    //
    // tính giá trị nạp lại
    // reload value = 65536 - 1/16 * Fsys/Pre-scale * 1/Baud
    RH3 = HIBYTE(65464);
    RL3 = LOBYTE(65464);
    //
    // run timer 3
    T3CON |= SET_BIT3;



    while (1) {

        // Insert code

        if (RI_1) {
            uint8_t key = SBUF_1;
            RI_1 = 0;
            switch (key) {
                case 'y':
                    LED1 = ON_LED;
                    break;
                case 'n':
                    LED1 = OFF_LED;
                default:;
            }
        }
    }
}
