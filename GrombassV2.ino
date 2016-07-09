/*
  Template for creating control of analog effects ; 
onkartgromt.com  - 2015. Oyvind Mjanger 

 */
 
// Pin 13 has an LED connected on most Arduino boards.
// List all pins to mainboard. Compatible with Grombass v2 board rev 1

int CV1 = A0; //boost
int CV2 = A1; //CV expression pedal
int Input = A6; //input sound
int Output = A7; //Output sound

int PWM1 = 5; //L1
int PWM2 = 6; //L2

int DIO1 = 7; //ON/Off LED
int DIO2 = 8; //BOOST LED
int DIO3 = 9; //MIDDLE LED

int RELAY = 10; //TRUE BYPASS RELAY

int BP0 = 2; //INT0 - footswitches actually connected to pin 2/ON/OFF
int BP1 = 1; //INT1 - footswitches actually connected to pin 3/BOOST
 
//END PIN def. 
 
double CV1Input = 0;  // variable to store the value coming from CV1
double CV2Input = 0;  // variable to store the value coming from CV2

double Toggle1 = 0;  // variable to store the value coming from Switch1
double Toggle2 = 0;  // variable to store the value coming from Switch2

int PWMout0;
int PWMout1;

int tapPressed = 0; //active low
int numberOfButtonPresses = 0;
int tempoState = 0;
int lastTempoState = LOW;
int TapTempo = 0;
int PotTempo = 1;
int MidiTempo = 0;
double LastTempo;

unsigned long previousMillis = 0;
long interval = 100;
unsigned long  startTime;
unsigned long  endTime;
int LFOmode = 0;   //0=sine, 1= square, 2=triangle, 3= sawtooth

double  tempo=300;            //time between tabs [ms]
int LFOupdaterate = 1000;
float modifier = 1.0;
float counter = 0;
float counterlength = 100;
float maxFreq = 62.0;
float minFreq = 5.0;

double fDepth = 0.5;
double fFreq = 0.05;
double temp = 10.0;
double a;
volatile int state = LOW;
//IntervalTimer LFOtimer;

///////////////////////////////////////////////////////
// the setup routine runs once when you press reset:
void setup() {        
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);   
  // initialize the digital pins as outputs. Used for LEDs and Relay
  pinMode(DIO1, OUTPUT);   
  pinMode(DIO2, OUTPUT);     
  pinMode(DIO3, OUTPUT); 
 // pinMode(PB0, INPUT); 
  attachInterrupt(0, blink, RISING);

}

///////////////////////////////////////////////////////
// the loop routine runs over and over again forever:


void loop()
{ 
  delay(100);
  Serial.println(numberOfButtonPresses); 
  digitalWrite(DIO1, state);
}

void blink()
{
  detachInterrupt(0);
  numberOfButtonPresses++;
  state = !state;
  delay(15);
  attachInterrupt(0, blink, RISING);
}
/*void loop() {
  delay(10);
    // read the value from the CV1 potmeter:
  CV1Input = analogRead(CV1);  
  CV2Input  =analogRead(CV2);
     
  Toggle1 = analogRead(SW1);

  fDepth = CV1Input / 1023;

  if ((CV2Input > (LastTempo + 2)) || (CV2Input < (LastTempo - 2)) ){ //Check it Potmeter has moved  || TapTempo == 0
    fFreq = CV2Input / 1023;
    temp = CV2Input / 8;
    PotTempo = 1;
    TapTempo = 0;
    Serial.print(CV2Input); 
    Serial.print(" ,"); 
    Serial.println(LastTempo);  
  }
  
  if (fFreq < minFreq) fFreq = minFreq;
  
  PWMout2 = map(CV3Input, 0, 1023, 0, 255);   
  
    // change the analog out value:
  analogWrite(PWM1, PWMout0);  
  analogWrite(PWM2, PWMout1); 
  analogWrite(PWM3, PWMout2);
  
    LastTempo = CV2Input;


  Serial.print("\t Tempo = ");      
  Serial.print(tempo); 
  Serial.print("\t LFOmode = ");      
  Serial.println(LFOmode); 
  }

*/
