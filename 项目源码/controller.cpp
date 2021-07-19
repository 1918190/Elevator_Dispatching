#include "controller.h"
#include <QTimer>

#include <QDebug>
#define INF 100000000
#define INTERVAL 1 * 1000
#define TIMESLOT 3
#define MAXREPAIRTIME 10
#define ELEVATOR_NUM 5
#define FLOOR 20
#define RUN 1
#define STOP 0
#define OPEN 1
#define CLOSE 0
#define FREESTATE 2
#define UPSTATE 1
#define DOWNSTATE 0
#define UPDIRECTION -11
#define DOWNDIRECTION -28

Controller::Controller() {
  QTimer *timer = new QTimer();
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(INTERVAL);
}

void Controller::updateMinMax(int elevatorID, int floor) {
  if (myElevator[elevatorID - 1].direction == UPSTATE) {
    if (maxFloor[elevatorID - 1] < floor) {
      maxFloor[elevatorID - 1] = floor;
    }
  } else if (myElevator[elevatorID - 1].direction == DOWNSTATE) {
    if (minFloor[elevatorID - 1] > floor) {
      minFloor[elevatorID - 1] = floor;
    }
  } else if (myElevator[elevatorID - 1].direction == FREESTATE) {
    if (floor > myElevator[elevatorID - 1].currentFloor) {
      myElevator[elevatorID - 1].direction = UPSTATE;
      maxFloor[elevatorID - 1] = floor;
      emit change(elevatorID - 1, UPSTATE);
    } else if (floor < myElevator[elevatorID - 1].currentFloor) {
      myElevator[elevatorID - 1].direction = DOWNSTATE;
      minFloor[elevatorID - 1] = floor;
      emit change(elevatorID - 1, DOWNSTATE);
    } else if (floor == myElevator[elevatorID - 1].currentFloor) {
      myElevator[elevatorID - 1].direction = UPSTATE;
      maxFloor[elevatorID - 1] = floor;
      flag[elevatorID - 1] = 1;
    }
    myElevator[elevatorID - 1].elevatorUi->setStyleSheet(
        "QSlider::groove:vertical{"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 802px;"
        "width:50px;"
        "border-radius: 4px;"
        "}"
        "QSlider::handle:vertical{"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 #eee, "
        "stop:1 #ccc);"
        "border: 1px solid #777;"
        "border-image: url(:/new/image/image/little door.png);"
        "background: yellow;"
        "border-radius:4px;"
        "width: 50px;"
        "height: 50px;"
        "margin-top: -2px;"
        "margin-bottom: -2px;"
        "}");
  }
}

void Controller::reachMinMax(int i) {
  if (myElevator[i].direction == UPSTATE) {
    if (myElevator[i].currentFloor >= maxFloor[i]) {
      for (int j = 0; j < myElevator[i].currentFloor; j++) {
        if ((myElevator[i].insideStatus[j]) ||
            (myElevator[i].outsideStatus[j][UPSTATE]) ||
            (myElevator[i].outsideStatus[j][DOWNSTATE])) {
          minFloor[i] = j + 1;
          myElevator[i].direction = DOWNSTATE;
          emit change(i, DOWNSTATE);
          if (myElevator[i]
                  .outsideStatus[myElevator[i].currentFloor - 1][DOWNSTATE]) {
            flag[i] = 1;
          }
          break;
        }
        if (j == myElevator[i].currentFloor - 1) {
          myElevator[i].direction = FREESTATE;
          emit change(i, FREESTATE);
        }
      }
    }
  } else if (myElevator[i].direction == DOWNSTATE) {
    if (myElevator[i].currentFloor <= minFloor[i]) {
      for (int j = FLOOR - 1; j >= myElevator[i].currentFloor - 1; j--) {
        if ((myElevator[i].insideStatus[j]) ||
            (myElevator[i].outsideStatus[j][UPSTATE]) ||
            (myElevator[i].outsideStatus[j][DOWNSTATE])) {
          maxFloor[i] = j + 1;
          myElevator[i].direction = UPSTATE;
          emit change(i, UPSTATE);
          if (myElevator[i]
                  .outsideStatus[myElevator[i].currentFloor - 1][UPSTATE]) {
            flag[i] = 1;
          }
          break;
        }
        if (j == myElevator[i].currentFloor - 1) {
          myElevator[i].direction = FREESTATE;
          emit change(i, FREESTATE);
        }
      }
    }
  }
}

