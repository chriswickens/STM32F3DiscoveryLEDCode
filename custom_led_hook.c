/*
 *	Custom LED commands
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "stm32f3_discovery_gyroscope.h"
#include "../STM32F3-Discovery/stm32f3_discovery.h"

#include "common.h"

#define DELAYTIME 50 // Master delay time for HAL_Delay() use
#define GYROSENS 65000 // Master gyroscope sensitivity setting
#define GYRO_Z_DELAY 50 // Delay for leds using gyro Z axis

const Led_TypeDef MyLeds[] = {LED3, LED5, LED7, LED9, LED10, LED8, LED6, LED4};
const uint32_t ledCount = sizeof(MyLeds)/sizeof(MyLeds[0]);

// ARM function prototype:
int asmtest(int *x);
int stackfunct();

// Turn OFF ALL leds
void TurnOffAllLeds()
{
  for (int i = 0; i < ledCount; i++)
  {
    BSP_LED_Off(MyLeds[i]);
  }
}

// Turn ON ALL leds
void TurnOnAllLeds()
{
    for (int i = 0; i < ledCount; i++)
  {
    BSP_LED_On(MyLeds[i]);
  }
}

// Gryoscope /w leds function
void Gleds(int mode)
{
  float xyz[3];

  if(mode != CMD_INTERACTIVE) 
  {
    return;
  }

  while(!BSP_PB_GetState(BUTTON_USER))
  {

    // Delay here ensures the LEDs will stay on for a brief moment
    HAL_Delay(50);

    // Then turn off the leds
    TurnOffAllLeds();

    // Read the Gyro
    BSP_GYRO_GetXYZ(xyz);

    // X axis - forward/backwards TILT when flat
    if(xyz[0] < -GYROSENS)
    {
      // printf("X: %lf\n", xyz[0]);
      BSP_LED_On(LED3);
    }
    else if (xyz[0] > GYROSENS)
    {
      // printf("X: %lf\n", xyz[0]);
      BSP_LED_On(LED10);
    }

    // Y axis - left/right TILT when flat
    if(xyz[1] < -GYROSENS)
    {
      // printf("X: %lf\n", xyz[0]);
      BSP_LED_On(LED6);
    }
    else if (xyz[1] > GYROSENS)
    {
      // printf("X: %lf\n", xyz[0]);
      BSP_LED_On(LED7);
    }

    // Z axis - rotate entire board left/right when flat
    if(xyz[2] < -GYROSENS)
    {
      // printf("X: %lf\n", xyz[2]);
      for (int i = 0; i < 4; i++)
      {
        BSP_LED_On(MyLeds[i]);
        HAL_Delay(GYRO_Z_DELAY);
      }
    }

    else if (xyz[2] > GYROSENS)
    {
      // printf("X: %lf\n", xyz[2]);
      for (int i = 7; i > 4; i--)
      {
        BSP_LED_On(MyLeds[0]);
        // printf("I value: %i\n", i);
        BSP_LED_On(MyLeds[i]);
        HAL_Delay(GYRO_Z_DELAY);
      }
    }
  }
  TurnOffAllLeds();
}

ADD_CMD("gleds", Gleds,"                GYRO /w Leds");

void LedCycle(int argument)
{
  uint32_t speed, cycle; // Argument variables
  int rc; // The value of the argument (this is mangled after being processed)
  // If you typed testing 50 4 (assuming 50 is the delay and 4 is the cycle tyle)
  // you would need to fetch the FIRST argument, which would mangle the original string
  // which means it would pull out the 50 and leave the 4, which can then be fetched after

  /*
  * If the argument != CMD_INTERACTIVE  
  */
  if(argument != CMD_INTERACTIVE) 
  {
    printf("argument != interactive command\n"); // This is only used when 'help FUNCTION_NAME' is entered into the prompt
    return;
  }
  if(argument == CMD_LONG_HELP || argument == CMD_SHORT_HELP)
  {
    printf("Short or Long help!\n");
    return;
  }

  // Mangle the input string to get the FIRST argument for the delay SPEED
  // Notice the &speed? It stores the value it found in that variable, same for the cycle
  rc = fetch_uint32_arg(&speed);
  if(rc || speed > 1000 || speed < 20) 
  {
    printf("No cycle speed provided, defaulting to 100ms\n");
    speed = 100;
  }
  
  // Mangle the input string to get the SECOND argument for the cycle
  rc = fetch_uint32_arg(&cycle);
  if(rc || cycle > 3)
  {
    printf("No cycle selected, defaulting to cycle 1!\n");
    cycle = 1;
  }
  
  // Code to rotate through, one LED at a time
  if(cycle == 1)
  {
    int ledStart = 0;
    while(!BSP_PB_GetState(BUTTON_USER))
    {
      BSP_LED_On(MyLeds[ledStart % 8]);
      ledStart++;
      HAL_Delay(speed);
      BSP_LED_Off(MyLeds[(ledStart - 1) % 8]);
    }
  }

  // Code to light individual LEDs backwards
  // Turn off all leds after a full cycle
  // and start again
  if(cycle == 2)
  {
    int ledStart = 8;
    
    while(!BSP_PB_GetState(BUTTON_USER))
    {
      BSP_LED_On(MyLeds[ledStart % 8]);
      ledStart--;

      if(ledStart < 0)
      {
        ledStart = 8;
        TurnOffAllLeds();
      }

      HAL_Delay(speed);
    }
    TurnOffAllLeds();
  }

  // Turn on LEDs and turn off LEDs -3 behind the led being turned ON
  // Looks like a snake trail
  if(cycle == 3)
  {
    int ledStart = 0;
    int ledOff = 0;
    while(!BSP_PB_GetState(BUTTON_USER))
    {
      BSP_LED_On(MyLeds[ledStart % 8]);
      ledStart++;

      ledOff = (ledStart + 4) % 8;
      BSP_LED_Off(MyLeds[ledOff]);

      HAL_Delay(speed);
    }

    TurnOffAllLeds();
  }
}

