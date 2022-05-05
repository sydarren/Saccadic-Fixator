#include <ShiftOutX.h>
#include <ShiftPinNo.h>

 // define statements
#define N_Buttons 32       // number of buttons/lights 

// Timer define statement
#define Timer_u 15 // unit
#define Timer_t 14 // tens

// Score define statement
#define Score_u 16
#define Score_t 17


// Input Scan Delay
#define Scan_DelayMs 300

shiftOutX regOne(8, 11, 12, MSBFIRST, 4);

// 7 SEGMENT Decoder Array
int num_array[11][7] = {  { 1,1,1,1,1,1,0 },    // 0
                          { 0,1,1,0,0,0,0 },    // 1
                          { 1,1,0,1,1,0,1 },    // 2
                          { 1,1,1,1,0,0,1 },    // 3
                          { 0,1,1,0,0,1,1 },    // 4
                          { 1,0,1,1,0,1,1 },    // 5
                          { 1,0,1,1,1,1,1 },    // 6
                          { 1,1,1,0,0,0,0 },    // 7
                          { 1,1,1,1,1,1,1 },    // 8
                          { 1,1,1,0,0,1,1 },    // 9 
                          { 0,0,0,0,0,0,0 } };   // BLANK (10) input 10 to get blank
// Pin Assignment Arrays
int BUTTON_ID[] = {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 
                   38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};

//Pin connected to ST_CP of 74HC595 GREEN
int latchPin = 8;
//Pin connected to SH_CP of 74HC595 YELLOW
int clockPin = 12;
////Pin connected to DS of 74HC595 BLUE
int dataPin = 11;
// 1 to 32
uint64_t  LED_arr[32] = {shPin1, shPin2, shPin3, shPin4, shPin5, shPin6, shPin7, shPin8, shPin9, shPin10, 
                            shPin11, shPin12, shPin13, shPin14, shPin15, shPin16,shPin17, shPin18, shPin19, shPin20,
                            shPin21, shPin22, shPin23, shPin24, shPin25, shPin26, shPin27, shPin28,shPin29, shPin30,
                            shPin31, shPin32}; 

//Debouncing contants 
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
uint64_t lastDebounceTime = 0;  // the last time the output pin was toggled
uint64_t debounceDelay = 40;    // the debounce time; increase if the output flickers

//LED/Button variables
int previousID;


// Timer Variables
int timer = 30;
uint64_t time;
uint64_t time1;
uint64_t prev_time=0;
uint64_t prev_time1=0;

// etc.
int score = 0;
int mistakes =0;
int random_arr[N_Buttons]; //   contains the sequence to be used in the game
int counter =0;
boolean INIT_FLAG =1; // flag to check if first state is to be executed
boolean START_FLAG =0;
boolean TIMEUP_FLAG =0;


/******** FUNCTION HEADERS**********/
void Num_Write(int);
void UpdateLED(int);
void Init();
void Tick();
void Display_Score();
void Display_Timer();
void Display_Mistakes();
void SS_LOW();
int checkMyButtons();
void ScanMistakes(int);
boolean Debounce(int);
void ENDFUNC();


void setup() 
{ 
  // Start Button Light
  pinMode(13, OUTPUT);
  
  // 7 Segment
  pinMode(0, OUTPUT);   
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  //Timer (7-SEG)
  pinMode(Timer_u, OUTPUT);
  pinMode(Timer_t, OUTPUT);

  // Score (7-SEG)
  pinMode(Score_u, OUTPUT);
  pinMode(Score_t, OUTPUT);

  // SHIFT REG
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // Button input *sample*
  int cnt;
  for(cnt =0; cnt<N_Buttons; cnt++)
    pinMode(BUTTON_ID[cnt], INPUT_PULLUP);

  pinMode(9, INPUT_PULLUP); // start button
  digitalWrite(13, LOW); // SET 
  
}