void Controller::changeElevator(int elevatorID) {
  for (int i = 0; i < FLOOR; i++) {
    if (myElevator[elevatorID - 1].outsideStatus[i][UPSTATE]) {
      myElevator[elevatorID - 1].outsideStatus[i][UPSTATE] = false;
      selectBestElev(UPSTATE, i + 1);
    }
    if (myElevator[elevatorID - 1].outsideStatus[i][DOWNSTATE]) {
      myElevator[elevatorID - 1].outsideStatus[i][DOWNSTATE] = false;
      selectBestElev(DOWNSTATE, i + 1);
    }
  }
  if (myElevator[elevatorID - 1].direction == UPSTATE) {
    for (int j = FLOOR - 1; j >= myElevator[elevatorID - 1].currentFloor - 1;
         j--) {
      if ((myElevator[elevatorID - 1].insideStatus[j])) {
        maxFloor[elevatorID - 1] = j + 1;
        break;
      }
      if (j == myElevator[elevatorID - 1].currentFloor - 1) {
        maxFloor[elevatorID - 1] = j + 1;
      }
    }
  } else if (myElevator[elevatorID - 1].direction == DOWNSTATE) {
    for (int j = 0; j < myElevator[elevatorID - 1].currentFloor; j++) {
      if (myElevator[elevatorID - 1].insideStatus[j]) {
        minFloor[elevatorID - 1] = j + 1;
        break;
      }
      if (j == myElevator[elevatorID - 1].currentFloor - 1) {
        minFloor[elevatorID - 1] = j + 1;
      }
    }
  }
  reachMinMax(elevatorID - 1);
}

void Controller::warningState(int elevatorID) {
  if (myElevator[elevatorID - 1].lock->isChecked()) {
    myElevator[elevatorID - 1].lock->setChecked(false);
    myElevator[elevatorID - 1].runningState = RUN;
    myElevator[elevatorID - 1].repairTime = 0;
    if (myElevator[elevatorID - 1].direction == FREESTATE) {
      myElevator[elevatorID - 1].elevatorUi->setStyleSheet(
          "QSlider::groove:vertical{"
          "border: 1px solid #bbb;"
          "background: white;"
          "height: 802px;"
          "width:50px;"
          "border-radius: 4px;"
          "}"
          "QSlider::handle:vertical{"
          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  "
          "stop:0 #eee, "
          "stop:1 #ccc);"
          "border: 1px solid #777;"
          "border-image: url(:/new/image/image/little door.png);"
          "background: green;"
          "border-radius:4px;"
          "width: 50px;"
          "height: 50px;"
          "margin-top: -2px;"
          "margin-bottom: -2px;"
          "}");
    } else if (myElevator[elevatorID - 1].doorState == OPEN) {
      myElevator[elevatorID - 1].elevatorUi->setStyleSheet(
          "QSlider::groove:vertical{"
          "border: 1px solid #bbb;"
          "background: white;"
          "height: 802px;"
          "width:50px;"
          "border-radius: 4px;"
          "}"
          "QSlider::handle:vertical{"
          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  "
          "stop:0 #eee, "
          "stop:1 #ccc);"
          "border: 1px solid #777;"
          "border-image: url(:/new/image/image/little door.png);"
          "background: red;"
          "border-radius:4px;"
          "width: 50px;"
          "height: 50px;"
          "margin-top: -2px;"
          "margin-bottom: -2px;"
          "}");
    } else {
      myElevator[elevatorID - 1].elevatorUi->setStyleSheet(
          "QSlider::groove:vertical{"
          "border: 1px solid #bbb;"
          "background: white;"
          "height: 802px;"
          "width:50px;"
          "border-radius: 4px;"
          "}"
          "QSlider::handle:vertical{"
          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 "
          "#eee, "
          "stop:1 #ccc);"
          "border: 1px solid #777;"
          "border-image: url(:/new/image/image/little door.png);"
          "background: yellow;"
          "border-radius:4px;"
          "width: 50px;"
          "height: 50px;"
          "margin-top: -2px;"
          "margin-bottom: -2px;"
          "}");
    }
    for (int i = 0; i < 20; i++) {
      myElevator[elevatorID - 1].floorButton[i]->setEnabled(true);
    }
  } else {
    myElevator[elevatorID - 1].lock->setChecked(true);
    myElevator[elevatorID - 1].runningState = STOP;
    myElevator[elevatorID - 1].elevatorUi->setStyleSheet(
        "QSlider::groove:vertical{"
        "border: 1px solid #bbb;"
        "background: white;"
        "height: 802px;"
        "width:50px;"
        "border-radius: 4px;"
        "}"
        "QSlider::handle:vertical{"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 #eee, "
        "stop:1 #ccc);"
        "border: 1px solid #777;"
        "border-image: url(:/new/image/image/little door.png);"
        "background: gray;"
        "border-radius:4px;"
        "width: 50px;"
        "height: 50px;"
        "margin-top: -2px;"
        "margin-bottom: -2px;"
        "}");
    for (int i = 0; i < 20; i++) {
      myElevator[elevatorID - 1].floorButton[i]->setEnabled(false);
    }
  }
}