ADD_CMD("cycle", LedCycle,"\n\tcycle <time in MS> <cycle style>\n\tStyles:\n\t1: One led at a time\n\t2: Light backwards, stay on until end\n\t3: Snake style lights\n")


void FrontBack(int action)
{
  int left = 0; // Start
  int right = 7; // End

  while (!BSP_PB_GetState(BUTTON_USER)) 
  {
    // printf("START\n");
    for (int i = 0; i < 8; i++) 
    {
      if (i % 2 == 0) 
      {
        HAL_Delay(DELAYTIME);
        // printf("ON IF LEFT: %d\n", left);
        BSP_LED_On(MyLeds[left]);
        
        left++;
      }

      else 
      {
        HAL_Delay(DELAYTIME);
        // printf("ON ELSE RIGHT: %d\n", right);
        BSP_LED_On(MyLeds[right]);
        
        right--;
      }
    }

    HAL_Delay(500);

    // LED backward pattern
    for (int i = 8; i >= 0; i--) 
    {
      if (i % 2 == 0) 
      {
        HAL_Delay(DELAYTIME);
        // printf("OFF IF: %d\n", i%2);
        // printf("OFF RIGHT: %d\n", right);
        BSP_LED_Off(MyLeds[right]);
        
        right++;
      } 
      
      else 
      {
        HAL_Delay(DELAYTIME);
        // printf("OFF ELSE: %d\n", i);
        // printf("OFF LEFT: %d\n", left);
        BSP_LED_Off(MyLeds[left]);
        
        left--;
      }
    }

    TurnOffAllLeds();
    HAL_Delay(500);

    // Reset for the next forward pattern
    left = 0;
    right = 7;    
  }
}

ADD_CMD("frontback", FrontBack,"Led cycle, front to back")

// Function to test getting a result from an ARM Thumb assembly function
void asmTest()
{
  int testNP; // Create a place to store the return from the assembly
  printf("Testing ASM out: %i\n", asmtest(&testNP)); // Calls the arm function asmtest, check the code!
}

ADD_CMD("asmtest", asmTest, "TESTING!")

void stackTesting()
{
  printf("Value: %i", stackfunct());
}
ADD_CMD("stest", stackTesting, "Stack stuff")