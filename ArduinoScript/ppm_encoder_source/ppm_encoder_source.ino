/*
 * PPM generator originally written by David Hasko
 * on https://code.google.com/p/generate-ppm-signal/ 
 * 
 */


#include "DroneSignal.h"
#include "IOHandling.h"

//////////////////////CONFIGURATION///////////////////////////////
#include "Config.h"   // Go here to set up params
//////////////////////////////////////////////////////////////////

byte previousSwitchValue;

IOHandler IOHandling1;
DSDroneSignal UAV1;
int currentChannelStep;

void setup()
{  
  
  previousSwitchValue = HIGH;
  

  // Setup Serial comms
  Serial.begin(BAUD_RATE);
  // Setup LED to show if Serial is busy.
  pinMode(LEDPin, OUTPUT);  // LED pin


  pinMode(sigPin, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  digitalWrite(sigPin, !onState);  // set the PPM signal pin to the default state (off)
  
  cli();    // Defined in Arduino.h as noInterrupts(). Disables interrupts. Enabled by default.
  // TCCR: Timer/Counter Control Register sets timer mode. Resets if set to 0
  TCCR1A = 0;
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, will be set to correct times in the ISR interrupt.
  // Change the bits of registers,
  TCCR1B |= (1 << WGM12);   // turn on CTC mode. Clear Timer on Compare, hardware-reset and -start when compare and timer registers match.
  TCCR1B |= (1 << CS11);    // 8 prescaler: 0,5 microseconds at 16MHz (Only Arduino PRO has a different clock: 8MHz). Multiply pulse length with 2 to get 1ms
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  sei();    // Defined in Arduino.h as interrupts(). Enables interrupts. ISR is dependent of this

  currentChannelStep = SWITCH_STEP;

  // Check too see if ready, or in case that Arduino resets when it shouldn't or vice versa.
  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    if (i % 2)    // Odd
    {
      digitalWrite(LEDPin, HIGH);
    }
    else
    { 
      digitalWrite(LEDPin, LOW);
    }
    delay(100); 
  }
  digitalWrite(LEDPin, LOW);    // Just as a safety measure. Want to be low.

  // DEBUG: See initial value of PPM-channel
  char CurrentPPMChannelValue[3]; 
  sprintf(CurrentPPMChannelValue, "%03i", UAV1.GetPPMChannelValue(0));
  Serial.println("[Ready]");
}



void loop()
{
  // reset individual channels in the case that nothing has been modified during that duration
  /*
  for (int ChannelIndex = 0; ChannelIndex < CHANNEL_COUNT; ChannelIndex++)
  {
    if (UAV1.ShouldChannelBeReset(ChannelIndex) && !UAV1.IsChannelOnHold(ChannelIndex))
    {
      UAV1.ResetChannel(ChannelIndex);
    }
  }
  */

  
  int switchState;
  switchState = digitalRead(SWITCH_PIN);

  
  



  
  static bool bIsSignalBeingHeld = false;   // Used to check if signal is being hold, giving user some response

  // IO handling happening here. First check if signal is on hold.
  if (UAV1.IsSignalOnHold())
  {
    if (bIsSignalBeingHeld == false)
    {
      Serial.println("[HOLDON]");
      bIsSignalBeingHeld = true;
    }
    if (UAV1.IsHoldFinished())      // What to do after hold command should tick off.
    {
      UAV1.SetHoldSignal(false, 0);
      bIsSignalBeingHeld = false;
      Serial.println("[HOLDOFF]");
      for (int i = 0; i < CHANNEL_COUNT; i++)
      {
        UAV1.ResetChannel(i);
      }
    }
  }
  else    // If signal is not being held
  {
    if (bIsSignalBeingHeld == true)   // In the case of a override this is necessary
    {
      Serial.println("[HOLDOFF]");
      bIsSignalBeingHeld = false;
    }
    // In case of complete loss of input at the serial comms this will return all channels to default values instead of only single channels.
    // Reset all channels given lack of input for a duration, if signal is not being held.
    IOHandling1.UpdateCurrentTime();
    IOHandling1.OutputTimeoutCheck(UAV1);
  }
  IOHandling1.SerialMonitor(UAV1);

  
  if (switchState == LOW && previousSwitchValue == HIGH)
  {
  
    static int val = SWITCH_STEP;

    UAV1.ChangeVelocity(CHANNEL_TO_MODIFY, UAV1.GetPPMChannelValue(CHANNEL_TO_MODIFY) + currentChannelStep);
    
    if (UAV1.GetPPMChannelValue(CHANNEL_TO_MODIFY) > 2000 || UAV1.GetPPMChannelValue(CHANNEL_TO_MODIFY) < 1000) 
    {
      currentChannelStep = currentChannelStep * -1;
      UAV1.ChangeVelocity(CHANNEL_TO_MODIFY, UAV1.GetPPMChannelValue(CHANNEL_TO_MODIFY) + currentChannelStep);
    }
    
  }  
  
  previousSwitchValue = switchState;
  
  delay(10);
}








/* Interrupt Service Routine (ISR)
 *  ISR is a timed interrupt since Arduinos do not support multithreading.
 *  This will break from the main routine given a certain time interval and run the ISR body.
 */
ISR(TIMER1_COMPA_vect)
{  
  static boolean state = true;
  
  TCNT1 = 0;    // Timer/counter 1 value

  
  if (state)    // start pulse
  {  
    digitalWrite(sigPin, onState);    // Give pulse
    OCR1A = PULSE_LENGTH * 2; // Output Compare Register, when OCR1A == TCNT1 and interrupt is triggered
    state = false;
  } 
  else      // end pulse and calculate when to start the next pulse
  {  
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= CHANNEL_COUNT)
    {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;
      OCR1A = (FRAME_LENGTH - calc_rest) * 2;
      calc_rest = 0;
    }
    else
    {
      OCR1A =  (UAV1.GetPPMChannelValue(cur_chan_numb) - PULSE_LENGTH) * 2;
      calc_rest = calc_rest + UAV1.GetPPMChannelValue(cur_chan_numb);
      cur_chan_numb++;
    }
  }
}
