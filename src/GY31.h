enum OutputFreq{
  PowerDown,
  Percent2,
  Percent20,
  Full
};

void configureGY31(int S0,int S1,int S2,int S3,int OUT);
unsigned long getRed();
unsigned long getGreen();
unsigned long getBlue();
unsigned long getLight();
void setOutputFreq(OutputFreq freq);