void loop() 
{
  // check if the start button is presed
  if (Debounce(9)&& START_FLAG==0) 
  {
    START_FLAG =1;
    // turn off LED start and settings
    regOne.pinOff(LED_arr[18]);
    regOne.pinOff(LED_arr[19]);
    digitalWrite(13, HIGH);
  }

  // IF the start button was pressed
  if (START_FLAG == HIGH)
  {
      int8_t pressedButton = checkMyButtons();
    
    // if it is the initial statement
    if(INIT_FLAG)
      Init();
  
    if(pressedButton == random_arr[counter])
    {
      counter++;
      mistakes--;
      UpdateLED(random_arr[counter]);
    }
    
    ScanMistakes(pressedButton);
  
    Tick();    
    Display_Timer();
    //Display_Mistakes();
    Display_Score();
  }
  // ELSE Settings
  else if(START_FLAG==0)
  {
    int8_t pButton = checkMyButtons();
    // LED ON 
    regOne.pinOn(LED_arr[18]);
    regOne.pinOn(LED_arr[19]);
    digitalWrite(13, LOW);

    Display_Timer();

    if(pButton == 18)
      timer =30;
    else if (pButton == 19)
      timer =60;
  }

  /* Check if the timer is up then deactivate flags and go to end state
   * 
   */
  if(counter==N_Buttons || TIMEUP_FLAG)
  {
    regOne.allOn();
    delay(1000);
    regOne.allOff();
    
    ENDFUNC();// wont exit until I press the start the start button 
  }
}

/*
 * Function initializes the required stuff for the first state.
 * This function will run only once as the first LED/Button to be pressed
 * The value is then stored at a variable of "previous" LED/Button integer
 * The ID will then be passed to deactivate the previous LED on the main loop
 * new plan "pre-generate" a fixed array then use random to "shuffle" the elements n number of times
*/
void Init()
{
  int temp, i,j;
  mistakes =0;
  counter =0;
  SS_LOW();

   regOne.allOn();
   delay(1000);
   regOne.allOff();
   delay(1000);

   
  // Initialize the random_arr[] elements (0 to N_Buttons)
  for(int x=0; x<N_Buttons; x++)
  {
    random_arr[x] = x; // assign 0- N_Buttons value
  }
  
  randomSeed(analogRead(0)); // to keep things random 
  
    // Start from the last element and swap  
    // one by one. We don't need to run for  
    // the first element that's why i > 0  
    for (i = N_Buttons - 1; i > 0; i--)  
    {  
        // Pick a random index from 0 to i  
        j = random(i);  
        // Swap arr[i] with the element  
        // at random index  
        temp = random_arr[i];
        random_arr[i] = random_arr[j];
        random_arr[j] = temp;
    }

  // DISPLAY initial LED/7-SEG score *(the timer will be a separate init ((on the setting)))*
  // Display before a button input BECAUSE DUH    

  UpdateLED(random_arr[counter]);
  INIT_FLAG = 0;
}

/* 
 * This function is called repeatedly in the loop and serves as the LED lights updater
 * should also probably deactivate the prevoius LED
 * ID is an element in the already randomized array
 */
void UpdateLED(int ID)
{
  uint64_t val;
  val = LED_arr[ID];
  regOne.pinOn(val);
  
  if(counter!=0)
    regOne.pinOff(LED_arr[previousID]);
  previousID = ID;
}

/* 
 *  Just make sure to throw in a number from 0-9
 *  this functions writes values to the sev seg pins
 */
void Num_Write(int number) 
{
  int pin= 0;
  for (int j=0; j < 7; j++) 
  {
   digitalWrite(pin, num_array[number][j]);
   pin++;
  }
}

/*
 * Updates timer WHEN a second has elapsed
 */
void Tick()
{
  time = millis();
  if(time-prev_time>=999)
  {
    timer--;
    if (timer == -1)
    {
       TIMEUP_FLAG = HIGH;
       timer=0;
    }
      
    prev_time = time;
  }
}

void Display_Score()
{
   int ones, tens;
   ones = counter%10;
   tens = counter/10;

   // DISPLAY ONE'S DIGIT
   Num_Write(ones);
   digitalWrite(Score_u, HIGH);
   digitalWrite(Score_t, LOW);
   digitalWrite(Timer_u, LOW);
   digitalWrite(Timer_t, LOW);

   SS_LOW();
   
   // DISPLAY TEN'S DIGIT
   Num_Write(tens);
   digitalWrite(Score_t, HIGH);
   digitalWrite(Score_u, LOW);
   digitalWrite(Timer_u, LOW);
   digitalWrite(Timer_t, LOW);

   SS_LOW();
}

