enum OutputFreq{
  PowerDown,
  Percent2,
  Percent20,
  Full
};

void configureGY31(int S0,int S1,int S2,int S3,int OUT);
unsigned long getRed(); // 返回读取到的红色的强度
unsigned long getGreen(); // 返回读取到的绿色的强度
unsigned long getBlue(); // 返回读取到的蓝色的强度
unsigned long getLight(); // 返回读取到的总的强度
void setOutputFreq(OutputFreq freq);
