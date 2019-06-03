#include "DroneSignal.h"

DSDroneSignal::DSDroneSignal()
{
  // General class values
  // Calibration specified for Taranis
  DSDroneSignal::VelocityCalibration = -5;   // -5 equals -1 to the channel value when looking at the trainer menu on the Taranis.
  DSDroneSignal::VelocityMappingCalibration = -10;    // Used when mapping the velocity from 0-100 to CHANNEL_MIN to CHANNEL_MAX in IOHandler::InputProcess()

  // TODO: Modular later on
  ValidVelocityNames[0] = 'E';    // Elevator/Pitch
  DSDroneSignal::PPMChannels[0].ChannelName = 'E';
    DSDroneSignal::PPMChannels[0].ChannelType = VELOCITY;
    DSDroneSignal::PPMChannels[0].VelocityType = ELEVATION;
  ValidVelocityNames[1] = 'T';    // Throttle
  DSDroneSignal::PPMChannels[1].ChannelName = 'T';
    DSDroneSignal::PPMChannels[1].ChannelType = VELOCITY;
    DSDroneSignal::PPMChannels[1].VelocityType = THROTTLE;
  ValidVelocityNames[2] = 'R';    // Rudder/Yaw
  DSDroneSignal::PPMChannels[2].ChannelName = 'R';
    DSDroneSignal::PPMChannels[2].ChannelType = VELOCITY;
    DSDroneSignal::PPMChannels[2].VelocityType = RUDDER;
  ValidVelocityNames[3] = 'A';    // Aileron/Roll
  DSDroneSignal::PPMChannels[3].ChannelName = 'A';
    DSDroneSignal::PPMChannels[3].ChannelType = VELOCITY;
    DSDroneSignal::PPMChannels[3].VelocityType = AILERON;

  // VelocityChannels will come first
  int VelocityIterator = 0;
  for (int i = 0; i < CHANNEL_COUNT; i++)
  {
    DSDroneSignal::PPMChannels[i].bIsTimeWatchDogOn = true;
    DSDroneSignal::PPMChannels[i].bHasBeenUpdated = false;
    DSDroneSignal::PPMChannels[i].UpdateIntervalDeadline = 3000;    // in milliseconds
    DSDroneSignal::PPMChannels[i].bIsChannelOnHold = false;
    
    if (DSDroneSignal::PPMChannels[i].ChannelType == VELOCITY)
    {
      VelocityIterator++;
      
      DSDroneSignal::PPMChannels[i].DefaultValue = CHANNEL_MID_VALUE + ( DSDroneSignal::Velocity_Max / 100 * DSDroneSignal::GetVelocityCalibration() );
      
      DSDroneSignal::PPMChannels[i].ChannelName = ValidVelocityNames[i];
      DSDroneSignal::PPMChannels[i].ChannelIndex = i;   // Will determine which ppm-value to change
      DSDroneSignal::PPMChannels[i].MinValue = CHANNEL_MIN_VALUE;
      DSDroneSignal::PPMChannels[i].MaxValue = CHANNEL_MAX_VALUE;
      DSDroneSignal::PPMChannels[i].bHasBeenUpdated = false;


      // Set initial Value of channel.
      DSDroneSignal::PPMChannels[i].Value = DSDroneSignal::PPMChannels[i].DefaultValue;
    }
  }

  /* Hold Properties */
  DSDroneSignal::bIsHoldSignalOn = false;
  
} 


/* ChangeVelocity
 *  Change velocity of velocity control.
 *  params:
 *    ChannelIndex: Which channel to be changed
 *    Value: The new  value for the channel in addition to the calibration constant given
 */
void DSDroneSignal::ChangeVelocity(int ChannelIndex, int Value)
{
  // TODO?: Make failsafe so that the value can only be below a certain level if a "password" Macro has been given in the serial prompt.
  //       This is to prevent motor start, for instance. Not sure how this will work with other devices than a terminal, though.
  DSDroneSignal::PPMChannels[ChannelIndex].Value = Value + DSDroneSignal::GetVelocityCalibration();
  DSDroneSignal::ToggleChannelUpdated(ChannelIndex, true);    // Mark channel updated
};


