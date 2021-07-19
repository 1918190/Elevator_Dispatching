#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractAnimation>
#include <QTime>
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setWindowFlags(windowFlags() &
                 ~Qt::WindowMaximizeButtonHint); // 禁止最大化按钮
  setFixedSize(this->width(), this->height());
  this->setWindowTitle("Elevator");
  this->setWindowIcon(QIcon(":/new/image/image/little door.png"));
  ctrl = new Controller();
  playerList = new QMediaPlaylist;
  player = new QMediaPlayer;
  playerList->addMedia(QUrl("qrc:/new/image/music/ding.mp3"));
  playerList->setCurrentIndex(0);
  player->setPlaylist(playerList);
  player->setVolume(50);
  init();
  setAnimation();
  setConnection();
  setStyleSheet(
      //正常状态样式
      "QPushButton{"
      "border-radius:15px;"
      "border:2px solid gray;"
      "color:black;"
      "}"
      //鼠标悬停样式
      "QPushButton:hover{"
      "background-color:green;"
      "color:black;"
      "}"
      //鼠标按下样式
      "QPushButton:pressed{"
      "background-color:red;"
      "border:2px solid gray;"
      "color:rgba(0,0,0,100);"
      "}"
      //设置数字显示
      "QLCDNumber{"
      "border:20px black;"
      "color:red;"
      "background:silver;"
      "}"
      //设置滑块显示
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
      "background: green;"
      "border-radius:4px;"
      "width: 50px;"
      "height: 50px;"
      "margin-top: -2px;"
      "margin-bottom: -2px;"
      "}");
}

void MainWindow::init() {
  QList<QGraphicsView *> graphicsViewList =
      ui->centralwidget->findChildren<QGraphicsView *>();
  auto it = graphicsViewList.begin();
  for (int i = 0; i < 5; i++) {
    it++;
  }
  for (int i = 0; i < 5; i++) {
    upDownButton[i] = *it;
    it++;
  }
  for (int i = 0; i < 10; i++) {
    door[i] = *it;
    elevAnimation[i] = new QPropertyAnimation(door[i], "geometry");
    it++;
  }
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    person[i] = *it;
    person[i]->setVisible(false);
    preAnimation[i] = new QPropertyAnimation(person[i], "geometry");
    it++;
  }
  QList<QLCDNumber *> lcdNumberList =
      ui->centralwidget->findChildren<QLCDNumber *>();
  QList<QSlider *> sliderList = ui->centralwidget->findChildren<QSlider *>();
  auto it_2 = sliderList.begin();
  auto it_5 = lcdNumberList.begin();
  QList<QCheckBox *> checkboxList =
      ui->centralwidget->findChildren<QCheckBox *>();
  auto it_3 = checkboxList.begin();
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    ctrl->myElevator[i].elevatorUi = *it_2;
    ctrl->myElevator[i].lock = *it_3;
    ctrl->myElevator[i].elevatorLCD = *it_5;
    ctrl->myElevator[i].elevatorUi->setEnabled(false);
    it_2++;
    it_3++;
    it_5++;
  }
  ctrl->calDoorTime = *it_3;
  it_3++;
  ctrl->calRepairTime = *it_3;
  it_3++;
  ctrl->calOpenTime = *it_3;
  QList<QPushButton *> buttonList =
      ui->centralwidget->findChildren<QPushButton *>();
  auto it_4 = buttonList.begin();
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    for (int j = 0; j < FLOOR; j++) {
      ctrl->myElevator[i].floorButton[j] = *it_4;
      connect(*it_4, SIGNAL(clicked(bool)), this, SLOT(clickFloor()));
      it_4++;
    }
    ctrl->myElevator[i].floorButton[20] = *it_4;
    connect(*it_4, SIGNAL(clicked(bool)), this, SLOT(clickWarning()));
    it_4++;
    ctrl->myElevator[i].floorButton[21] = *it_4;
    connect(*it_4, SIGNAL(clicked(bool)), this, SLOT(clickPhone()));
    it_4++;
  }
  for (int i = 0; i < FLOOR; i++) {
    for (int j = 0; j < 2; j++) {
      floorButton[i][j] = *it_4;
      connect(*it_4, SIGNAL(clicked(bool)), this, SLOT(clickDirection()));
      it_4++;
    }
  }
  floorButton[19][1]->setEnabled(false);
  floorButton[0][0]->setEnabled(false);
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    ctrl->myElevator[i].floorButton[22] = *it_4;
    connect(*it_4, SIGNAL(clicked(bool)), this, SLOT(clickOpen()));
    it_4++;
    ctrl->myElevator[i].floorButton[23] = *it_4;
    connect(*it_4, SIGNAL(clicked(bool)), this, SLOT(clickClose()));
    it_4++;
  }
}

