
#include "stdint.h"
#include "trigtable.h"

#define TRIG_TABLE_SIZE 91  // 0..90 градусов, шаг 2°  
#define SCALE_SHIFT 8      

static const uint8_t sin_tableQ8[91] = {
    0,    4,    8,    13,   17,   22,   26,   31,   35,   40,
    44,   48,   53,   57,   61,   66,   70,   74,   79,   83,
    87,   91,   95,   100,  104,  108,  112,  116,  120,  124,
    128,  131,  135,  139,  143,  146,  150,  154,  157,  161,
    164,  167,  171,  174,  177,  180,  183,  186,  189,  192,
    195,  198,  201,  204,  206,  209,  211,  214,  216,  218,
    221,  223,  225,  227,  229,  231,  233,  234,  236,  238,
    239,  241,  242,  243,  245,  246,  247,  248,  249,  250,
    250,  251,  252,  252,  253,  253,  254,  254,  254,  254,
    255
};

int16_t sin_mult(int16_t angle, int16_t numb){
    int32_t temp = ((int32_t)numb * sin_deg(angle)) >> SCALE_SHIFT;
    return (int16_t)temp;
}

int16_t cos_mult(int16_t angle, int16_t numb){
    int32_t temp = ((int32_t)numb * sin_deg(90 - angle)) >> SCALE_SHIFT;
    return (int16_t)temp;
}

int sin_deg(int16_t degrees) {
    if (degrees >= 360){
        degrees -= 360;
    }else if(degrees < 0){
        degrees += 360;
    }

    int sign = 1;
    int angle = degrees;

    if (degrees <= 90) {             //1
        angle = degrees;
    } else if (degrees <= 180) {     //2
        angle = 180 - degrees;
    } else if (degrees <= 270) {     //3
        angle = degrees - 180;
        sign  = -1;
    } else {                         //4
        angle = 360 - degrees;
        sign  = -1;
    }

    return (int)(sign * sin_tableQ8[angle]);
}

