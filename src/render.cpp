#include  "geomentry.h"
#include "render.h"
#include "TFT_eSPI.h"
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include "trigtable.h"

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

extern TFT_eSPI tft;

static point2d_t verts2D[16] = {0};  //макс для 2д вершин
static point3d_t verts3D[32] = {0}; //макс для 3д вершин возможно круг нах убрать
static point2d_t width_vec = {0};
static point2d_t height_vec = {0};
//для вектора 2Drect 
static int16_t hw = 0; 
static int16_t hh = 0; 
//для двух векторов 3Drect 
static int16_t hx = 0;
static int16_t hy = 0;
static int16_t hz = 0;
static int16_t r = 0;

//для линий, для оптимизации не стал прятать в структуру
static int16_t px0 = 0;
static int16_t py0 = 0;
static int16_t px1 = 0;
static int16_t py1 = 0;

#define WIDTH 320
#define HEIGHT 240
#define BYTES_WIDTH 40
#define BYTES_HEIGHT 30
#define BUFFER_BASE 0x40000000
#define SETBIT(X) (1 << X)

void setPixel(int16_t x, int16_t y);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
void drawLine3D(point3d_t p0, point3d_t p1, camera_t cam); //для оптимизации без структуры поинтв
void drawFig2D(figure2D_t self, camera_t cam, uint8_t n);
void drawFig3D(figure3D_t self, camera_t cam, uint8_t n);
point2d_t project3D_to_2D(point3d_t p3d, camera_t cam);