void Controller::phoneState(int elevatorID) {
  if (myElevator[elevatorID - 1].runningState == STOP) {
    QTimer::singleShot(5 * 1000, this,
                       [&, elevatorID]() { warningState(elevatorID); });
  }
}

void Controller::controlDoorState(int elevatorID, int state) {
  if (myElevator[elevatorID - 1].doorState == OPEN) {
    if ((state - 23) == CLOSE) {
      myElevator[elevatorID - 1].timeInterval = 1;
    } else {
      myElevator[elevatorID - 1].timeInterval = 3;
    }
  }
}

void Controller::distribute(int elevatorID, int floor) {
  if (myElevator[elevatorID - 1].currentFloor == floor &&
      myElevator[elevatorID - 1].direction != FREESTATE) {
    myElevator[elevatorID - 1]
        .floorButton[myElevator[elevatorID - 1].currentFloor - 1]
        ->setStyleSheet("border-radius:15px;"
                        "border:2px solid gray;"
                        "color:black;"
                        "font: 9pt 'Stencil';");
  } else {
    myElevator[elevatorID - 1].floorButton[floor - 1]->setEnabled(false);
    myElevator[elevatorID - 1].insideStatus[floor - 1] = true;
    updateMinMax(elevatorID, floor);
  }
}

//维修时间过长换电梯(finish)
//开门时间过长延长时间(finish)
//计算开门时间(finish)

