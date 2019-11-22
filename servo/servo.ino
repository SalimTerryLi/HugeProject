/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  Servo test - this will drive 8 servos, one after the other on the
  first 8 pins of the PCA9685

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815
  
  These drivers use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); //创建PCA9685舵机驱动板


#define SERVOMIN  150 // 定义最小脉冲宽度
#define SERVOMAX  600 // 最大脉冲宽度

void setup() {
  Serial.begin(9600); // 设置串口波特率9600

  pwm.begin(); // 启动PCA9685
  
  pwm.setPWMFreq(60); // 设置PWM频率为60HZ

  delay(100);

  pwm.setPWM(0, 0, 390); // 复位三个舵机位置
  pwm.setPWM(1, 0, 390);
  pwm.setPWM(2, 0, 390);
}

void loop() {
  int tmp=Serial.read(); // 不停尝试读取串口
  if(tmp!=-1){ // 如果有读到内容
    delay(1200);
    pwm.setPWM(tmp-'0', 0, 315); // 将读到的舵机旋转至推下的角度，把东西丢进去
    delay(500);
    pwm.setPWM(tmp-'0', 0, 390); // 复位该舵机
    if(tmp-'0'==3){ // 如果读到的舵机为3，即不需要舵机运动，只依靠传送带将物品投入最后一个框框，这时候就需要延时一下，等待传送带把东西丢进框里。
      delay(1000);
    }
    Serial.print('!'); // 告诉另一个单片机工作完成了
  }
}
