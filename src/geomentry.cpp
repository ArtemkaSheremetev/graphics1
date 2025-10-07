#include "geomentry.h"
#include "math.h"
#include "malloc.h"
#include "trigtable.h"
#include "render.h"
#include "SPI.h"
#include "TFT_eSPI.h"

extern TFT_eSPI tft;

extern void render2Drect(figure2D_t self, camera_t cam);
extern void render2Dtriang(figure2D_t self, camera_t cam);
extern void render2Dcircle(figure2D_t self, camera_t cam);
extern void render3Drect(figure3D_t self, camera_t cam);
extern void render3Dtriang(figure3D_t self, camera_t cam);
extern void render3Dcircle(figure3D_t self, camera_t cam);


figure2D_t init2D(shape_type_t shape) {
    figure2D_t temp = (figure2D_t)malloc(sizeof(struct FIGURE2D));
    if (!temp) {
        return NULL;
        Serial.printf("ХУЙ");
    }
    
    temp->shape  = shape;
    temp->centre = (point2d_t){0, 0};
    temp->rotation = 0;

    temp->methods.shift = shift2D;
    temp->methods.scale = scale2D;
    temp->methods.rotate = rotate2D;

    switch (shape) {
        case SHAPE_RECT:
            temp->methods.render = render2Drect;
            temp->rect.dx = 20;   
            temp->rect.dy = 10;
            Serial.printf("%d %d\n", temp->rect.dx, temp->rect.dy);    
            break;

        case SHAPE_TRIANG:
            temp->methods.render = render2Dtriang;
            temp->triang.r = 10;
            Serial.printf("%d\n", temp->triang.r);  
            break;

        case SHAPE_CIRCLE:
            temp->methods.render = render2Dcircle;
            temp->circle.heights_num = 16;
            temp->circle.r = 10; 
            Serial.printf("%d\n", temp->circle.r);  
            break;
    }
    return temp;
}

void scale2D(figure2D_t self, int8_t kx, int8_t ky) {

    int8_t shift_x = get_shift(kx);
    int8_t shift_y = get_shift(ky);

    switch (self->shape) {
        case SHAPE_RECT:
            if (shift_x >= 0) self->rect.dx <<= shift_x;
            else self->rect.dx >>= (-shift_x);
            
            if (shift_y >= 0) self->rect.dy <<= shift_y;
            else self->rect.dy >>= (-shift_y);
            break;
            
        case SHAPE_TRIANG:
            if (shift_x >= shift_y) {
                if (shift_x >= 0) self->triang.r <<= shift_x;
                else self->triang.r >>= (-shift_x);
            } else {
                if (shift_y >= 0) self->triang.r <<= shift_y;
                else self->triang.r >>= (-shift_y);
            }
            break;
            
        case SHAPE_CIRCLE:
            if (shift_x >= shift_y) {
                if (shift_x >= 0) self->circle.r <<= shift_x;
                else self->circle.r >>= (-shift_x);
            } else {
                if (shift_y >= 0) self->circle.r <<= shift_y;
                else self->circle.r >>= (-shift_y);
            }
            break;
    }
}

inline void rotate2D(figure2D_t self, int16_t angle) {
    self->rotation = (self->rotation + angle);
    while(self->rotation > 360) {self->rotation -= 360;}
    while(self->rotation < 0) {self->rotation += 360;}
}

inline void shift2D(figure2D_t self, int16_t dx, int16_t dy) {
    self->centre.x += dx;
    self->centre.y += dy;
}

///////////////////////////////////////////////////////////////////////////////////////