void Controller::selectBestElev(int direction, int floor) {
  int dist[5] = {INF, INF, INF, INF, INF};
  int onTheWay = direction;
  int minElev = 0;
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    if (myElevator[i].runningState != STOP) {
      if (myElevator[i].direction == FREESTATE) //电梯空闲
      {
        dist[i] = abs(myElevator[i].currentFloor - floor);
      } else if (myElevator[i].direction == onTheWay &&
                 floor > myElevator[i].currentFloor &&
                 onTheWay == UPSTATE) //向上顺路
      {
        dist[i] = floor - myElevator[i].currentFloor;
        if (calDoorTime->isChecked()) {
          for (int j = myElevator[i].currentFloor - 1; j < floor; j++) {
            if ((myElevator[i].insideStatus[j]) ||
                (myElevator[i].outsideStatus[j][UPSTATE])) {
              dist[i] += INTERVAL;
            }
          }
        }
      } else if (myElevator[i].direction == onTheWay &&
                 floor < myElevator[i].currentFloor &&
                 onTheWay == DOWNSTATE) //向下顺路
      {
        dist[i] = myElevator[i].currentFloor - floor;
        if (calDoorTime->isChecked()) {
          for (int j = floor - 1; j < myElevator[i].currentFloor; j++) {
            if ((myElevator[i].insideStatus[j]) ||
                (myElevator[i].outsideStatus[j][DOWNSTATE])) {
              dist[i] += INTERVAL;
            }
          }
        }
      } else if (myElevator[i].direction == onTheWay &&
                 floor < myElevator[i].currentFloor &&
                 onTheWay == UPSTATE) //向上不顺路
      {
        dist[i] = 38 - myElevator[i].currentFloor + floor;
        if (calDoorTime->isChecked()) {
          for (int j = 0; j < FLOOR; j++) {
            if ((myElevator[i].insideStatus[j]) ||
                (myElevator[i].outsideStatus[j][DOWNSTATE])) {

              dist[i] += INTERVAL;
            }
            if (myElevator[i].outsideStatus[j][UPSTATE]) {
              if ((j < floor) || (j >= myElevator[i].currentFloor - 1)) {
                dist[i] += INTERVAL;
              }
            }
          }
        }
      } else if (myElevator[i].direction != onTheWay &&
                 onTheWay == DOWNSTATE) //电梯向上，向下按钮
      {
        dist[i] = 40 - myElevator[i].currentFloor - floor;
        if (calDoorTime->isChecked()) {
          for (int j = myElevator[i].currentFloor - 1; j < FLOOR; j++) {
            if ((myElevator[i].insideStatus[j]) ||
                (myElevator[i].outsideStatus[j][UPSTATE])) {
              dist[i] += INTERVAL;
            }
          }
          for (int j = floor - 1; j < FLOOR; j++) {
            if (myElevator[i].outsideStatus[j][DOWNSTATE]) {
              dist[i] += INTERVAL;
            }
          }
        }
      } else if (myElevator[i].direction == onTheWay &&
                 floor > myElevator[i].currentFloor &&
                 onTheWay == DOWNSTATE) //向下不顺路
      {
        dist[i] = 38 + myElevator[i].currentFloor - floor;
        if (calDoorTime->isChecked()) {
          for (int j = 0; j < FLOOR; j++) {
            if ((myElevator[i].insideStatus[j]) ||
                (myElevator[i].outsideStatus[j][UPSTATE])) {

              dist[i] += INTERVAL;
            }
            if (myElevator[i].outsideStatus[j][DOWNSTATE]) {
              if ((j < myElevator[i].currentFloor) || (j >= floor - 1)) {
                dist[i] += INTERVAL;
              }
            }
          }
        }
      } else if (myElevator[i].direction != onTheWay &&
                 onTheWay == UPSTATE) //电梯向下，向上按钮
      {
        dist[i] = myElevator[i].currentFloor + floor - 2;
        if (calDoorTime->isChecked()) {
          for (int j = 0; j < floor; j++) {
            if (myElevator[i].outsideStatus[j][UPSTATE]) {
              dist[i] += INTERVAL;
            }
          }
          for (int j = 0; j < myElevator[i].currentFloor; j++) {
            if ((myElevator[i].insideStatus[j]) ||
                (myElevator[i].outsideStatus[j][DOWNSTATE])) {
              dist[i] += INTERVAL;
            }
          }
        }
      }
      if (calOpenTime->isChecked()) {
        if (myElevator[i].openTime > 5) {
          dist[i] += (myElevator[i].openTime - 5);
        }
      }
    }
  }
  int min = dist[0];
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    if (dist[i] < min) {
      minElev = i;
      min = dist[i];
    }
  }
  if (min == INF) {
    emit recover(onTheWay, floor);
  } else {
    myElevator[minElev].outsideStatus[floor - 1][onTheWay] = true;
    updateMinMax(minElev + 1, floor);
  }
}
//注意停靠后消除

