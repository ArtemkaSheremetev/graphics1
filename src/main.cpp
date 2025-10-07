#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include "main.h"
#include <vector>
#include "geomentry.h"
#include "trigtable.h"
#include "math.h"

#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

void testTrigTable();
void loopik(void* pvParams);
void loopik1(void* pvParams);
TaskHandle_t task;
uint8_t counterFig2D = 0;
figure2D_t* figures2D[10];

extern "C" void app_main()
{
  initArduino();
  Serial.begin(115200);
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(1);
  xTaskCreate(loopik1, "loopik", 20000, NULL, 0, &task);
}
void loopik1(void* pvParams){
  camera_t cam = {0, 0, -300};
  
  #define NUM_TRIANGLES 6
  figure2D_t triangles[NUM_TRIANGLES];
  int16_t rotation_speeds[NUM_TRIANGLES];
  
  // Треугольники разных размеров в разных позициях
  triangles[0] = init2D(SHAPE_TRIANG);
  triangles[0]->centre = {60, 80};
  triangles[0]->triang = {25};
  rotation_speeds[0] = 2;

  triangles[1] = init2D(SHAPE_TRIANG);
  triangles[1]->centre = {180, 80};
  triangles[1]->triang = {35};
  rotation_speeds[1] = -2;

  triangles[2] = init2D(SHAPE_TRIANG);
  triangles[2]->centre = {120, 120};
  triangles[2]->triang = {45};
  rotation_speeds[2] = 2;

  triangles[3] = init2D(SHAPE_TRIANG);
  triangles[3]->centre = {60, 200};
  triangles[3]->triang = {30};
  rotation_speeds[3] = 2;

  triangles[4] = init2D(SHAPE_TRIANG);
  triangles[4]->centre = {180, 200};
  triangles[4]->triang = {40};
  rotation_speeds[4] = 1;

  triangles[5] = init2D(SHAPE_TRIANG);
  triangles[5]->centre = {120, 240};
  triangles[5]->triang = {20};
  rotation_speeds[5] = 3;

  while (1) {
    tft.fillScreen(TFT_BLACK);
    
    for (int i = 0; i < NUM_TRIANGLES; i++) {
      triangles[i]->methods.rotate(triangles[i], rotation_speeds[i]);
      triangles[i]->methods.render(triangles[i], cam);
    }
    
    vTaskDelay(pdMS_TO_TICKS(50)); 
  }
}

void loopik(void* pvParams){
  camera_t cam = {120, 160, -300};
  
  figure2D_t rect0 = init2D(SHAPE_RECT);
  rect0->centre = {120, 160};
  rect0->rect = {60,60};

  figure2D_t triang0 = init2D(SHAPE_RECT);
  triang0->centre = {120, 160};
  triang0->triang = {60};

  figure2D_t circle0 = init2D(SHAPE_CIRCLE);
  circle0->centre = {120, 160};
  circle0->circle.r = 50;
  circle0->circle.heights_num = 32;

  figure3D_t cube0 = init3D(SHAPE_RECT);
  cube0->centre = {120, 160, 0};
  cube0->rect = {60, 60, 60};

  TS_Point p;
  TS_Point prev_p = {0, 0, 0};
  bool is_touching = false;
  int16_t prev_x = 0, prev_y = 0;
  
  while (1) {
    p = ts.getPoint();
    
    if (p.z > 0) {
      if (!is_touching) {
        prev_x = p.x;
        prev_y = p.y;
        is_touching = true;
      } else {
        int16_t delta_x = (p.x - prev_x) >> 1;
        int16_t delta_y = (p.y - prev_y) >> 1;
        
        if (abs(delta_x) > 2 || abs(delta_y) > 2) { 
          cube0->methods.rotate(cube0, delta_y, delta_x, 5);
          prev_x = p.x;
          prev_y = p.y;
        }
      }
    } else {
      is_touching = false;
      
      cube0->methods.rotate(cube0, -2, -2, -2);
    }
    
    cube0->methods.render(cube0, cam);
    vTaskDelay(pdMS_TO_TICKS(40)); 
    tft.fillScreen(TFT_BLACK);
  }
}

void testTrigTable() {
    Serial.println("deg | mine_sin | math_sin | mine_cos | math_cos");
    Serial.println("----+----------+----------+----------+----------");

    for (int deg = 0; deg <= 360; deg++) {
        int16_t mine_sin = sin_mult(deg, 100);
        int16_t mine_cos = cos_mult(deg, 100);

        float rad = deg * M_PI / 180.0f;
        float math_sin = 100.0f * sin(rad);
        float math_cos = 100.0f * cos(rad);

        Serial.printf("%3d | %8d | %8.3f | %8d | %8.3f\n",
                      deg, mine_sin, math_sin, mine_cos, math_cos);
    }
}