void Display_Timer()
{
   int ones, tens;
   ones = timer%10;
   tens = timer/10;

   // DISPLAY ONE'S DIGIT
   Num_Write(ones);
   digitalWrite(Timer_u, HIGH);
   digitalWrite(Score_u, LOW);
   digitalWrite(Score_t, LOW);
   digitalWrite(Timer_t, LOW);

   SS_LOW();

   // DISPLAY TEN'S DIGIT
   Num_Write(tens);
   digitalWrite(Timer_t, HIGH);
   digitalWrite(Score_u, LOW);
   digitalWrite(Score_t, LOW);
   digitalWrite(Timer_u, LOW);
   
   SS_LOW();
}

void Display_Mistakes()
{
   int ones, tens;
   ones = mistakes%10;
   tens = mistakes/10;
   
   // DISPLAY ONE'S DIGIT
   Num_Write(ones);
   digitalWrite(Timer_u, HIGH);
   digitalWrite(Score_u, LOW);
   digitalWrite(Score_t, LOW);
   digitalWrite(Timer_t, LOW);

   SS_LOW();

   // DISPLAY TEN'S DIGIT
   Num_Write(tens);
   digitalWrite(Timer_t, HIGH);
   digitalWrite(Score_u, LOW);
   digitalWrite(Score_t, LOW);
   digitalWrite(Timer_u, LOW);
   
   SS_LOW();
}

void SS_LOW()
{
  digitalWrite(Score_u, LOW);
  digitalWrite(Score_t, LOW);
  digitalWrite(Timer_u, LOW);
  digitalWrite(Timer_t, LOW);
}


int checkMyButtons()
{
  static int8_t lastButtonState[N_Buttons] = {1};
  static unsigned long lastMillis[N_Buttons] = {0};
  int8_t pressedButton = -1;
  for (byte i = 0; i < N_Buttons; i++)
  {
    if (millis() - lastMillis[i] > 25UL) // debounce
    {
      lastMillis[i] = millis();
    }
    else
    {
      continue;
    }
    byte state = digitalRead(BUTTON_ID[i]);
    if (state != lastButtonState[i])
    {
      if (state == 0)
      {
        pressedButton = i;
      }
    }
    lastButtonState[i] = state;
  }
  return pressedButton;
}

void ScanMistakes(int pressedButton)
{
  int i;
  for(i=0; i<N_Buttons; i++)
  {
    if(i!= random_arr[counter])
    {
      if(pressedButton == i)
      {
        mistakes++;
      }
    }
  }
}

// Lazy to edit code, I'll just use this for the start button
boolean Debounce(int ID)
{
 // read the state of the switch into a local variable:
  int reading = digitalRead(ID);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
       return HIGH;
      }
    }
  }
  
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  return LOW;
}

/*
 * END STATE FUNCTION 
 * 1. Displays the score 
 * 2. Displays the Time left 
 * 3. Displays the Mistakes (via toggle) 
 * A. Yellow button to exit and reset to settings 
 */
void ENDFUNC()
{
    int toggle =0;
    // TURN ON NEEDED LEDs
    regOne.pinOn(LED_arr[18]); // toggle MISTAKES and Time Left
    digitalWrite(13, LOW); // Start button light
      
    // LOOP end until yellow button is pressed
    while(!Debounce(9))
    {
      int8_t ENDBUTTONS = checkMyButtons();
      Display_Score();

      if (toggle ==0)
        Display_Timer();
      else 
        Display_Mistakes();
        
      if(ENDBUTTONS == 18)
      {
        // check the toggle flag
        if (toggle == 0)
          toggle =1;
        else if (toggle ==1)
          toggle =0;
      }
      
    } // end while

    // What to do when it exits loop
    regOne.pinOff(LED_arr[18]); // turn off the toggle LED

    // RESET ALL FLAGS
    mistakes =0;
    timer = 30;
    counter =0;
    INIT_FLAG =1;
    START_FLAG = 0;
    TIMEUP_FLAG = 0;    
   
}
