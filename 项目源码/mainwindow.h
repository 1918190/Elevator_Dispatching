#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPropertyAnimation>
#include <QtCore>
#include <controller.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr); //构造函数
  void init();                           //分配ui窗口部件
  void setAnimation();                   //设置动画
  void setConnection();                  //为槽函数与信号设置连接
  void openDoor(int elevatorID);         //设置开门动画
  void personIn(int elevatorID);         //人物进入电梯动画
  void personOut(int elevatorID);        //人物离开电梯动画

  ~MainWindow(); //析构函数

private slots:
  void setDoorTop(int elevatorID);   //将电梯门视图放在最上面
  void setPersonTop(int elevatorID); //将小人视图放在最上面
  void clickFloor();                 //点击电梯内楼层按钮事件
  void clickDirection();             //点击楼层中上行/下行按钮事件
  void clickWarning();               //点击电梯内警报按钮事件
  void clickPhone();                 //点击电梯内呼叫按钮事件
  void clickOpen();                  //点击电梯内开门按钮事件
  void clickClose();                 //点击电梯内关门按钮事件
  void doorOpenAni(int elevatorID);  //人物进电梯整体动画
  void doorCloseAni(int elevatorID); //人物出电梯整体动画
  void changeButton(int elevatorID, int state); //改变电梯上行/下行状态显示
  void closeDoor(int elevatorID);               //设置关门动画
  void recoverButton(int direction, int floor); //恢复按钮状态

signals:
  void warning(int elevatorID);                //警报键按下信号
  void phone(int elevatorID);                  //呼叫键按下信号
  void controlDoor(int elevatorID, int state); //电梯内开门/关门按钮按下信号
  void inMessage(int elevatorID, int floor); //电梯内楼层按钮按下信号
  void outMessage(int direction, int floor); //楼层中上行/下行按钮按下信号

private:
  void getMessage(QString objectName, int *curMessage);
  //获取请求内容与信息
  Ui::MainWindow *ui;                    // ui界面
  Controller *ctrl;                      //控制器实例
  QPushButton *floorButton[20][2];       //楼层上行/下行按钮
  QPropertyAnimation *elevAnimation[10]; //电梯开门/关门动画
  QPropertyAnimation *preAnimation[5];   //小人进出动画
  QGraphicsView *person[5];              //小人视图
  QGraphicsView *door[10];               //电梯门视图
  QGraphicsView *upDownButton[5];        //电梯上行/下行状态视图
  QMediaPlaylist *playerList;            //音乐播放列表
  QMediaPlayer *player;                  //音乐播放器
};
#endif // MAINWINDOW_H
