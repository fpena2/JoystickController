unsigned long adc_result = 0;
register unsigned int adc_data = 0;

void setupADC_int();

void ADC_ISR() iv IVT_INT_ADC1_2 ics ICS_AUTO {
  adc_data = (ADC1_DR & 0x0FFF);
}

char *ButtonStrBuffer;

void buttonAction(int a, int b, int c, int d);
int computeMap( int value, int optionXorY);
void writePair(char * valueX, char * valueY, int x, int y);
void carefulPrinter(char * str);

void main() {
  int buttonA;
  int buttonB;
  int buttonC;
  int buttonD;

  char textValueX[200];
  char textValueY[200];
  char debTextValueX[200];
  char debTextValueY[200];

  int debX;
  int debY;

  unsigned long valueX;
  unsigned long valueY;

  //UART CODE
  UART1_Init(56000);
  // Wait for UART module to stabilize
  Delay_ms(100);
  UART1_Write_Text("Start\r\n");

  //Set PORTD as output
  GPIO_Digital_Output(&GPIOD_ODR, _GPIO_PINMASK_LOW);
  GPIO_Digital_Output(&GPIOC_ODR, _GPIO_PINMASK_LOW);

  //Setup four buttons
  GPIO_Digital_Input(&GPIOD_IDR, _GPIO_PINMASK_0);

  // Choose ADC channel
  ADC_Set_Input_Channel(_ADC_CHANNEL_3 | _ADC_CHANNEL_4 );

  //Configures ADC module to work with default settings
  ADC1_Init();

  GPIOD_ODR = 0; //clear leds

  while (1) {
    //Read values for UART
    valueX = ADC1_Get_Sample(3);
    valueY = ADC1_Get_Sample(4);

    if ( (computeMap(valueX , 0) < 3) && ( computeMap(valueY , 1) < 3 ) ) {

      IntToStr(computeMap(valueX , 0), textValueX);
      IntToStr( computeMap(valueY , 1), textValueY);

      writePair(textValueX, textValueY, (computeMap(valueX , 0) < 3),  computeMap(valueY , 1));
      Delay_ms(100);
      //Reads from buttons
      buttonAction(buttonA, buttonB, buttonC, buttonD);
    }
  }
}

void buttonAction(int Da, int Db, int Dc, int Dd) {
  int shownA , shownB, shownC, shownD;
  if (Button(&GPIOD_IDR, 8 , 1, 1)) {
    Da = 1;
    Db = 0;
    Dc = 0;
    Dd = 0;
  }
  if (Button(&GPIOD_IDR, 9, 1, 1)) {
    Da = 0;
    Db = 1;
    Dc = 0;
    Dd = 0;
  }
  if (Button(&GPIOD_IDR, 10, 1, 1)) {
    Da = 0;
    Db = 0;
    Dc = 1;
    Dd = 0;
  }
  if (Button(&GPIOD_IDR, 11, 1, 1)) {
    Da = 0;
    Db = 0;
    Dc = 0;
    Dd = 1;
  }

  // Initial values
  shownA = 0;
  shownB = 0;
  shownC = 0;
  shownD = 0;

  if ( Da == 1 && shownA == 0) {
    shownA = 1;
    shownB = 0;
    shownC = 0;
    shownD = 0;
    carefulPrinter("0000\r\n");
    Delay_ms(100);
  }
  if ( Db == 1 && shownB == 0) {
    shownA = 0;
    shownB = 1;
    shownC = 0;
    shownD = 0;
    carefulPrinter("0001\r\n");
    Delay_ms(100);
  }
  if ( Dc == 1 &&  shownC == 0) {
    shownA = 0;
    shownB = 0;
    shownC = 1;
    shownD = 0;
    carefulPrinter("0010\r\n");
    Delay_ms(100);
  }
  if ( Dd == 1 && shownD == 0) {
    shownA = 0;
    shownB = 0;
    shownC = 0;
    shownD = 1;
    carefulPrinter("0011\r\n");
    Delay_ms(100);
  }
}

int computeMap( int value, int optionXorY) {
  if ( value > 3500 ) {
    if (optionXorY == 0) GPIOD_ODR = 16;
    if (optionXorY == 1) GPIOC_ODR = 1;
    return -2;
  }
  if (value < 3500 & value > 2050) {
    if (optionXorY == 0) GPIOD_ODR = 32;
    if (optionXorY == 1) GPIOC_ODR = 2;
    return -1;
  }
  if (value < 2050 & value > 1800) {
    if (optionXorY == 0) GPIOD_ODR = 0;
    if (optionXorY == 1) GPIOC_ODR = 0;
    return 0;
  }
  if (value < 1800 & value > 30 ) {
    if (optionXorY == 0) GPIOD_ODR = 64;
    if (optionXorY == 1) GPIOC_ODR= 4;
    return 1;
  }
  if (value < 30) {
    if (optionXorY == 0) GPIOD_ODR = 128;
    if (optionXorY == 1) GPIOC_ODR = 8;
    return 2;
  }
}


void carefulPrinter(char* str) {
//Button output
    if (strcmp( ButtonStrBuffer , str) != 0 ) {
      UART1_Write_Text(str);
    } else {
      // UART1_Write_Text("----");
    }
    //overriding the pointer
    ButtonStrBuffer = str;
}

char *bufferXValue;
char *bufferYValue;
int holdX;
int holdY;


void writePair(char* xValue, char * yValue, int x, int y) {
  if(x != holdX || y != holdY){
    char bufferOrderPair[] = "(";
    strcat(bufferOrderPair, xValue);
    strcat(bufferOrderPair, ",");
    strcat(bufferOrderPair, yValue);
    strcat(bufferOrderPair, " )\r\n");
    UART1_Write_Text(bufferOrderPair);
  }else{
   //UART1_Write_Text("here2");
   }
   holdX = x;
   holdY = y ;
}
