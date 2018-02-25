
void buttonAction(int a, int b, int c, int d);
int computeMap( int value, int optionXorY);
void writePair(char * valueX, char * valueY, int x, int y);
void carefulPrinter(char * str);

char *ButtonStrBuffer;

void main() {
  int buttonA;
  int buttonB;
  int buttonC;
  int buttonD;

  char textValueX[200];
  char textValueY[200];

  unsigned long valueX;
  unsigned long valueY;

  //UART Initialization
  UART1_Init(56000);
  // Wait for UART module to stabilize
  Delay_ms(100);
  UART1_Write_Text("Start\r\n");

  //Set PORTD and PORTC as output
  // These will display the postion of the joystick
  GPIO_Digital_Output(&GPIOD_ODR, _GPIO_PINMASK_LOW);
  GPIO_Digital_Output(&GPIOC_ODR, _GPIO_PINMASK_LOW);

  //Setup four buttons for input
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

    //Ensures that joystick index values are not over 3
    if ( (computeMap(valueX , 0) < 3) && ( computeMap(valueY , 1) < 3 ) ) {
      //Converts index to string
      IntToStr(computeMap(valueX , 0), textValueX);
      IntToStr( computeMap(valueY , 1), textValueY);
      
      //Prints to the UART
      writePair(textValueX, textValueY, (computeMap(valueX , 0) < 3),  computeMap(valueY , 1));
      Delay_ms(100);
      
      //Reads from buttons
      buttonAction(buttonA, buttonB, buttonC, buttonD);
    }
  }
}

//Monitors when buttons are pressed
void buttonAction(int pressed_A, int pressed_B, int pressed_C, int pressed_D) {
  //Flags to monitor when a button has been already pressed
  int shownA , shownB, shownC, shownD;
  
  if (Button(&GPIOD_IDR, 8 , 1, 1)) {
    pressed_A = 1;
    pressed_B = 0;
    pressed_C = 0;
    pressed_D = 0;
  }
  if (Button(&GPIOD_IDR, 9, 1, 1)) {
    pressed_A = 0;
    pressed_B = 1;
    pressed_C = 0;
    pressed_D = 0;
  }
  if (Button(&GPIOD_IDR, 10, 1, 1)) {
    pressed_A = 0;
    pressed_B = 0;
    pressed_C = 1;
    pressed_D = 0;
  }
  if (Button(&GPIOD_IDR, 11, 1, 1)) {
    pressed_A = 0;
    pressed_B = 0;
    pressed_C = 0;
    pressed_D = 1;
  }

  // Initial values
  shownA = 0;
  shownB = 0;
  shownC = 0;
  shownD = 0;

  if ( pressed_A == 1 && shownA == 0) {
    shownA = 1;
    shownB = 0;
    shownC = 0;
    shownD = 0;
    carefulPrinter("0000\r\n");
    Delay_ms(100);
  }
  if ( pressed_B == 1 && shownB == 0) {
    shownA = 0;
    shownB = 1;
    shownC = 0;
    shownD = 0;
    carefulPrinter("0001\r\n");
    Delay_ms(100);
  }
  if ( pressed_C == 1 &&  shownC == 0) {
    shownA = 0;
    shownB = 0;
    shownC = 1;
    shownD = 0;
    carefulPrinter("0010\r\n");
    Delay_ms(100);
  }
  if ( pressed_D == 1 && shownD == 0) {
    shownA = 0;
    shownB = 0;
    shownC = 0;
    shownD = 1;
    carefulPrinter("0011\r\n");
    Delay_ms(100);
  }
}

//Sets joystick index value on LEDS
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

//Prints values non-sequently (Button values)
void carefulPrinter(char* str) {
    if (strcmp( ButtonStrBuffer , str) != 0 ) {
      UART1_Write_Text(str);
    } else {
      // UART1_Write_Text("----");
    }
    //overriding the pointer
    ButtonStrBuffer = str;
}

//Variables for writePair function
int holdX;
int holdY;
//Prints values non-sequently (Order pair value)
void writePair(char* xValue, char * yValue, int x, int y) {
  if(x != holdX || y != holdY){
    char bufferOrderPair[] = "(";
    strcat(bufferOrderPair, xValue);
    strcat(bufferOrderPair, ",");
    strcat(bufferOrderPair, yValue);
    strcat(bufferOrderPair, " )\r\n");
    UART1_Write_Text(bufferOrderPair);
  }else{
   }
   holdX = x;
   holdY = y ;
}
