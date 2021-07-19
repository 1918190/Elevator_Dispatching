#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <elevator.h>

class Controller : public QWidget {
  Q_OBJECT

public:
  Controller();                                 //构造函数
  void updateMinMax(int elevatorID, int floor); //更新最高/最低请求楼层
  void reachMinMax(int elevatorID); //判断是否达到最高/最低请求楼层
  void changeElevator(int elevatorID); //更换服务电梯
  Elevator myElevator[5];              //五部电梯
  QCheckBox *calDoorTime;              //计算开门时间功能显示
  QCheckBox *calRepairTime; //电梯维修时间过长更换服务电梯功能显示
  QCheckBox *calOpenTime; //关门时间过长惩罚功能显示

private slots:
  void warningState(int elevatorID);                //响应警报键被按下
  void phoneState(int elevatorID);                  //响应呼叫键被按下
  void controlDoorState(int elevatorID, int state); //响应开门/关门键被按下
  void distribute(int elevatorID, int floor); //响应电梯内楼层按钮被按下
  void selectBestElev(int direction, int floor); //响应楼层中上行/下行按钮被按下
  void update();                                 //系统状态更新函数

signals:
  void open(int elevatorID);              //开门信号
  void close(int elevatorID);             //关门信号
  void change(int elevatorID, int state); //电梯上行/下行状态显示改变信号
  void recover(int direction, int floor); //按钮恢复信号

private:
  int maxFloor[5] = {1};  //电梯上行时最高请求楼层
  int minFloor[5] = {20}; //电梯下行时最低请求楼层
  int flag[5] = {0};      //判断拐点
};

#endif // CONTROLLER_H
