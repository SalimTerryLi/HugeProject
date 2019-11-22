#include "GY31.h"
#include <Wire.h>

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define OUT 2

#define LGRED 9
#define LGYELLOW 10
#define LGBLUE 11

#define SLIDEPWR 13

unsigned long RedRef = 30, GreenRef = 25, BlueRef = 30; // 校准颜色传感器的参数
unsigned long RedStep = 40, GreenStep = 30, BlueStep = 32; // 校准颜色传感器的参数

unsigned long lastTickMS = 0; // 记录上一次执行loop函数的时间



void setup() {
  Serial.begin(9600); // 开启串口
  pinMode(LGRED, INPUT); // 设置颜色传感器
  pinMode(LGYELLOW, INPUT); // 设置颜色传感器
  pinMode(LGBLUE, INPUT); // 设置颜色传感器
  pinMode(SLIDEPWR, OUTPUT); // 设置传送带控制针脚
  digitalWrite(SLIDEPWR, LOW); // 关闭传送带

  configureGY31(S0, S1, S2, S3, OUT); // 设置颜色传感器
  setOutputFreq(Percent20); // 设置颜色传感器

  lastTickMS = millis(); // 初始化时间
}

int itemColor = -1; // 储存颜色信息

bool detectItem() {
  bool haveRed = false, haveGreen = false, haveBlue = false; // 记录是否探测到指定颜色
  unsigned long tmp; // 计数器
  tmp = getRed();
  if (tmp > RedRef + RedStep) { // 红色强度大于判断指标
    haveRed = true;
  }
  tmp = getGreen();
  if (tmp > GreenRef + GreenStep) { // 绿色强度大于判断指标
    haveGreen = true;
  }
  tmp = getBlue();
  if (tmp > BlueRef + BlueStep) { // 蓝色强度大于判断指标
    haveBlue = true;
  }
  itemColor = 1 * haveRed + 2 * haveGreen + 4 * haveBlue; // 返回颜色，1=红，2=绿，4=蓝，3=黄，5=紫，6=青，7=白

  /*Serial.print(haveRed);
    Serial.print(" ");
    Serial.print(haveGreen);
    Serial.print(" ");
    Serial.print(haveBlue);
    Serial.println();*/
  return haveRed || haveGreen || haveBlue; // 如果识别到任意一种颜色，则成功识别物体，返回true
}

bool isItem = false; // 储存是否检测到物体

bool isPassed = false; // 储存物体是否落入框中

void push(int index) { // 告诉另一个单片机应当转动哪一个舵机
  Serial.print(index);
  isPassed = true;
}

int samplecount = 0; // 记录采样次数
int redC = 0, greenC = 0, blueC = 0, yellowC = 0; // 记录各种颜色被识别到的次数
void loop() {

  if (Serial.read() == '!') {
    digitalWrite(SLIDEPWR, LOW); // 如果接收到另一个单片机发出的消息，表明物体已经掉下去了，关闭传送带
  }

  if (millis() - lastTickMS >= 1) { // 控制里面的代码，不要执行的太快

    if (isItem == false) { // 如果当前没有检测到物体
      if (detectItem()) { // 检测物体，如果现在检测到了
        samplecount++; // 计数器+1
        switch (itemColor) { 判断检测到的是什么颜色
          case 1:
            redC++; // 对应的颜色计数器+1
            break;
          case 2:
            greenC++;
            break;
          case 3:
            yellowC++;
            break;
          case 4:
            blueC++;
            break;
        }
      } // 如果已经确定检测到了物体，那么以上代码就不执行了
      
      if (samplecount >= 3) { // 条件：检测物体的计数器要大于3
        isItem = true; // 这时候确定真的有物体
        digitalWrite(SLIDEPWR, HIGH); // 打开传送带
        if (yellowC != 0) { //因为黄色是由两种颜色合成的，因此不好检测，一旦检测到一次，就认为这个物体是黄色的
          itemColor = 3;
        }
        samplecount = 0; // 成功检测到物体，清空所有计数器
        redC = 0;
        greenC = 0;
        blueC = 0;
        yellowC = 0;

      }
    } // 如果不满足条件，则以上代码不执行

    if (isItem == true) { // 如果确定之前检测到物体
      if (digitalRead(LGRED) == HIGH) { // 物体运行到红色框旁边
        if (itemColor == 1) { // 这个物体是红色的
          push(0); // 推他！
        }
      }
      if (digitalRead(LGYELLOW) == HIGH) {
        if (itemColor == 3) {
          push(1);
        }
      }
      if (digitalRead(LGBLUE) == HIGH) { // 如果运行到蓝色旁边，这时候有俩情况
        if (itemColor == 4) { // 如果物体是蓝色
          push(2); // 推他！
        } else if (itemColor == 2) { // 如果物体是绿色
          push(3); // 假装推他，实际上没有推，只是告诉另一个单片机，这个物体过去了
        }
        isPassed = true; // 物体已经通过了所有的光电门
      }
    }

    if ((isItem == true) && (isPassed == true)) { // 之前检测到了物体，而且物体已经被推下去或者绿色的物体通过了最后一个光电门
      isItem = false; // 清空这两个标志，准备迎接下一个物体
      isPassed = false;
    }

    // sys loop ctrl.
    lastTickMS = millis(); // 更新一下上一次执行到这里时候的事件，以此控制执行速率
  }
}
