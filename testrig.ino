#include "HX711.h"
#include <SPI.h>
#include <SD.h>

File file; // file object that is used to read and write data
HX711 loadcell1; //scale object for making load cell read/write easier
HX711 loadcell2; //scale object for making load cell read/write easier

//SD PINS

const int CS = 53; //chip select pin for the MicroSD Card Adapter, This is the CS Pin

//Button PIN

const int buttonPin = 2;  // the number of the pushbutton pin

//ENCODER PINS

//FOR BOTH ENCODERS
#define PPR                  2048    // encoder resolution
#define CLOCKWISE            1       // direction constant
#define COUNTER_CLOCKWISE    2       // direction constant


//SPECIFIC TO ENCODER 1

#define ENCODER1PINA         20    // this pin needs to support interrupts
#define ENCODER1PINB         17      // no interrupt required

volatile long encoder1Position = 0;
volatile float angle1 = 0;
short currentDirection1 = CLOCKWISE;
long previousPosition1 = 0;



//SPECIFIC TO ENCODER 2

#define ENCODER2PINA         21    // this pin needs to support interrupts
#define ENCODER2PINB         15      // no interrupt required

volatile long encoder2Position = 0;
volatile float angle2 = 0;
short currentDirection2 = CLOCKWISE;
long previousPosition2 = 0;

//SCALE PINS

const int LOADCELL1_DOUT_PIN = 5;
const int LOADCELL1_SCK_PIN = 6;

const int LOADCELL2_DOUT_PIN = 9;
const int LOADCELL2_SCK_PIN = 10;


void setup() {

  //ENCODER SETUP
  // inputs
  pinMode(ENCODER1PINA, INPUT);
  pinMode(ENCODER1PINB, INPUT);
  pinMode(ENCODER2PINA, INPUT);
  pinMode(ENCODER2PINB, INPUT);

  // INTERRUPTS FOR ENCODERS
  attachInterrupt(digitalPinToInterrupt(20),Interrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(21),Interrupt2, RISING);
  
  Serial.begin (9600);
  Serial.println("\n\n\n");
  Serial.println("Ready.");

  loadcell1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN); // begin loadcell 1
  loadcell2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN); // begin loadcell 2

  loadcell1.set_scale();
  loadcell1.tare(); //Reset the scale to 0

  loadcell2.set_scale();
  loadcell2.tare(); //Reset the scale to 0

  loadcell1.set_scale(-7050); //Adjust to this calibration factor
  loadcell2.set_scale(-7050); //Adjust to this calibration factor

  pinMode(CS, OUTPUT); // chip select pin is set as OUTPUT

  pinMode(buttonPin, INPUT); //button input 
  
  if (!SD.begin(CS)) { // Initialize SD card
  Serial.println("Could not initialize SD card."); // if return value is false, something went wrong.

  }

  SD.remove("poo.txt"); //remove file before running new test

  file = SD.open("poo.txt", FILE_WRITE); // write titles of thrust force, and encoder values to SD card
  if (file) {  
    file.println("THRUST FORCE,Encoder1 Angle,Encoder 2 Angle");
    //file.println("\t");
    //file.println("Encoder 1 Angle,");
    //file.println("\t");
    //file.println("Encoder 2 Angle");
    //file.println("\n");
    file.close(); // close file
   
  } else {
    Serial.println("Could not open file (writing).");
  
  }

  if (loadcell1.is_ready() && loadcell2.is_ready()) { //reading values from first loadcell
          Serial.println("LOADCELLS ARE READY");
        }

}
        
  float readLoadCells(){
  
  return ((loadcell1.get_units()+loadcell2.get_units())/2);
 
}



void writeData(float thrustforce, float encoder1angle, float encoder2angle) //writing something to the SD card 
{
  file = SD.open("poo.txt", FILE_WRITE); // 
  if (file) {  
    file.print(thrustforce,5);
    file.print(",");
    file.print(encoder1angle,5);
    file.print(",");
    file.println(encoder2angle,5);
    file.close(); // close file
   
  } else {
    Serial.println("Could not open file (writing).");
  }
}


void Interrupt1()
{
  if (digitalRead(buttonPin) == HIGH){
    int a = digitalRead(ENCODER1PINA);
    int b = digitalRead(ENCODER1PINB);
  
    if (a == b)
    {
      // b is leading a (counter-clockwise)
      encoder1Position--;
      currentDirection1 = COUNTER_CLOCKWISE;
    }
    else
    {
      // a is leading b (clockwise)
      encoder1Position++;
      currentDirection1 = CLOCKWISE;
    }
  
    // reset to 0 after reaching home position and then calculate angle.
    encoder1Position = encoder1Position % 2048;
    angle1 = ((float)encoder1Position/PPR) * 360;

    //writeData(readLoadCells(),angle1,angle2);
  }
}


void Interrupt2()
{
  if (digitalRead(buttonPin) == HIGH){
    int a = digitalRead(ENCODER2PINA);
    int b = digitalRead(ENCODER2PINB);
  
    if (a == b)
    {
      // b is leading a (counter-clockwise)
      encoder2Position--;
      currentDirection2 = COUNTER_CLOCKWISE;
    }
    else
    {
      // a is leading b (clockwise)
      encoder2Position++;
      currentDirection2 = CLOCKWISE;
    }
  
    // reset to 0 after reaching home position and then calculate angle.
    encoder2Position = encoder2Position % 2048;
    angle2 = ((float)encoder2Position/PPR) * 360;

    //writeData(readLoadCells(),angle1,angle2);
  }
}

//LOOP FUNCTION

void loop() {

    if (digitalRead(buttonPin) == HIGH){ //record data
        
          //readLoadCells();
          writeData(readLoadCells(),angle1,angle2);
          /*
          Serial.print("\t");
          Serial.print(angle1, DEC);
          Serial.print("\t");
          Serial.print(angle2, DEC);
          Serial.print("\t");
          Serial.print("\n");
          Serial.print("\n");
        
        
        if (encoder1Position != previousPosition1){
          /*
            Serial.print(encoder1Position, DEC);
            Serial.print("\n");
            Serial.print(currentDirection1 == CLOCKWISE ? "clockwise" : "counter-clockwise");
            Serial.print("\n");
            Serial.print("angle");
            Serial.print("\t");
            Serial.print(angle1, DEC);
            Serial.print("\n");
            */
            previousPosition1 = encoder1Position;
   
        }
        if (encoder2Position != previousPosition2){
         /*
            Serial.print(encoder2Position, DEC);
            Serial.print("\n");
            Serial.print(currentDirection2 == CLOCKWISE ? "clockwise" : "counter-clockwise");
            Serial.print("\n");
            Serial.print("angle");
            Serial.print("\t");
            Serial.print(angle2, DEC);
            Serial.print("\n");
            */
            previousPosition2 = encoder2Position;
        }
  }
