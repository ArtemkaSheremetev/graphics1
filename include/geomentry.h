#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <stdint.h>
#include <stdarg.h>

typedef enum{
    SHAPE_RECT = 0,
    SHAPE_TRIANG,
    SHAPE_CIRCLE
}shape_type_t;

typedef enum{
    DIM_2D = 0,
    DIM_3D
}dim_type_t;

typedef struct{
    int16_t x;
    int16_t y;
}point2d_t;

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}point3d_t;

typedef struct {
    int16_t x; 
    int16_t y;
    int16_t z;
} camera_t;

typedef struct {
    int16_t roll;  
    int16_t yaw;    
    int16_t pitch; 
} rotation3d_t;

///////////////////////////////////////////////////////////
typedef struct FIGURE2D* figure2D_t;

typedef struct FIGURE2D{
    shape_type_t shape;
      point2d_t centre;
      int16_t rotation;  

    union{
        struct{
            uint16_t dx;
            uint16_t dy;
        }rect;

        struct{
            uint16_t r;
        }triang;

        struct{
            uint16_t r;
            uint8_t heights_num;
        }circle;
    };
    struct{
      void (*scale)(figure2D_t self, int8_t kx, int8_t ky);
      void (*rotate)(figure2D_t self, int16_t angle);
      void (*shift)(figure2D_t self, int16_t x, int16_t y);
      void (*render)(figure2D_t self, camera_t);  
    }methods;

}*figure2D_t;
///////////////////////////////////////////////////////////
typedef struct FIGURE3D* figure3D_t;

typedef struct FIGURE3D {
    shape_type_t shape;
    point3d_t centre;
    rotation3d_t rotation; 
    union {
        struct {
            uint16_t dx, dy, dz;  
        } rect;

        struct {
            uint16_t r;
            uint16_t base; 
            uint16_t height;
        } pyramid;

        struct {
            uint16_t r;
            uint8_t vert_num;
        } sphere;
    };
    struct
    {
        void (*scale)(figure3D_t self, int8_t kx, int8_t ky, int8_t kz);
        void (*rotate)(figure3D_t self, int16_t yaw, int16_t pitch, int16_t roll);
        void (*shift)(figure3D_t self, int16_t x, int16_t y, int16_t z);
        void (*render)(figure3D_t self, camera_t);
    }methods;
    
} *figure3D_t;
/////////////////////////////////////////////////////////////////////////
typedef struct FIGURE* figure_t;

typedef struct FIGURE{
    dim_type_t dimension;
    union
    {
        figure2D_t f2D;
        figure3D_t f3D;
    };
    struct
    {
        uint16_t* buff;
        uint8_t width;
        uint8_t height;
    }sprite;
    
}*figure_t; //мне стало лень потом доделаю пох !пока не используется поэтому в вверхних struct sprite

///////////////////////////////////////////////////////////
figure2D_t init2D(shape_type_t shape);
void scale2D(figure2D_t self, int8_t kx, int8_t ky);
void rotate2D(figure2D_t self, int16_t angle);
void shift2D (figure2D_t self, int16_t dx, int16_t dy);
void destroy2D(figure2D_t self);
///////////////////////////////////////////////////////////
figure3D_t init3D(shape_type_t shape);
void scale3D(figure3D_t self, int8_t kx, int8_t ky, int8_t kz);
void rotate3D(figure3D_t self, int16_t row, int16_t yaw, int16_t pitch);
void shift3D(figure3D_t self, int16_t x, int16_t y, int16_t z);
void destroy3D(figure3D_t self);
///////////////////////////////////////////////////////////
int8_t get_shift(int8_t k);

#endif
