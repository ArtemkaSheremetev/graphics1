#pragma once

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    void (*buttonHandler)();
}button_pos_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t z;
    bool motion;
}tPoint;

typedef enum {
    MENU = 0,
    MODE,
} app_states_t;

void buttonStructInit();
void MenuButtonInit();
void buttonMode7Handler();
void buttonMode16Handler();
void MenuHandler();
void printTouchToSerial(TS_Point p);
void printTouchToDisplay(TS_Point p);
void RtosToolsInit(void);
void IRAM_ATTR touchISR();
void spritesInit();
void ButtonInit();
void buttonClearHandler();
void buttonReadyHandler();
void buttonExitHandler();
void checkButtons(tPoint*, bool);
