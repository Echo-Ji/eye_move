#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h> 

///////////////////////////////////////////
// 装置测量信息宏定义
//////////////////////////////////////////
#define EYECOUNT_X 11
#define EYECOUNT_Y 11
#define BASE_DISTANCE 100
#define EYE_MATRIX_WIDTH 73
#define EYE_MATRIX_HEIGHT 50
#define SERVOMIN  110 
#define SERVOMAX  510 
#define SERVO_PANEL_COUNT 8
#define SERVO_FREQ 50

////////////////////////////////////////////
// 全局变量定义
////////////////////////////////////////////
/**
 * 装置预设信息
 */
// 根据装置各模块的几何位置确定总宽度和总高度
float total_width = BASE_DISTANCE*2 + EYE_MATRIX_WIDTH,
      total_height = BASE_DISTANCE + EYE_MATRIX_HEIGHT;
// 根据眼球阵列确定每个眼球cell的宽和高
float cell_width = EYE_MATRIX_WIDTH / (EYECOUNT_Y-1), 
      cell_height = EYE_MATRIX_HEIGHT / (EYECOUNT_X-1);

/**
 * 程序全局变量
 */
float angle; 
// 当前位置向量
float vec_x = 0.0, vec_y = 0.0;
// 当前用户位置
float pos_x = total_height, pos_y = total_width/2+10;

typedef struct{
  uint16_t id;
  uint16_t num;
}SERVO;
// 眼球阵列的板载ID与板内序号信息
SERVO SM[11][11];

// 舵机上一次的脉冲(SERVOMIN + SERVOMAX) / 2 = 310，即90度
uint16_t last_pulse[11][11];

/**
 * 舵机驱动板定义
 */
Adafruit_PWMServoDriver pwms[SERVO_PANEL_COUNT] = {
        Adafruit_PWMServoDriver(0x41), Adafruit_PWMServoDriver(0x42),
        Adafruit_PWMServoDriver(0x43), Adafruit_PWMServoDriver(0x44),
        Adafruit_PWMServoDriver(0x45), Adafruit_PWMServoDriver(0x46),
        Adafruit_PWMServoDriver(0x47), Adafruit_PWMServoDriver(0x48)
        };

void setup() {
  Serial.begin(9600);
  
  // 初始化眼球阵列（位置和角度）
  init_EM();

  // 初始化舵机控制板
  init_servo_panel();

  // 初始化板载ID与板内序号信息
  init_SM();
}

void loop() {
  pos_y = pos_y - 30;
  if(pos_y >= -273/2){
    servoSweep();
  }
}


/////////////////////////////////////////////
// 帮助函数
/////////////////////////////////////////////
/**
 * 初始化眼球阵列的角度（脉冲宽度）
 */
void init_EM(){   
  for(uint16_t i=0; i<EYECOUNT_X; i++){
    for(uint16_t j=0; j<EYECOUNT_Y; j++){
      last_pulse[i][j] = (SERVOMIN+SERVOMAX)/2; // 脉冲宽度
    }
  }
  return;
}

/**
 * 初始化所有舵机控制板
 */
void init_servo_panel(){
  for(uint16_t i=0; i<SERVO_PANEL_COUNT; i++){
    pwms[i].begin();
    pwms[i].setPWMFreq(SERVO_FREQ);
  }
}

/**
 * 计算给定向量与X轴的夹角，范围为[-90, 90]
 */
float vector2angle(float x, float y){
  return atan2f(x, y)*180*M_1_PI;
}

/**
 * 计算观众当前位置与位置[x, y]处眼球的方向向量的x分量
 */
float getDirX(uint16_t x, uint16_t y){
//  return pos_x - EM[x][y].x;
  return pos_x - cell_height*x;
}

/**
 * 计算观众当前位置与位置[x, y]处眼球的方向向量的y分量
 */
float getDirY(uint16_t x, uint16_t y){
//  return pos_y - EM[x][y].y;
  return pos_y - (-EYE_MATRIX_WIDTH/2 + cell_width*y);
}

/**
 * 计算用户当前位置与位置[x, y]处眼球的角度
 */
float getAngle(uint16_t x, uint16_t y){
  return vector2angle(getDirX(x, y), getDirY(x, y));
}

/**
 * 将舵机转动角度转化为脉冲大小
 */
uint16_t angle2pulse(float angle){
  return uint16_t(angle / 180 * (SERVOMAX - SERVOMIN) + SERVOMIN);
}

/**
 * 驱动所有舵机从from转动到to，参数为脉冲宽度
 */