void MainWindow::setAnimation() {
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    elevAnimation[2 * i]->setDuration(1000);
    elevAnimation[2 * i]->setStartValue(QRect(37 + 180 * i, 45, 42, 127));
    elevAnimation[2 * i]->setEndValue(QRect(37 + 180 * i, 45, 0, 127));
    elevAnimation[2 * i + 1]->setDuration(1000);
    elevAnimation[2 * i + 1]->setStartValue(QRect(78 + 180 * i, 45, 42, 127));
    elevAnimation[2 * i + 1]->setEndValue(QRect(120 + 180 * i, 45, 0, 127));
  }
  for (int i = 0; i < ELEVATOR_NUM; i++) {
    preAnimation[i]->setDuration(1500);
    preAnimation[i]->setStartValue(QRect(0 + 180 * i, 90, 51, 111));
    preAnimation[i]->setEndValue(QRect(50 + 180 * i, 60, 51, 111));
  }
}

void MainWindow::setConnection() {
  connect(this, SIGNAL(warning(int)), ctrl, SLOT(warningState(int)));
  connect(this, SIGNAL(phone(int)), ctrl, SLOT(phoneState(int)));
  connect(this, SIGNAL(controlDoor(int, int)), ctrl,
          SLOT(controlDoorState(int, int)));
  connect(this, SIGNAL(inMessage(int, int)), ctrl, SLOT(distribute(int, int)));
  connect(this, SIGNAL(outMessage(int, int)), ctrl,
          SLOT(selectBestElev(int, int)));
  connect(ctrl, SIGNAL(open(int)), this, SLOT(doorOpenAni(int)));
  connect(ctrl, SIGNAL(close(int)), this, SLOT(doorCloseAni(int)));
  connect(ctrl, SIGNAL(change(int, int)), this, SLOT(changeButton(int, int)));
  connect(ctrl, SIGNAL(recover(int, int)), this, SLOT(recoverButton(int, int)));
}

void MainWindow::getMessage(QString objectName, int *curMessage) {
  QStringList sl = objectName.split("_");
  QByteArray byte = sl.at(0).toUtf8();
  curMessage[0] = int(byte.at(0)) - 96;
  curMessage[1] = sl.at(1).toInt();
}

void MainWindow::openDoor(int elevatorID) {
  elevAnimation[2 * elevatorID]->setDirection(
      QAbstractAnimation::Forward); //正向设定动画
  elevAnimation[2 * elevatorID + 1]->setDirection(QAbstractAnimation::Forward);
  elevAnimation[2 * elevatorID]->start();
  elevAnimation[2 * elevatorID + 1]->start();
}

void MainWindow::closeDoor(int elevatorID) {
  elevAnimation[2 * elevatorID]->setDirection(
      QAbstractAnimation::Backward); //反向设定动画
  elevAnimation[2 * elevatorID + 1]->setDirection(QAbstractAnimation::Backward);
  elevAnimation[2 * elevatorID]->start();
  elevAnimation[2 * elevatorID + 1]->start();
}

void MainWindow::personIn(int elevatorID) {
  person[elevatorID]->setVisible(true);
  setPersonTop(elevatorID);
  preAnimation[elevatorID]->setDirection(QAbstractAnimation::Forward);
  preAnimation[elevatorID]->start();
  QTimer::singleShot(1.5 * 1000, this,
                     [&, elevatorID]() { setDoorTop(elevatorID); });
}

