#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



#define     LED1        P04
// #define     LED2        P03
#define     ON_LED      0
#define     OFF_LED     1



#define     STATE_INIT              0
#define     STATE_BEG_START_BIT     1
#define     STATE_MID_START_BIT     2
#define     STATE_BEG_LOGIC_BIT     3
#define     STATE_MID_LOGIC_BIT     4
#define     STATE_BEG_STOP_BIT      5

typedef struct {
    uint8_t logic_bit : 7;
    uint8_t success : 1;
    uint8_t state;
    union {
        struct {
            uint8_t inverse_of_command;
            uint8_t command;
            uint8_t inverse_of_address;
            uint8_t address;
        };
        uint32_t full;
    } code;
} nec_t;

// biến lưu kết quả giải mã
nec_t decode;



void necReset() {
    //
    // dừng timer
    TR2 = 0;
    //
    // capture kênh 2 tại cạnh xuống
    CAPCON1 &= CLR_BIT5 & CLR_BIT4;
    //
    decode.success = 0;
    decode.state = STATE_INIT;
}

void necHandler() {
    uint16_t capture = MAKEWORD(C2H, C2L);

    switch (decode.state) {
        case STATE_INIT:
            //
            // run timer
            TR2 = 1;
            //
            // capture kênh 2 tại bất kỳ cạnh xung
            CAPCON1 |= SET_BIT5;
            CAPCON1 &= CLR_BIT4;
            //
            decode.state = STATE_BEG_START_BIT;
        break;

        case STATE_BEG_START_BIT:
            if ((capture >= 47000) && (capture <= 60825)) {
                decode.state = STATE_MID_START_BIT;
            } else {
                decode.state = STATE_INIT;
            }
        break;

        case STATE_MID_START_BIT:
            if ((capture >= 22118) && (capture <= 27648)) {
                decode.logic_bit = 0;
                decode.success = 0;
                decode.state = STATE_BEG_LOGIC_BIT;
            } else {
                decode.state = STATE_INIT;
            }
        break;

        case STATE_BEG_LOGIC_BIT:
            if ((capture >= 2212) && (capture <= 4424)) {
                decode.state = STATE_MID_LOGIC_BIT;
            } else {
                decode.state = STATE_INIT;
            }
        break;

        case STATE_MID_LOGIC_BIT:
            if ((capture >= 2212) && (capture <= 4424)) {
                ++decode.logic_bit;
                decode.code.full <<= 1;
            } else if ((capture >= 8294) && (capture <= 9953)) {
                ++decode.logic_bit;
                decode.code.full <<= 1;
                decode.code.inverse_of_command |= SET_BIT0;
            } else {
                decode.state = STATE_INIT;
                break;
            }
            //----------------
            if (decode.logic_bit == 32) {
                decode.state = STATE_BEG_STOP_BIT;
            } else {
                decode.state = STATE_BEG_LOGIC_BIT;
            }
        break;

        case STATE_BEG_STOP_BIT:
            if ((capture >= 2212) && (capture <= 4424)) {
                decode.success = 1;
            }
            decode.state = STATE_INIT;
        break;
    }

    if (decode.state == STATE_INIT) {
        //
        // dừng timer
        TR2 = 0;
        //
        // capture kênh 2 tại cạnh xuống
        CAPCON1 &= CLR_BIT5 & CLR_BIT4;
    }
}



void main(void) {

    // Insert code

    //
    // cấu hình ouput cho pin LED1 (P0.4)
    P0M1 &= CLR_BIT4;
    P0M2 &= CLR_BIT4;
    LED1 = OFF_LED;

    //
    // khởi tạo input capture
    //
    // cấu hình input cho pin của input capture kênh 2 (P2.2)
    P2M1 &= CLR_BIT2;
    P2M2 &= CLR_BIT2;
    P22 = 1;
    //
    // cho phép kênh 2
    CAPCON0 |= SET_BIT6;
    //
    // cho phép bộ lọc nhiễu kênh 2
    CAPCON2 |= SET_BIT6;
    //
    // capture kênh 2 tại cạnh xuống
    CAPCON1 &= CLR_BIT5 & CLR_BIT4;

    //
    // khởi tạo timer 2
    //
    // prescaler = 1/4
    T2MOD &= CLR_BIT6 & CLR_BIT5;
    T2MOD |= SET_BIT4;
    //
    // mode không nạp lại
    CM_RL2 = 0;
    T2MOD &= CLR_BIT7;
    //
    // tự xoá timer khi capture
    T2MOD |= SET_BIT3;

    //
    // khởi tạo tiến trình giải mã mới
    //
    necReset();



    while (1) {

        //
        // sự kiện capture
        //
        if (CAPCON0 & SET_BIT2) {
            //
            // xoá cờ capture kênh 2
            CAPCON0 &= CLR_BIT2;
            //
            necHandler();
        }

        //
        // sự kiện timer 2 tràn
        //
        if (TF2) {
            //
            // xoá cờ timer 2
            TF2 = 0;
            //
            necReset();
        }

        //
        // chương trình chính
        //
        if (decode.success) {
            decode.success = 0;

            if (decode.code.address == 0x00) {
                switch (decode.code.command) {
                    case 0x98:
                        LED1 = ON_LED;
                    break;

                    case 0xD8:
                        LED1 = OFF_LED;
                    break;
                }
            }
        }
    }
}