/* ToggleChannelUpdated
 *  Set a channel to updated or not updated. Particularly used for resetting individual channels, 
 *  but will remain used given potential for other uses
 *  
 *  params:
 *    ChannelIndex: Which Channel to toggle
 *    bIsUpdated: Toggle updated or not updated?
 * 
 */
void DSDroneSignal::ToggleChannelUpdated(int ChannelIndex, bool bIsUpdated)
{
  DSDroneSignal::PPMChannels[ChannelIndex].bHasBeenUpdated = bIsUpdated;

  if(DSDroneSignal::PPMChannels[ChannelIndex].bHasBeenUpdated)
  {
    DSDroneSignal::PPMChannels[ChannelIndex].UpdateTime = millis();   // Time channel has been updated.
  }
};


/* ShouldChannelReturnToDefaultValue
 *  Will check if it is time to reset channel value due to lack of input.
 *  Roll-over safe use of millis() function.
 *  
 *  params: 
 *    ChannelIndex: Which channel to reset
 *    
 */
bool DSDroneSignal::ShouldChannelBeReset(int ChannelIndex) const
{
  
  
  if ( !DSDroneSignal::HasChannelBeenUpdated(ChannelIndex) ) // || !DSDroneSignal::PPMChannels[ChannelIndex].bIsTimeWatchDogOn)
  {
    return false;
  }
  else
  {
    const unsigned long UpdateTime = DSDroneSignal::GetUpdateTime(ChannelIndex);
    const unsigned long ChannelUpdateDeadline = GetChannelUpdateDeadline(ChannelIndex);
    const unsigned long CurrentTime = millis();
   
    if ((unsigned long)(CurrentTime - UpdateTime) >= ChannelUpdateDeadline)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
};

void DSDroneSignal::SetHoldSignal(bool bToggleHoldOn, unsigned long HoldDuration)
{
  DSDroneSignal::bIsHoldSignalOn = bToggleHoldOn;
  if (DSDroneSignal::bIsHoldSignalOn)
  {
    DSDroneSignal::HoldDuration = HoldDuration;
    DSDroneSignal::SetCurrentHoldTime();
    for (int ChannelIndex = 0; ChannelIndex < CHANNEL_COUNT; ChannelIndex++)
    {
      if (DSDroneSignal::PPMChannels[ChannelIndex].bIsChannelOnHold)
      {
        DSDroneSignal::ToggleChannelOnHold(ChannelIndex, true);
      }
    }
    
  }
  else
  {
    DSDroneSignal::HoldDuration = 0;
    for (int ChannelIndex = 0; ChannelIndex < CHANNEL_COUNT; ChannelIndex++)
    {
      if (DSDroneSignal::PPMChannels[ChannelIndex].bIsChannelOnHold)
      {
        DSDroneSignal::ToggleChannelOnHold(ChannelIndex, false);
      }
    }
  }
};

/* IsHoldFinished
 *  Will see whether a hold command has been completed or not
 * 
 */
bool DSDroneSignal::IsHoldFinished()
{
  if ( (unsigned long)( millis() - DSDroneSignal::GetCurrentHoldTime()) >= DSDroneSignal::GetHoldDuration() )
  {
    return true; 
  }
  else
  {
    return false;
  }
};


/* ResetChannel
 *  Will reset a given channel
 *  
 *  params:
 *    ChannelIndex: Which channel to reset.
 * 
 * TODO: Send array of channels to be reset instead.
 * 
 */
void DSDroneSignal::ResetChannel(int ChannelIndex)
{
  DSDroneSignal::PPMChannels[ChannelIndex].Value = DSDroneSignal::PPMChannels[ChannelIndex].DefaultValue;
  DSDroneSignal::ToggleChannelUpdated(ChannelIndex, false);   // Mark channel as not updated.
  DSDroneSignal::PPMChannels[ChannelIndex].UpdateTime = millis();
  Serial.print( F("[") );
  Serial.print(DSDroneSignal::GetPPMChannelName(ChannelIndex)); 
  Serial.println( F("RES]") );
}



