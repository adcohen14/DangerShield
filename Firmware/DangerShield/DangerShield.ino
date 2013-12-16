/*
 Danger Shield Example Sketch
 Written by Chris Taylor 3-3-2010
 Tweaked by Nathan Seidle 10-22-2012
 Spark Fun Electronics

 This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 The project is OSHW and the schematic is released under Creative Commons Share Alike w/ Attribution v3.0
 
 This is the example code for the Danger Shield product by SparkFun Electronics. 
 It demonstrates each feature of the shield. Move one of the sliders to get a different sound out of the buzzer. 
 Move another slider to change the digit being displayed on the seven segment display. Move the last slider to 
 change the brightness of one of the two status LEDs. Press one of the large buttons to toggle on/off the status LED. 
 Cover the light sensor to 'scare' the unit.

 This code works best with product SKU 10115: https://www.sparkfun.com/products/10115

 Knock sensor, new temp sensor: https://www.sparkfun.com/products/10115
 Cap touch sensor: https://www.sparkfun.com/products/10570

 The idea of a Danger Shield was originally created by Zach Hoeken (http://www.zachhoeken.com/) released CC-SA.

*/

// Shift register bit values to display 0-9 on the seven-segment display
const byte ledCharSet[10] = {
  B00111111, B00000110, B01011011, B01001111, B01100110, B01101101, B01111101, B00000111, B01111111, B01101111
};

// Pin definitions
#define SLIDER1  0
#define SLIDER2  1
#define SLIDER3  2

#define KNOCK    5

#define BUTTON1  10
#define BUTTON2  11
#define BUTTON3  12

#define LED1  5
#define LED2  6

#define BUZZER   9

#define TEMP  4

#define LIGHT  3

#define LATCH 7
#define CLOCK 8
#define DATA 4

int avgLightLevel;

void setup()
{
  Serial.begin(57600);
  
  pinMode(BUTTON1,INPUT);
  pinMode(BUTTON2,INPUT);
  pinMode(BUTTON3,INPUT);
  
  digitalWrite(BUTTON1,HIGH);
  digitalWrite(BUTTON2,HIGH);
  digitalWrite(BUTTON3,HIGH);
  
  pinMode(BUZZER, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA,OUTPUT);

  Serial.println("Danger Shield Component Test");  
  
  //Take 16 readings from the light sensor and average them together
  avgLightLevel = 0;
  for(int x = 0 ; x < 16 ; x++)
    avgLightLevel += analogRead(LIGHT);
  avgLightLevel /= 16;

  Serial.print("Avg: ");
  Serial.print(avgLightLevel);
  
}

void loop()
{
  if(digitalRead(BUTTON1) == LOW) {
    Serial.println("Button 1!");
    digitalWrite(LED1, HIGH);
  }
  else
    digitalWrite(LED1, LOW);

  //Read inputs
  int val1 = analogRead(SLIDER1);
  int val2 = analogRead(SLIDER2);
  int val3 = analogRead(SLIDER3);
  int lightLevel = analogRead(LIGHT);
  int temp = analogRead(TEMP);

  Serial.print("Sliders: ");
  Serial.print(" ");
  Serial.print(val1);
  Serial.print("  ");
  Serial.print(val2);
  Serial.print("  ");
  Serial.print(val3);
  Serial.print("  ");
  Serial.print(lightLevel);
  
  Serial.println();
  
  int ledLevel = map(val1, 0, 1020, 0, 255);
  int numToDisplay = map(val2, 0, 1020, 0, 9);
  long buzSound = map(val3, 0, 1020, 1000, 10000);
  
  //Set outputs
  if(buzSound > 1100)
    tone(BUZZER, buzSound); //Set sound value
  else
    noTone(BUZZER);
  
  analogWrite(6, ledLevel); //Set LED brightness

  //Set 7 segment display
  digitalWrite(LATCH,LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, ~(ledCharSet[numToDisplay]));
  digitalWrite(LATCH,HIGH);
  
  //If light sensor is covered up, freak out
  while(lightLevel < (avgLightLevel * 3 / 4)) {
    
    if(digitalRead(LED1) == LOW){
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
    }
    else {
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
    }

    digitalWrite(LATCH,LOW);
    shiftOut(DATA, CLOCK, MSBFIRST, ~(ledCharSet[numToDisplay]));
    digitalWrite(LATCH,HIGH);
    numToDisplay++;
    if(numToDisplay > 9) numToDisplay = 0;

    tone(BUZZER, buzSound);
    buzSound += 100;
    if(buzSound > 3000) buzSound = 1000;
    
    delay(25);

    lightLevel = analogRead(LIGHT); //We need to take another reading to be able to exit the while loop
  }
}