void Controller::update() {
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    if (myElevator[i].runningState == STOP) {
      if (calRepairTime->isChecked()) {
        myElevator[i].repairTime++;
        if (myElevator[i].repairTime == MAXREPAIRTIME) {
          changeElevator(i + 1);
        }
      }
    } else {
      if (myElevator[i].doorState == OPEN) {
        myElevator[i].timeInterval--;
        myElevator[i].openTime++;
        if (myElevator[i].timeInterval == 0) {
          myElevator[i].timeInterval = TIMESLOT;
          myElevator[i].doorState = CLOSE;
          myElevator[i].openTime = 0;
          reachMinMax(i);
          if (myElevator[i].direction == FREESTATE) {
            myElevator[i].elevatorUi->setStyleSheet(
                "QSlider::groove:vertical{"
                "border: 1px solid #bbb;"
                "background: white;"
                "height: 802px;"
                "width:50px;"
                "border-radius: 4px;"
                "}"
                "QSlider::handle:vertical{"
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  "
                "stop:0 #eee, "
                "stop:1 #ccc);"
                "border: 1px solid #777;"
                "border-image: url(:/new/image/image/little door.png);"
                "background: green;"
                "border-radius:4px;"
                "width: 50px;"
                "height: 50px;"
                "margin-top: -2px;"
                "margin-bottom: -2px;"
                "}");
          } else {
            myElevator[i].elevatorUi->setStyleSheet(
                "QSlider::groove:vertical{"
                "border: 1px solid #bbb;"
                "background: white;"
                "height: 802px;"
                "width:50px;"
                "border-radius: 4px;"
                "}"
                "QSlider::handle:vertical{"
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 "
                "#eee, "
                "stop:1 #ccc);"
                "border: 1px solid #777;"
                "border-image: url(:/new/image/image/little door.png);"
                "background: yellow;"
                "border-radius:4px;"
                "width: 50px;"
                "height: 50px;"
                "margin-top: -2px;"
                "margin-bottom: -2px;"
                "}");
          }
        }
      } else {
        if (myElevator[i].direction == UPSTATE) {
          if (flag[i] == 1) {
            flag[i] = 0;
          } else {
            myElevator[i].currentFloor++;
          }
          if (myElevator[i]
                  .outsideStatus[myElevator[i].currentFloor - 1][UPSTATE]) {
            myElevator[i].doorState = OPEN;
            myElevator[i].elevatorUi->setStyleSheet(
                "QSlider::groove:vertical{"
                "border: 1px solid #bbb;"
                "background: white;"
                "height: 802px;"
                "width:50px;"
                "border-radius: 4px;"
                "}"
                "QSlider::handle:vertical{"
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 "
                "#eee, "
                "stop:1 #ccc);"
                "border: 1px solid #777;"
                "border-image: url(:/new/image/image/little door.png);"
                "background: red;"
                "border-radius:4px;"
                "width: 50px;"
                "height: 50px;"
                "margin-top: -2px;"
                "margin-bottom: -2px;"
                "}");
            emit recover(UPSTATE, myElevator[i].currentFloor);
            myElevator[i]
                .outsideStatus[myElevator[i].currentFloor - 1][UPSTATE] = false;
            open(i);
          }
        } else if (myElevator[i].direction == DOWNSTATE) {
          if (flag[i] == 1) {
            flag[i] = 0;
          } else {
            myElevator[i].currentFloor--;
          }
          if (myElevator[i]
                  .outsideStatus[myElevator[i].currentFloor - 1][DOWNSTATE]) {
            myElevator[i].doorState = OPEN;
            myElevator[i].elevatorUi->setStyleSheet(
                "QSlider::groove:vertical{"
                "border: 1px solid #bbb;"
                "background: white;"
                "height: 802px;"
                "width:50px;"
                "border-radius: 4px;"
                "}"
                "QSlider::handle:vertical{"
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 "
                "#eee, "
                "stop:1 #ccc);"
                "border: 1px solid #777;"
                "border-image: url(:/new/image/image/little door.png);"
                "background: red;"
                "border-radius:4px;"
                "width: 50px;"
                "height: 50px;"
                "margin-top: -2px;"
                "margin-bottom: -2px;"
                "}");
            emit recover(DOWNSTATE, myElevator[i].currentFloor);
            myElevator[i]
                .outsideStatus[myElevator[i].currentFloor - 1][DOWNSTATE] =
                false;
            open(i);
          }
        }
        myElevator[i].elevatorUi->setValue(myElevator[i].currentFloor);
        myElevator[i].elevatorLCD->display(myElevator[i].currentFloor);
        if (myElevator[i].insideStatus[myElevator[i].currentFloor - 1]) {
          myElevator[i]
              .floorButton[myElevator[i].currentFloor - 1]
              ->setStyleSheet("border-radius:15px;"
                              "border:2px solid gray;"
                              "color:black;"
                              "font: 9pt 'Stencil';");
          if (myElevator[i].doorState != OPEN) {
            myElevator[i].doorState = OPEN;
            myElevator[i].elevatorUi->setStyleSheet(
                "QSlider::groove:vertical{"
                "border: 1px solid #bbb;"
                "background: white;"
                "height: 802px;"
                "width:50px;"
                "border-radius: 4px;"
                "}"
                "QSlider::handle:vertical{"
                "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,  stop:0 "
                "#eee, "
                "stop:1 #ccc);"
                "border: 1px solid #777;"
                "border-image: url(:/new/image/image/little door.png);"
                "background: red;"
                "border-radius:4px;"
                "width: 50px;"
                "height: 50px;"
                "margin-top: -2px;"
                "margin-bottom: -2px;"
                "}");
          }
          close(i);
          myElevator[i].insideStatus[myElevator[i].currentFloor - 1] = false;
          myElevator[i].floorButton[myElevator[i].currentFloor - 1]->setEnabled(
              true);
        }
        if (myElevator[i].doorState == CLOSE) {
          reachMinMax(i);
        }
      }
    }
  }
}
