#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QCheckBox>
#include <QLCDNumber>
#include <QPushButton>
#include <QSlider>

class Elevator {
public:
  Elevator();                      //构造函数
  QSlider *elevatorUi;             //电梯井视图
  QPushButton *floorButton[24];    //电梯内部按钮
  QCheckBox *lock;                 //电梯是否停止显示
  QLCDNumber *elevatorLCD;         //电梯当前楼层数码显示管
  bool insideStatus[20] = {false}; //电梯内部任务队列
  bool outsideStatus[20][2] = {{false}, {false}}; //电梯外部任务队列
  int timeInterval = 3;                           //一次开门时间间隔
  int repairTime = 0;                             //维修时间
  int openTime = 0;                               //开门时间
  int direction = 2;                              //电梯运行状态
  int currentFloor = 1;                           //电梯当前所在楼层
  int runningState = 1;                           //电梯工作状态
  int doorState = 0;                              //电梯门状态
};

#endif // ELEVATOR_H
