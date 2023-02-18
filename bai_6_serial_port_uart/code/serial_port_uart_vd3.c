#include <stdint.h>
#include <mcs51/N76E885.h>
#include <mcs51/Define.h>



// #define     LED1        P04
// #define     LED2        P03
// #define     ON_LED      0
// #define     OFF_LED     1



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



void number2TextHex(uint8_t *text_hex, uint8_t number) {
    uint8_t digit;

    digit = number >> 4;
    if (digit > 9) {
        text_hex[0] = 'A' + (digit - 10);
    } else {
        text_hex[0] = '0' + digit;
    }

    digit = number & 0x0f;
    if (digit > 9) {
        text_hex[1] = 'A' + (digit - 10);
    } else {
        text_hex[1] = '0' + digit;
    }
}



void main(void) {

    // Insert code

    //
    // cấu hình serial port
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

            uint8_t str[10];

            number2TextHex(str + 0, decode.code.address);
            number2TextHex(str + 2, decode.code.inverse_of_address);
            number2TextHex(str + 4, decode.code.command);
            number2TextHex(str + 6, decode.code.inverse_of_command);
            str[8] = '\n';  // xuống dòng
            str[9] = 0;     // null

            for (uint8_t i = 0; str[i] != 0; ++i) {
                while (!TI_1);
                TI_1 = 0;
                SBUF_1 = str[i];
            }
        }
    }
}