void servoSweep(){
  for(uint16_t i=0; i<EYECOUNT_X; i++){
    for(uint16_t j=0; j<EYECOUNT_Y; j++){
      uint16_t pulse = angle2pulse(getAngle(i, j));
      if(pulse > last_pulse[i][j]){
        for(uint16_t pulselen=last_pulse[i][j]; pulselen<pulse; pulselen+=2){
          pwms[SM[i][j].id].setPWM(SM[i][j].num, 0, pulselen);
        }
      }else{
        for(uint16_t pulselen=last_pulse[i][j]; pulselen>pulse; pulselen-=2){
          pwms[SM[i][j].id].setPWM(SM[i][j].num, 0, pulselen);
        }
      }
      // 存储上一次脉冲
      last_pulse[i][j] = pulse;
    }
  }
  delay(1000);
  return;
}

/**
 * 初始化眼球阵列的板载ID与板内序号信息
 */
void init_SM(){
  SM[0][0].id = 0;
  SM[0][0].num = 10;
  SM[1][0].id = 0;
  SM[1][0].num = 9;
  SM[2][0].id = 0;
  SM[2][0].num = 8;
  SM[3][0].id = 0;
  SM[3][0].num = 7;
  SM[4][0].id = 0;
  SM[4][0].num = 6;
  SM[5][0].id = 0;
  SM[5][0].num = 5;
  SM[6][0].id = 0;
  SM[6][0].num = 4;
  SM[7][0].id = 0;
  SM[7][0].num = 3;
  SM[8][0].id = 0;
  SM[8][0].num = 2;
  SM[9][0].id = 0;
  SM[9][0].num = 1;
  SM[10][0].id = 0;
  SM[10][0].num = 0;

  SM[0][1].id = 0;
  SM[0][1].num = 11;
  SM[1][1].id = 0;
  SM[1][1].num = 12;
  SM[2][1].id = 0;
  SM[2][1].num = 13;
  SM[3][1].id = 0;
  SM[3][1].num = 14;
  SM[4][1].id = 0;
  SM[4][1].num = 15;
  SM[5][1].id = 1;
  SM[5][1].num = 0;
  SM[6][1].id = 1;
  SM[6][1].num = 1;
  SM[7][1].id = 1;
  SM[7][1].num = 2;
  SM[8][1].id = 1;
  SM[8][1].num = 3;
  SM[9][1].id = 1;
  SM[9][1].num = 4;
  SM[10][1].id = 1;
  SM[10][1].num = 5;

  SM[0][2].id = 2;
  SM[0][2].num = 0;
  SM[1][2].id = 1;
  SM[1][2].num = 15;
  SM[2][2].id = 1;
  SM[2][2].num = 14;
  SM[3][2].id = 1;
  SM[3][2].num = 13;
  SM[4][2].id = 1;
  SM[4][2].num = 12;
  SM[5][2].id = 1;
  SM[5][2].num = 11;
  SM[6][2].id = 1;
  SM[6][2].num = 10;
  SM[7][2].id = 1;
  SM[7][2].num = 9;
  SM[8][2].id = 1;
  SM[8][2].num = 8;
  SM[9][2].id = 1;
  SM[9][2].num = 7;
  SM[10][2].id = 1;
  SM[10][2].num = 6;

  SM[0][3].id = 2;
  SM[0][3].num = 1;
  SM[1][3].id = 2;
  SM[1][3].num = 2;
  SM[2][3].id = 2;
  SM[2][3].num = 3;
  SM[3][3].id = 2;
  SM[3][3].num = 4;
  SM[4][3].id = 2;
  SM[4][3].num = 5;
  SM[5][3].id = 2;
  SM[5][3].num = 6;
  SM[6][3].id = 2;
  SM[6][3].num = 7;
  SM[7][3].id = 2;
  SM[7][3].num = 8;
  SM[8][3].id = 2;
  SM[8][3].num = 9;
  SM[9][3].id = 2;
  SM[9][3].num = 10;
  SM[10][3].id = 2;
  SM[10][3].num = 11;

  SM[0][4].id = 3;
  SM[0][4].num = 6;
  SM[1][4].id = 3;
  SM[1][4].num = 5;
  SM[2][4].id = 3;
  SM[2][4].num = 4;
  SM[3][4].id = 3;
  SM[3][4].num = 3;
  SM[4][4].id = 3;
  SM[4][4].num = 2;
  SM[5][4].id = 3;
  SM[5][4].num = 1;
  SM[6][4].id = 3;
  SM[6][4].num = 0;
  SM[7][4].id = 2;
  SM[7][4].num = 15;
  SM[8][4].id = 2;
  SM[8][4].num = 14;
  SM[9][4].id = 2;
  SM[9][4].num = 13;
  SM[10][4].id = 2;
  SM[10][4].num = 12;

  SM[0][5].id = 3;
  SM[0][5].num = 7;
  SM[1][5].id = 3;
  SM[1][5].num = 8;
  SM[2][5].id = 3;
  SM[2][5].num = 9;
  SM[3][5].id = 3;
  SM[3][5].num = 10;
  SM[4][5].id = 3;
  SM[4][5].num = 11;
  SM[5][5].id = 3;
  SM[5][5].num = 12;
  SM[6][5].id = 3;
  SM[6][5].num = 13;
  SM[7][5].id = 3;
  SM[7][5].num = 14;
  SM[8][5].id = 3;
  SM[8][5].num = 15;
  SM[9][5].id = 4;
  SM[9][5].num = 0;
  SM[10][5].id = 4;
  SM[10][5].num = 1;

  SM[0][6].id = 4;
  SM[0][6].num = 12;
  SM[1][6].id = 4;
  SM[1][6].num = 11;
  SM[2][6].id = 4;
  SM[2][6].num = 10;
  SM[3][6].id = 4;
  SM[3][6].num = 9;
  SM[4][6].id = 4;
  SM[4][6].num = 8;
  SM[5][6].id = 4;
  SM[5][6].num = 7;
  SM[6][6].id = 4;
  SM[6][6].num = 6;
  SM[7][6].id = 4;
  SM[7][6].num = 5;
  SM[8][6].id = 4;
  SM[8][6].num = 4;
  SM[9][6].id = 4;
  SM[9][6].num = 3;
  SM[10][6].id = 4;
  SM[10][6].num = 2;

  SM[0][7].id = 4;
  SM[0][7].num = 13;
  SM[1][7].id = 4;
  SM[1][7].num = 14;
  SM[2][7].id = 4;
  SM[2][7].num = 15;
  SM[3][7].id = 5;
  SM[3][7].num = 0;
  SM[4][7].id = 5;
  SM[4][7].num = 1;
  SM[5][7].id = 5;
  SM[5][7].num = 2;
  SM[6][7].id = 5;
  SM[6][7].num = 3;
  SM[7][7].id = 5;
  SM[7][7].num = 4;
  SM[8][7].id = 5;
  SM[8][7].num = 5;
  SM[9][7].id = 5;
  SM[9][7].num = 6;
  SM[10][7].id = 5;
  SM[10][7].num = 7;

  SM[0][8].id = 6;
  SM[0][8].num = 2;
  SM[1][8].id = 6;
  SM[1][8].num = 1;
  SM[2][8].id = 6;
  SM[2][8].num = 0;
  SM[3][8].id = 5;
  SM[3][8].num = 15;
  SM[4][8].id = 5;
  SM[4][8].num = 14;
  SM[5][8].id = 5;
  SM[5][8].num = 13;
  SM[6][8].id = 5;
  SM[6][8].num = 12;
  SM[7][8].id = 5;
  SM[7][8].num = 11;
  SM[8][8].id = 5;
  SM[8][8].num = 10;
  SM[9][8].id = 5;
  SM[9][8].num = 9;
  SM[10][8].id = 5;
  SM[10][8].num = 8;

  SM[0][9].id = 6;
  SM[0][9].num = 3;
  SM[1][9].id = 6;
  SM[1][9].num = 4;
  SM[2][9].id = 6;
  SM[2][9].num = 5;
  SM[3][9].id = 6;
  SM[3][9].num = 6;
  SM[4][9].id = 6;
  SM[4][9].num = 7;
  SM[5][9].id = 6;
  SM[5][9].num = 8;
  SM[6][9].id = 6;
  SM[6][9].num = 9;
  SM[7][9].id = 6;
  SM[7][9].num = 10;
  SM[8][9].id = 6;
  SM[8][9].num = 11;
  SM[9][9].id = 6;
  SM[9][9].num = 12;
  SM[10][9].id = 6;
  SM[10][9].num = 13;

  SM[0][10].id = 7;
  SM[0][10].num = 8;
  SM[1][10].id = 7;
  SM[1][10].num = 7;
  SM[2][10].id = 7;
  SM[2][10].num = 6;
  SM[3][10].id = 7;
  SM[3][10].num = 5;
  SM[4][10].id = 7;
  SM[4][10].num = 4;
  SM[5][10].id = 7;
  SM[5][10].num = 3;
  SM[6][10].id = 7;
  SM[6][10].num = 2;
  SM[7][10].id = 7;
  SM[7][10].num = 1;
  SM[8][10].id = 7;
  SM[8][10].num = 0;
  SM[9][10].id = 6;
  SM[9][10].num = 15;
  SM[10][10].id = 6;
  SM[10][10].num = 14;
  return;
}