figure3D_t init3D(shape_type_t shape) {
    figure3D_t temp = (figure3D_t)malloc(sizeof(struct FIGURE3D));
    if (!temp) return NULL;

    temp->shape = shape;
    temp->centre = (point3d_t){0, 0, 0};
    temp->rotation = (rotation3d_t){0, 0, 0};

    temp->methods.rotate = rotate3D;
    temp->methods.scale  = scale3D;
    temp->methods.shift  = shift3D;


    switch (shape) {
        case SHAPE_RECT:
            temp->rect.dx = 20;   
            temp->rect.dy = 20;
            temp->rect.dz = 20;
            temp->methods.render = render3Drect;
            break;

        case SHAPE_TRIANG:
            temp->pyramid.base  = 20;
            temp->pyramid.height = 30;
            temp->pyramid.r = 10;
            temp->methods.render = render3Dtriang;
            break;

        case SHAPE_CIRCLE: 
            temp->sphere.r = 10;
            temp->methods.render = render3Dcircle;
            break;
    }
    Serial.print("INIT \n");
    return temp;
}

void scale3D(figure3D_t self, int8_t kx, int8_t ky, int8_t kz) {
    if (!self) return;

    int8_t shift_x = get_shift(kx);
    int8_t shift_y = get_shift(ky);
    int8_t shift_z = get_shift(kz);

    switch (self->shape) {
        case SHAPE_RECT: {
            if (shift_x >= 0) self->rect.dx <<= shift_x;
            else if (shift_x < 0) self->rect.dx >>= (-shift_x);
            
            if (shift_y >= 0) self->rect.dy <<= shift_y;
            else if (shift_y < 0) self->rect.dy >>= (-shift_y);
            
            if (shift_z >= 0) self->rect.dz <<= shift_z;
            else if (shift_z < 0) self->rect.dz >>= (-shift_z);
            break;
        }

        case SHAPE_TRIANG: {
            int8_t base_shift = (shift_x > shift_y ? shift_x : shift_y);
            if (base_shift >= 0) self->pyramid.base <<= base_shift;
            else if (base_shift < 0) self->pyramid.base >>= (-base_shift);
            
            if (shift_z >= 0) self->pyramid.height <<= shift_z;
            else if (shift_z < 0) self->pyramid.height >>= (-shift_z);
            break;
        }

        case SHAPE_CIRCLE: {
            int8_t max_shift = shift_x;
            if (shift_y > max_shift) max_shift = shift_y;
            if (shift_z > max_shift) max_shift = shift_z;
            
            if (max_shift >= 0) self->sphere.r <<= max_shift;
            else if (max_shift < 0) self->sphere.r >>= (-max_shift);
            break;
        }

        default:
            break;
    }
}

void rotate3D(figure3D_t self, int16_t roll, int16_t yaw, int16_t pitch) {
    self->rotation.roll  = (self->rotation.roll + roll);
    while(self->rotation.roll > 360) {self->rotation.roll -= 360;}
    while(self->rotation.roll < 0) {self->rotation.roll += 360;}

    self->rotation.yaw   = (self->rotation.yaw + yaw);
    while(self->rotation.yaw > 360) {self->rotation.yaw -= 360;}
    while(self->rotation.yaw < 0) {self->rotation.yaw += 360;}

    self->rotation.pitch = (self->rotation.pitch + pitch);
    while(self->rotation.pitch > 360) {self->rotation.pitch -= 360;}
    while(self->rotation.pitch < 0) {self->rotation.pitch += 360;}
}

void shift3D(figure3D_t self, int16_t dx, int16_t dy, int16_t dz) {
    self->centre.x += dx;
    self->centre.y += dy;
    self->centre.z += dz;
}

inline void destroy2D(figure2D_t self){
    free(self);
}

inline void destroy3D(figure3D_t self){
    free(self);
}

inline int8_t get_shift(int8_t k) {
    switch (k) {
        case -32: return -5;
        case -16: return -4;
        case -8:  return -3;
        case -4:  return -2;
        case -2:  return -1;
        case 2:   return 1;
        case 4:   return 2;
        case 8:   return 3;
        case 16:  return 4;
        case 32:  return 5;
        default: return 2;
    }
}
