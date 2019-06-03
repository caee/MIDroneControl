#include <stdlib.h>

#define BAUD_RATE 9600    // Baud rate must comply with the rate on the other end of the signal.
#define VELOCITY_COUNT 4  // How many velocity controls are in use
#define SWITCH_COUNT 0    // How many switches -||-
#define TRIM_COUNT 0      // How many trims -||-
#define CHANNEL_COUNT (VELOCITY_COUNT + SWITCH_COUNT + TRIM_COUNT)  //set the number of channels. Taranis' with old firmware can't have more than 9 channels for PPM because of pulselength limits.
  #define RESET_INTERVAL 10000 // in milliseconds. How long until all channels will reset without input?
#define CHANNEL_MID_VALUE 1500  //set the default servo value
  #define CHANNEL_MIN_VALUE 988
  #define CHANNEL_MAX_VALUE 2012
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000us)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 31  //set PPM signal output pin on the arduino
#define LEDPin 13 // LED Pin
/////////////////////////// DO NOT TOUCH ////////////////////////
#define ALLOWED_SYS_CHANGES 2     // How many system changes can be done in one command. For instance, toggle hold command. <S00> 
#define SIGNAL_LENGTH ( VELOCITY_COUNT * 4 + SWITCH_COUNT * 3 + TRIM_COUNT * 5 + 5)//  <V100S01T0100H1000> Does not expect carriage, newline, and "< ... >".
#define SWITCH_PIN 16
#define CHANNEL_TO_MODIFY 11
#define SWITCH_STEP 100
#define EXT_MACRO_COUNT 0     // User defined macros