void MainWindow::personOut(int elevatorID) {
  person[elevatorID]->setVisible(true);
  setPersonTop(elevatorID);
  preAnimation[elevatorID]->setDirection(QAbstractAnimation::Backward);
  preAnimation[elevatorID]->start();
  QTimer::singleShot(1 * 1000, this, [&, elevatorID]() {
    person[elevatorID]->setVisible(false);
  });
}

void MainWindow::setDoorTop(int elevatorID) {
  door[2 * elevatorID]->raise();
  door[2 * elevatorID + 1]->raise();
}

void MainWindow::setPersonTop(int elevatorID) { person[elevatorID]->raise(); }

void MainWindow::clickFloor() {
  int curMessage[2];
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  button->setStyleSheet("background-color:red;font: 9pt 'Stencil';");
  getMessage(button->objectName(), curMessage);
  emit inMessage(curMessage[0], curMessage[1]);
}

void MainWindow::clickDirection() {
  int curMessage[2];
  int direction;
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  button->setStyleSheet("background-color:red;font: 9pt 'Stencil';");
  getMessage(button->objectName(), curMessage);
  if (curMessage[0] == UPDIRECTION) {
    direction = UPSTATE;
  } else {
    direction = DOWNSTATE;
  }
  floorButton[curMessage[1] - 1][direction]->setEnabled(false);
  emit outMessage(direction, curMessage[1]);
}

void MainWindow::clickWarning() {
  int curMessage[2];
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  //  button->setStyleSheet(
  //      "background-color:red;image: url(:/new/image/image/alarm bell.png);");
  getMessage(button->objectName(), curMessage);
  emit warning(curMessage[0]);
}

void MainWindow::clickPhone() {
  int curMessage[2];
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  getMessage(button->objectName(), curMessage);
  emit phone(curMessage[0]);
}

void MainWindow::clickOpen() {
  int curMessage[2];
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  button->setStyleSheet("font: 9pt '黑体';");
  getMessage(button->objectName(), curMessage);
  emit controlDoor(curMessage[0], curMessage[1]);
}

void MainWindow::clickClose() {
  int curMessage[2];
  QPushButton *button = qobject_cast<QPushButton *>(sender());
  button->setStyleSheet("font: 9pt '黑体';");
  getMessage(button->objectName(), curMessage);
  emit controlDoor(curMessage[0], curMessage[1]);
}

void MainWindow::doorOpenAni(int elevatorID) {
  openDoor(elevatorID);
  personIn(elevatorID);
  player->play();
  QTimer::singleShot(1 * 1000, this,
                     [&, elevatorID]() { closeDoor(elevatorID); });
}

void MainWindow::doorCloseAni(int elevatorID) {
  openDoor(elevatorID);
  personOut(elevatorID);
  player->play();
  QTimer::singleShot(1 * 1000, this,
                     [&, elevatorID]() { closeDoor(elevatorID); });
}

void MainWindow::changeButton(int elevatorID, int state) {
  if (state == FREESTATE) {
    upDownButton[elevatorID]->setStyleSheet(
        "border-image: url(:/new/image/image/nothing button.png);");
  } else if (state == UPSTATE) {
    upDownButton[elevatorID]->setStyleSheet(
        "border-image: url(:/new/image/image/up button.png);");
  } else if (state == DOWNSTATE) {
    upDownButton[elevatorID]->setStyleSheet(
        "border-image: url(:/new/image/image/down button.png);");
  }
}

void MainWindow::recoverButton(int direction, int floor) {
  floorButton[floor - 1][direction]->setEnabled(true);
  floorButton[floor - 1][direction]->setStyleSheet("border-radius:15px;"
                                                   "border:2px solid gray;"
                                                   "color:black;"
                                                   "font: 9pt 'Stencil';");
}

MainWindow::~MainWindow() { delete ui; }
