#define STOPWATCH 1
#define WAITING 2


int dataPin = 11;
int clockPin = 12;
int latchPin = 8;
const int clearPin = 7;

int buttonLEDPin = 10;
int buttonPin = 9;
int ledPin = 13;

 
//holders for infromation you're going to pass to shifting function
byte dataRED;
byte dataArrayRED[10];
 
byte off = 0x00;
byte decimalpoint = 0x80;

int i = 0;
unsigned long previousMillis = 0;
long interval = 1000;

unsigned long ledPreviousMillis = 0;
long ledInterval = 5;


// Big Red Button Stuff
int buttonState = 0;         // variable for reading the pushbutton status
int lightState = 0;
int oldState = 0;
int LEDBrightness = 0;
boolean going_up = true;

int state;
 
void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(clearPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  pinMode(buttonLEDPin, OUTPUT);
  pinMode(buttonPin, INPUT);  
  pinMode(ledPin, OUTPUT);
  
  digitalWrite( clearPin, HIGH);

  dataArrayRED[0] = B00111111;
  dataArrayRED[1] = B00000110;
  dataArrayRED[2] = B01011011;
  dataArrayRED[3] = B01001111;
  dataArrayRED[4] = B01100110;
  dataArrayRED[5] = B01101101;
  dataArrayRED[6] = B01111101;
  dataArrayRED[7] = B00000111;
  dataArrayRED[8] = B01111111;
  dataArrayRED[9] = B01100111;
  
  state = WAITING;

}
 
void loop() {

  checkButtonState();
  switch(state)
  {
    case STOPWATCH:
      stopwatch();
      break;
    case WAITING:
      waiting();
      break;
  }

}

int checkButtonState()
{
  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH && oldState == LOW){
     lightState = 1 - lightState; 
     delay(25);
  }
  oldState = buttonState;
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (lightState  == 1) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH);
    state = STOPWATCH;
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
    state = WAITING;
  }
}

void waiting()
{
  turnOffDigits();
  analogWrite(buttonLEDPin,LEDBrightness);
  unsigned long currentMillis = millis();
 
  if(currentMillis - ledPreviousMillis > ledInterval) {
    ledPreviousMillis = currentMillis;   
    if(LEDBrightness == 255)
    {
      going_up = false; 
    } else if(LEDBrightness == 0)
    {
      going_up = true;  
    }
      
    if(going_up){
      LEDBrightness++;  
    } else {
      LEDBrightness--;
    }
  }
  
  
}

void turnOffDigits()
{
    digitalWrite(latchPin, 0);   
    shiftOut(dataPin, clockPin, off);
    digitalWrite(latchPin, 1); 
    i = 0;
}

void stopwatch()
{
  int count = 10;
  int val = 10; //this is for how many digits
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    if(i == count )
    {
     i = 0; 
    }
    previousMillis = currentMillis;   
    digitalWrite(latchPin, 0);   
    shiftOut(dataPin, clockPin, dataArrayRED[i]);
    digitalWrite(latchPin, 1);
    i++;
  }
}
 
 
// the heart of the program
void shiftOut(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first,
  //on the rising edge of the clock,
  //clock idles low
 
  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
 
  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
 
  //for each bit in the byte myDataOut…
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights.
 
  for (i=7; i>=0; i--)  
  {
    digitalWrite(myClockPin, 0);
 
    //if the value passed to myDataOut and a bitmask result
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000
    // and proceeds to set pinState to 1.
    if( myDataOut & (1<<i) ) 
    {
	pinState= 1;
    }
    else 
    {
	pinState= 0;
    }
 
    //Sets the pin to HIGH or LOW depending on pinState
 
    digitalWrite(myDataPin, pinState);
    //register shifts bits on upstroke of clock pin
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }
 
  //stop shifting
  digitalWrite(myClockPin, 0);
}
