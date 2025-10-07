#ifndef _RENDER_H_
#define _RENDER_H_

#include "geomentry.h"

#define WIDTH 320
#define HEIGHT 240
#define BUFFER_BASE 0x40000000
#define BUFFER8 (uint8_t*)BUFFER_BASE
#define BUFFER32 (uint32_t*)BUFFER_BASE
#define SETBIT(X) (1 << X)

void render2Drect(figure2D_t self, camera_t);
void render2Dtriang(figure2D_t self, camera_t);
void render2Dcircle(figure2D_t self, camera_t);
void render3Drect(figure3D_t self, camera_t);
void render3Dtriang(figure3D_t self, camera_t);
void render3Dcircle(figure3D_t self, camera_t);

void SpritedrawLine(uint8_t* buf, uint8_t width, uint8_t height ,int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void SpritesetPixel(uint8_t* buf, int16_t x, int16_t y);
//void Spriterender2D(figure2D_t self, uint8_t* buff);
//void Spriterender3D(figure3D_t self, uint8_t* buff);

#endif