void setPixel(int16_t x, int16_t y) {
    tft.drawPixel(x, y, TFT_RED);
    /*if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;

    uint32_t byteIndex = y * BYTES_WIDTH + (x >> 3);
    uint8_t bitMask = 1 << (7 - (x & 0x07));
    volatile uint8_t* buff = (uint8_t*)(0x40000000);
    buff[byteIndex] |= bitMask;*/
}

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    int16_t e2;

    while (1) {
        tft.drawPixel(x0, y0, TFT_RED);
        
        if (x0 == x1 && y0 == y1) break;
        
        e2 = 2 * err;
        
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void drawFig2D(figure2D_t self, camera_t cam, uint8_t n){

    for (int i = 0; i < n-1; ++i) {
        px0 = verts2D[i].x + self->centre.x + cam.x;
        py0 = verts2D[i].y + self->centre.y + cam.y;
        px1 = verts2D[i + 1].x + self->centre.x + cam.x;
        py1 = verts2D[i + 1].y + self->centre.y + cam.y;
        drawLine(px0, py0, px1, py1);
    }
        px0 = verts2D[n-1].x + self->centre.x + cam.x;
        py0 = verts2D[n-1].y + self->centre.y + cam.y;
        px1 = verts2D[0].x + self->centre.x + cam.x;
        py1 = verts2D[0].y + self->centre.y + cam.y;
        drawLine(px0, py0, px1, py1);
}

void render2Drect(figure2D_t self, camera_t cam){
    hw = self->rect.dx >> 1;
    hh = self->rect.dy >> 1;
    
    width_vec.x = cos_mult(self->rotation, hw);  
    width_vec.y = sin_mult(self->rotation, hw);
    
    height_vec.x = -sin_mult(self->rotation, hh); 
    height_vec.y = cos_mult(self->rotation, hh);
    
    verts2D[0] = (point2d_t){-width_vec.x - height_vec.x, -width_vec.y - height_vec.y}; // левый верхний
    verts2D[1] = (point2d_t){ width_vec.x - height_vec.x,  width_vec.y - height_vec.y}; // правый верхний  
    verts2D[2] = (point2d_t){ width_vec.x + height_vec.x,  width_vec.y + height_vec.y}; // правый нижний
    verts2D[3] = (point2d_t){-width_vec.x + height_vec.x, -width_vec.y + height_vec.y}; // левый нижний

    for(int i = 0; i < 4; ++i){
        printf("%d %d\n",verts2D[i].x, verts2D[i].y);
    }

    uint8_t n = 4;
    drawFig2D(self, cam, n);
}

void render2Dtriang(figure2D_t self, camera_t cam){
    uint8_t n = 3;
    r = self->triang.r;

    int16_t angle_temp = 0;
    for (int i = 0; i < n; i++) {
        int16_t angle = self->rotation + angle_temp;
        verts2D[i].x = sin_mult(angle, r);
        verts2D[i].y = -cos_mult(angle, r);
        angle_temp += 120;
    }
    drawFig2D(self, cam, n);
}

void render2Dcircle(figure2D_t self, camera_t cam){
    int16_t r = self->circle.r;
    uint8_t n = self->circle.heights_num;
            
    uint16_t angle_temp = 0;
    for (int i = 0; i < n; i++) {
        int16_t angle = self->rotation + angle_temp;
        verts2D[i].x = sin_mult(angle, r);
        verts2D[i].y = -cos_mult(angle, r);
        angle_temp += 22;
    }
    drawFig2D(self, cam, n);
}

#define SCALE_SHIFT 8

void render3Drect(figure3D_t self, camera_t cam){

    int16_t hx = self->rect.dx >> 1;
    int16_t hy = self->rect.dy >> 1;
    int16_t hz = self->rect.dz >> 1;
    
    point3d_t x_vec, y_vec, z_vec;
    
	// --- правильная композиция вращений: R = Rz(yaw) * Ry(pitch) * Rx(roll)
	int32_t sY = sin_deg(self->rotation.yaw);
	int32_t cY = sin_deg(90 - self->rotation.yaw);
	int32_t sP = sin_deg(self->rotation.pitch);
	int32_t cP = sin_deg(90 - self->rotation.pitch);
	int32_t sR = sin_deg(self->rotation.roll);
	int32_t cR = sin_deg(90 - self->rotation.roll);

	// элементы матрицы R (фиксированная точка с масштабом 1<<SCALE_SHIFT)
	int32_t r00 = (cY * cP) >> SCALE_SHIFT;
	int32_t r01 = ( ((cY * sP) >> SCALE_SHIFT) * sR >> SCALE_SHIFT ) - ((sY * cR) >> SCALE_SHIFT);
	int32_t r02 = ( ((cY * sP) >> SCALE_SHIFT) * cR >> SCALE_SHIFT ) + ((sY * sR) >> SCALE_SHIFT);

	int32_t r10 = (sY * cP) >> SCALE_SHIFT;
	int32_t r11 = ( ((sY * sP) >> SCALE_SHIFT) * sR >> SCALE_SHIFT ) + ((cY * cR) >> SCALE_SHIFT);
	int32_t r12 = ( ((sY * sP) >> SCALE_SHIFT) * cR >> SCALE_SHIFT ) - ((cY * sR) >> SCALE_SHIFT);

	int32_t r20 = (-sP);
	int32_t r21 = (cP * sR) >> SCALE_SHIFT;
	int32_t r22 = (cP * cR) >> SCALE_SHIFT;

	// осевые векторы = столбцы R, домноженные на полуразмеры
	x_vec.x = (int16_t)(((int32_t)hx * r00) >> SCALE_SHIFT);
	x_vec.y = (int16_t)(((int32_t)hx * r10) >> SCALE_SHIFT);
	x_vec.z = (int16_t)(((int32_t)hx * r20) >> SCALE_SHIFT);

	y_vec.x = (int16_t)(((int32_t)hy * r01) >> SCALE_SHIFT);
	y_vec.y = (int16_t)(((int32_t)hy * r11) >> SCALE_SHIFT);
	y_vec.z = (int16_t)(((int32_t)hy * r21) >> SCALE_SHIFT);

	z_vec.x = (int16_t)(((int32_t)hz * r02) >> SCALE_SHIFT);
	z_vec.y = (int16_t)(((int32_t)hz * r12) >> SCALE_SHIFT);
	z_vec.z = (int16_t)(((int32_t)hz * r22) >> SCALE_SHIFT);
    
    verts3D[0] = (point3d_t){-x_vec.x - y_vec.x - z_vec.x, -x_vec.y - y_vec.y - z_vec.y, -x_vec.z - y_vec.z - z_vec.z};
    verts3D[1] = (point3d_t){ x_vec.x - y_vec.x - z_vec.x,  x_vec.y - y_vec.y - z_vec.y,  x_vec.z - y_vec.z - z_vec.z};
    verts3D[2] = (point3d_t){ x_vec.x + y_vec.x - z_vec.x,  x_vec.y + y_vec.y - z_vec.y,  x_vec.z + y_vec.z - z_vec.z};
    verts3D[3] = (point3d_t){-x_vec.x + y_vec.x - z_vec.x, -x_vec.y + y_vec.y - z_vec.y, -x_vec.z + y_vec.z - z_vec.z};
    
    verts3D[4] = (point3d_t){-x_vec.x - y_vec.x + z_vec.x, -x_vec.y - y_vec.y + z_vec.y, -x_vec.z - y_vec.z + z_vec.z};
    verts3D[5] = (point3d_t){ x_vec.x - y_vec.x + z_vec.x,  x_vec.y - y_vec.y + z_vec.y,  x_vec.z - y_vec.z + z_vec.z};
    verts3D[6] = (point3d_t){ x_vec.x + y_vec.x + z_vec.x,  x_vec.y + y_vec.y + z_vec.y,  x_vec.z + y_vec.z + z_vec.z};
    verts3D[7] = (point3d_t){-x_vec.x + y_vec.x + z_vec.x, -x_vec.y + y_vec.y + z_vec.y, -x_vec.z + y_vec.z + z_vec.z};
    
    for (int i = 0; i < 8; i++) {
        verts3D[i].x += self->centre.x;
        verts3D[i].y += self->centre.y;
        verts3D[i].z += self->centre.z;
    }

    for (int i = 0; i < 3; i++) 
        drawLine3D(verts3D[i], verts3D[i + 1], cam); 
    drawLine3D(verts3D[3], verts3D[0], cam);

    // Верх 4-5-6-7  
    for (int i = 0; i < 3; i++) {
        drawLine3D(verts3D[i + 4], verts3D[i + 5], cam);
    }
    drawLine3D(verts3D[7], verts3D[4], cam);

    //боковые ништяки 0-4, 1-5, 2-6, 3-7
    for (int i = 0; i < 4; i++) {
        drawLine3D(verts3D[i], verts3D[i + 4], cam);
    }
}

void render3Dtriang(figure3D_t self, camera_t cam){
    int16_t r = self->pyramid.base >> 1;
    int16_t h = self->pyramid.height;
    verts3D[0] = (point3d_t){ -r, -r, 0 };
    verts3D[1] = (point3d_t){  r, -r, 0 };
    verts3D[2] = (point3d_t){  r,  r, 0 };
    verts3D[3] = (point3d_t){ -r,  r, 0 };
    verts3D[4] = (point3d_t){ 0, 0, h };

    drawLine3D(verts3D[0], verts3D[1], cam);
    drawLine3D(verts3D[1], verts3D[2], cam);
    drawLine3D(verts3D[2], verts3D[3], cam);
    drawLine3D(verts3D[3], verts3D[0], cam);
    drawLine3D(verts3D[4], verts3D[0], cam);
    drawLine3D(verts3D[4], verts3D[1], cam);
    drawLine3D(verts3D[4], verts3D[2], cam);
    drawLine3D(verts3D[4], verts3D[3], cam);
}

void render3Dcircle(figure3D_t self, camera_t cam){
    int16_t r = self->sphere.r;
    uint8_t n = self->sphere.vert_num;
    
    for (int i = 0; i < n; i++) {
        int16_t angle = (i * 360) / n;
        verts3D[i].x = sin_mult(angle, r);
        verts3D[i].y = -cos_mult(angle, r);
        verts3D[i].z = 0;
    }
    for (int i = 0; i < n - 1; i++) {
        drawLine3D(verts3D[i], verts3D[i + 1], cam);
    }
    drawLine3D(verts3D[n-1], verts3D[0], cam);
}

point2d_t project3D_to_2D(point3d_t p3d, camera_t cam) {
    point2d_t p2d;

    int32_t scale = 512; // fixed-point 8.8 format
    
    // Избегаем деления на 0 и отрицательных Z
    int16_t z_distance = p3d.z - cam.z;
    if (z_distance <= 0) z_distance = 1; // минимальная дистанция
    
    // scale = 256 / z_distance (fixed-point)
    scale = (scale << 9) / z_distance; // 256 * 256 / z_distance
    
    // Применяем масштабирование (fixed-point умножение)
    p2d.x = (int16_t)(((p3d.x - cam.x) * scale) >> 9) + 120; // 240/2 = 120
    p2d.y = (int16_t)(((p3d.y - cam.y) * scale) >> 9) + 160; // 320/2 = 160
    
    return p2d;
}

void drawLine3D(point3d_t p0, point3d_t p1, camera_t cam) {
    point2d_t screen_p0 = project3D_to_2D(p0, cam);
    point2d_t screen_p1 = project3D_to_2D(p1, cam);
    
    int16_t x0 = screen_p0.x;
    int16_t y0 = screen_p0.y;
    int16_t x1 = screen_p1.x;
    int16_t y1 = screen_p1.y;


    drawLine(x0, y0, x1, y1);
}
