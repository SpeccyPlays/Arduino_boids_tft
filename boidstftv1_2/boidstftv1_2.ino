/* Boids - flocking on 24x32 daisy chained LED matrixes
by Benjamin Gibbs
date started 2022/02/09

boidsv

## setup info ##
uses Max7219 LED 3 lots of 8 x 8 modules daisy chained
Use external power as the displays will use a lot of current
CLK - PIN 13
CS - PIN 10
DIN - PIN 11
module 0 = bottom left; top right is 11 in this instance
col 0 is at the bottom
MSB is on the left of the module

## aims ##
Improve method of displaying graphics on 3 daisy chained 8x32 MAX7219 led matrixes

## Updates - newest first ##

*/
//define commands for matrixes and module + screen constants
#include <SPI.h>
#include <TFT.h>
//#include <noDelay.h>
#define CS 10
#define DC 9
#define RESET 8
TFT myScreen = TFT(CS, DC, RESET);
const byte SCREENWIDTH = 160;
const byte SCREENHEIGHT = 128;
struct boidSingle{
  byte x;
  byte y;
  byte oldX;
  byte oldY;
  byte velocity;
  int16_t angle;
};
boidSingle boidArray[25];
uint32_t globalAverageX = 0;
uint32_t globalAverageY = 0;
uint8_t amountOfBoids = sizeof(boidArray) / sizeof(boidArray[0]);
void boidSetup(boidSingle *array);
void showBoids(boidSingle *array);
void firstRule(boidSingle *array);
void secondRule(byte &x, byte &y, int16_t &angle, boidSingle *array);
void removePreviousBoidLocation(boidSingle *array);
void setup(){
  myScreen.begin();
  myScreen.background(0, 0, 0);
  myScreen.stroke(255, 255, 255);
  Serial.begin(9600);
  SPI.begin();
  boidSetup(boidArray);
  firstRule(boidArray);
}
void loop(){
  showBoids(boidArray);
//  delay(50);
  removePreviousBoidLocation(boidArray);
  firstRule(boidArray);
}
void findAngleBetweenPoints(byte &x, byte &y, int16_t &angle){
// find dot product
//i hate maths
//  uint32_t dotProduct = (x * globalAverageX) + (y * globalAverageY);
  double magnitudeOld = sqrt(sq(x) + sq(y));
  double magnitudeNew = sqrt(sq(globalAverageX) + sq(globalAverageY));
//  double a = dotProduct / (magnitudeOld * magnitudeNew);
  double a = /*tan*/tan((x * globalAverageX + y * globalAverageY) / (magnitudeOld  - magnitudeNew));
  angle = (a * 180) / 3.141;//this should be cos(a) but what's there works better
//  if (angle > 0){ 
//    angle += 180;
//  }
//  Serial.println(angle);
}
void firstRule(boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    globalAverageX += array[i].x;
    globalAverageY += array[i].y;
  }
  globalAverageX = (globalAverageX / (amountOfBoids - 1));
  globalAverageY = (globalAverageY / (amountOfBoids - 1));
}
void secondRule(byte &x, byte &y, int16_t &angle, boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    if ((y + 3) == array[i].y | (x + 3) == array[i].x){
      y - 1;
      x - 1;
      array[i].velocity = 10;
//      angle = array[i].angle;
    }
    if ((y - 3) == array[i].y | (x - 3) == array[i].x){
      y + 1;
      x + 1;
      array[i].velocity = 10;
//      angle = array[i].angle;
    }
  }
}
void removePreviousBoidLocation(boidSingle *array){
  myScreen.stroke(0, 0, 0);
  for (byte i = 0; i < amountOfBoids; i++){
    myScreen.rect(array[i].oldX, array[i].oldY, 2, 2);
  }
}
void showBoids(boidSingle *array){
  myScreen.stroke(255, 255, 255);
  for (byte i = 0; i < amountOfBoids; i++){
    myScreen.rect(array[i].x, array[i].y, 2, 2);
    array[i].oldX = array[i].x;
    array[i].oldY = array[i].y;
    array[i].velocity ++;
    findAngleBetweenPoints(array[i].x, array[i].y, array[i].angle);
    int16_t rotation = 360 - array[i].angle;
    int16_t angleToRad = (rotation * 3.141) / 180;
    array[i].y += array[i].velocity * sin(angleToRad);
    array[i].x += array[i].velocity * cos(angleToRad);
    if (array[i].x <= 0){
      array[i].x =+ SCREENWIDTH;
      array[i].velocity = 10;
    }
    if (array[i].x > SCREENWIDTH){
      array[i].x = 0;//=- SCREENWIDTH;
      array[i].velocity = 10;
    }
    if (array[i].y <= 0){
      array[i].y =+ SCREENHEIGHT -2;
      array[i].velocity = 10;
    }
    if (array[i].y >= SCREENHEIGHT){
      array[i].y =- SCREENHEIGHT - 2;
      array[i].velocity = 10;
    }
    secondRule(array[i].x, array[i].y, array[i].angle, boidArray);
  }
}
void boidReset(byte &x, byte &y, byte &velocity, int16_t &angle){
  y = random(0, SCREENHEIGHT - 1);
  x = random(0, SCREENWIDTH - 1);
  velocity = random(10, 20);
  angle = random(0, 359);
}
void boidSetup(boidSingle *array){
  for (byte i = 0; i < amountOfBoids; i++){
    boidReset(array[i].x, array[i].y, array[i].velocity, array[i].angle);
  }
}
