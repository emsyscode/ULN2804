// Ofcourse the code can be more short and clean
// But this need be basic and easy to catch.
// I let with you thats task!!!

#define data 8
#define clock 9
#define mr 10

uint8_t arrayGrid[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uint8_t numbers[10] =  {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xE6};

#define AdjustPins    PIND // Use all Port D, Is possible use only some pins! Look inside of function adjustHMS() to redefine it!

/*Global Variables Declarations*/
unsigned char hours = 0;
unsigned char minutes = 0;
unsigned char minute = 0;
unsigned char secs=0;
unsigned char seconds=0;
unsigned char milisec = 0;

uint8_t digitSu = 0x00;
uint8_t digitSd = 0x00;
uint8_t digitMu = 0x00;
uint8_t digitMd = 0x00;
uint8_t digitHu = 0x00;
uint8_t digitHd = 0x00;

uint8_t numSecsU;
uint8_t numSecsD;
uint8_t numMinuU;
uint8_t numMinuD;
uint8_t numHourU;
uint8_t numHourD;

unsigned char digit=0;
unsigned char grid=0;

void setup(){
  pinMode (3, INPUT_PULLUP);
  pinMode (4, INPUT_PULLUP);
  pinMode (5, INPUT_PULLUP);

  pinMode(clock, OUTPUT); // make the clock pin an output
  pinMode(data , OUTPUT); // make the data pin an output
  pinMode(mr, OUTPUT);  // make the mr pin an output
  digitalWrite(mr, HIGH);

   pinMode(LED_BUILTIN, OUTPUT);
      Serial.begin(115200);
      seconds = 0x00;
      minutes =0x00;
      hours = 0x00;

      /*CS12  CS11 CS10 DESCRIPTION
      0        0     0  Timer/Counter1 Disabled 
      0        0     1  No Prescaling
      0        1     0  Clock / 8
      0        1     1  Clock / 64
      1        0     0  Clock / 256
      1        0     1  Clock / 1024
      1        1     0  External clock source on T1 pin, Clock on Falling edge
      1        1     1  External clock source on T1 pin, Clock on rising edge
      */
      // initialize timer1 
      cli();           // disable all interrupts
      //initialize timer1 
      //noInterrupts();    // disable all interrupts, same as CLI();
      TCCR1A = 0;
      TCCR1B = 0;// This initialisations is very important, to have sure the trigger take place!!!
      
      TCNT1  = 0;
      
      // Use 62499 to generate a cycle of 1 sex 2 X 0.5 Secs (16MHz / (2*256*(1+62449) = 0.5
      //Comment next line and uncomment the other line follow to get a count of seconds more fast to effect of test!
      OCR1A = 62498;            // compare match register 16MHz/256/2Hz
      //OCR1A = 1200; // only to use in test, increment seconds more fast!
      TCCR1B |= (1 << WGM12);   // CTC mode
      TCCR1B |= ((1 << CS12) | (0 << CS11) | (0 << CS10));    // 256 prescaler 
      TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

    // Note: this counts is done to a Arduino 1 with Atmega 328... Is possible you need adjust
    // a little the value 62499 upper or lower if the clock have a delay or advance on hours.
      
    //  a=0x33;
    //  b=0x01;

    CLKPR=(0x80);
    //Set PORT
    DDRD = 0xFF;  // IMPORTANT: from pin 0 to 7 is port D, from pin 8 to 13 is port B
    PORTD=0x00;
    DDRB =0xFF;
    PORTB =0xFF;  //This Port have the pin "mr", is necessary check it!!!!
    
    //I redefine here the pin Mode to avoid collisions of global set of the PORTD & PORTB above!!!
    pinMode (3, INPUT_PULLUP);
    pinMode (4, INPUT_PULLUP);
    pinMode (5, INPUT_PULLUP);
    
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);

    //interrupts();             // enable all interrupts, is same as sei();
    sei();
}
void showDigits(uint8_t grid, uint8_t digit){
  uint8_t byteL = 0x00;
  uint8_t byteH = 0x00;
  //Offcourse you can assign bytes direct, I only let it to allow test.
  //.........12345678 // Note we skip Grid "0", from pre-defined symbols.
  byteL = (0b00000000 | grid);
  //The following two commented lines are only to make the code easier to understand!
  //.........abcdefgh.........................//
  byteH = (0b00000000 | digit);
  byteL = ~byteL;
  byteH = ~byteH;
  //The constituition of function of Arduino Library: void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
  shiftOut(data, clock, MSBFIRST, byteH); // 
  shiftOut(data, clock, MSBFIRST, byteL); //  
  delay(3);//The clock is showeed at dynamic mode! One Grid, one Digit each time! If you put for exemple 5, you will see the FLICK.            
}
void send_update_clock(void){
  if (secs >=60){
    secs =0;
    minutes++;
  }
  if (minutes >=60){
    minutes =0;
    hours++;
  }
  if (hours >=24){
    hours =0;
  }
    //*************************************************************
    digitSu = (secs%10);
    numSecsU=numbers[digitSu];
    digitSd = (secs/10);
    numSecsD=numbers[digitSd];
    //*************************************************************
    digitMu = (minutes%10);
    numMinuU=numbers[digitMu];
    digitMd = (minutes/10);
    numMinuD=numbers[digitMd];
    //**************************************************************
    digitHu = (hours%10);
    numHourU=numbers[digitHu];
    digitHd = (hours/10);
    numHourD=numbers[digitHd];
    //**************************************************************
    SegTo8Bits(); // This is to send the total of digits to VFD
}
void SegTo8Bits(){
  //Note is here I define the number of grid will be used to show the timer!
          //Secondes Block
          showDigits(0x80, numSecsU);
          showDigits(0x40, numSecsD);
          //Minuts Block:
          showDigits(0x10, numMinuU);
          showDigits(0x08, numMinuD); 
          //Hours Block:
          showDigits(0x02, numHourU); 
          showDigits(0x01, numHourD);
          // Only to debug, keep it commented to normal work. 
          //  Serial.print(numSecsD, DEC); Serial.print(", ");   Serial.println(numSecsU, DEC); 
          //  Serial.print(numMinuD, DEC); Serial.print(", ");   Serial.println(numMinuU, DEC); 
          //  Serial.print(numHourD, DEC); Serial.print(", ");   Serial.println(numHourU, DEC); 
          //  Serial.println(".....");
}
void adjustHMS(){
  //This function implement buttons to set the Clock, case the panel you are
  //using don't have buttons!
  //Case the position of buttons belongs to panel are with different positions
  //bit inside of the byte you can modify it on the function readButtons();
  // Important is necessary put a pull-up resistor to the VCC(+5VDC) to this pins (3, 4, 5)
  //pinMode(3, INPUT_PULLUP); //This line must be placed inside of initial SETUP
  //pinMode(4, INPUT_PULLUP); //This line must be placed inside of initial SETUP
  //pinMode(5, INPUT_PULLUP); //This line must be placed inside of initial SETUP
  // if dont want adjust of the time comment the call of function on the loop
  /* Reset Seconds to 00 Pin number 3 Switch to GND*/
    if((AdjustPins & 0x08) == 0 ){  // Pay attention to the weigth of bit, and position at word of 4 bit: 1 Byte = 0x84218421
      delay(100);
      secs=00;
    }
    
    /* Set Minutes when SegCntrl Pin 4 Switch is Pressed*/
    if((AdjustPins & 0x10) == 0 ){  // Pay attention to the weigth of bit, and position at word of 4 bit: 1 Byte = 0x84218421
      delay(100);
      if(minutes < 59)
      minutes++;
      else
      minutes = 0;
    }
    /* Set Hours when SegCntrl Pin 5 Switch is Pressed*/
    if((AdjustPins & 0x20) == 0 ){  // Pay attention to the weigth of bit, and position at word of 4 bit: 1 Byte = 0x84218421
      delay(100);
      if(hours < 23)
      hours++;
      else
      hours = 0;
    }
}
void tst0to9(){
   uint8_t numberPlace = 0x00;
  for(uint8_t i = 0x01; i > 0x00; i=i << 1){ //This cycle FOR move the GRID from left to right, 1 set each time!
  numberPlace=0x00;  //Used to go over position of number's in array: number.
    for(uint8_t s = 0x01; s < 0x0A; s++){
        //shiftOut(data, clock, MSBFIRST, 1 << i); // bit shift a logic high (1) value by i
        shiftOut(data, clock, MSBFIRST, ~(numbers[s])); // 
        //shiftOut(data, clock, MSBFIRST, ~0xB6);
        shiftOut(data, clock, MSBFIRST, ~i); // 
        numberPlace++;
        delay(250); // delay 
      }
    }
}
void tstNumberByNumber(){
  uint8_t tmp = 0x00;
  for(uint8_t r = 0x00; r < 10; r++){
    shiftOut(data, clock, MSBFIRST, ~(numbers[r])); // 
        //shiftOut(data, clock, MSBFIRST, ~0xB6);
        shiftOut(data, clock, MSBFIRST, ~0xFF); // 
        delay(500);
  }
}
void tstGridByGrid(){
  uint8_t tmp = 0x00;
  for(uint8_t d = 0x01; d > 0; d=d*2){
    //Grid 0x80 is first on the right after the grid of pre-defined symbols.
    showDigits(d, (numbers[tmp]));
    tmp ++;
    delay(500);
  }
}
void loop() {
  tst0to9();
  delay(1000);
  tstNumberByNumber();
  delay(1000),
  tstGridByGrid();
  delay(1000);
  //You can comment the lines above to avoid the running test's!
  while(1){
      send_update_clock();
      adjustHMS();
    } 
}
ISR(TIMER1_COMPA_vect)   {  //This is the interrupt request
// https://avr-guide.github.io/timers-on-the-atmega328/
      secs++;
} 

