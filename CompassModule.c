/*
Author: Joe Michaels
Assignment 4 I2C Compass module
Date Complete: 3/16/17
SER 486 
*/
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#define DATAREADY 0
#define ADDRESS 0x1e

#define MODE       0x02  //Mode Register
#define REGX_MOST  0x03  //MSB X Register
#define REGX_LEAST 0x04  //LSB X Register
#define REGZ_MOST  0x05  //MSB Z Register
#define REGZ_LEAST 0x06  //LSB Z Register
#define REGY_MOST  0x07  //MSB Y Register
#define REGY_LEAST 0x08  //LSB Y Register

#define STATUS     0x09 //Status Register

#define CONTINUOUS 0x00 // set continuous read
#define SINGLE     0x01 // set single read

#define PI 3.14159265358979

int main(void)
{

  //to run on Pi:
  //gcc -Wall -o compMod CompassModule.c -lwiringPi -lm
  
  uint8_t msb,lsb;
  int16_t x,y;
  int fd,direction,status;
  double angle;
  char cardDirection;
  
  fd = wiringPiI2CSetup(ADDRESS); //get file device

  if (fd < 0)
  {
    printf("I2C setup failed\n");
  }else
  {
    printf("I2C setup complete\n");
  }

  wiringPiI2CWriteReg8(fd,MODE,CONTINUOUS); //set compass to continuous read
  
  while(1)
  {
    // module required a read from each register before it
	// would output a new reading
	
    msb = wiringPiI2CReadReg8(fd,REGX_MOST); // grab most significant byte
    lsb = wiringPiI2CReadReg8(fd,REGX_LEAST); // grab least significant byte
    x = (int16_t)(msb << 8 | lsb); // shift 8 and or values, cast to signed
	//test proof of concept, had to look this up, not using in final version
    //x = (x > 0x7fff) ? x - (0xffff - 1) : x; 
    
    msb = wiringPiI2CReadReg8(fd,REGY_MOST);
    lsb = wiringPiI2CReadReg8(fd,REGY_LEAST);
    y = (int16_t)(msb << 8 | lsb);
    //y = (y > 0x7fff) ? y - (0xffff - 1) : y;

    msb = wiringPiI2CReadReg8(fd,REGZ_MOST);
    lsb = wiringPiI2CReadReg8(fd,REGZ_LEAST);
    // z = (int16_t)(msb << 8 | lsb);
    //z = (z > 0x7fff) ? z - (0xffff - 1) : z;
    
    status = wiringPiI2CReadReg8(fd,STATUS);

	// calculate the angle in rads, convert to degrees
    angle = atan2(y,x) * (180/PI); 

    direction = round(angle/90) * 90; // round to nearest 90 degrees
    
    switch(direction){
    case 0:
      cardDirection = 'W';
      break;
    case 90:
      cardDirection = 'N';
      break;
    case 180:
      cardDirection = 'E';
      break;
    case 270:
      cardDirection = 'S';
      break;
    case 360:
      cardDirection = 'W';
      break;
    default:
      printf("\nSwitch error, direction: %d\n",direction);
      break;
    }
    
    printf("Status: %d\n",status);
    printf("X: %d, Y: %d, Angle: %f\n", x,y,angle);
    printf("Cardinal direction: %c\n\n",cardDirection);
    delay(1000);
  }
	
  return 0;
}

