#ifndef DroneSignal
#define DroneSignal

//#include "DroneLevelledControl.h"
#include "Config.h"
#include <Arduino.h>


class DSDroneSignal
{
 public:
  DSDroneSignal();    // Constructor set default ppm-values
  int EditControl(char InputSegment);
  void ChangeVelocity(int Channel, int Value);
  void ResetChannel(int ChannelIndex);
  inline int GetVelocityCalibration() const { return DSDroneSignal::VelocityCalibration;};    // Some calibration to the signal may be needed
  inline int GetVelocityMappingCalibration() const { return DSDroneSignal::VelocityMappingCalibration; };

  /* Channel Properties */
  inline int GetPPMMinValue() const { return DSDroneSignal::Velocity_Min; };
  inline int GetPPMMaxValue() const { return DSDroneSignal::Velocity_Max; };
  inline int GetPPMChannelValue(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].Value; };
  inline String GetPPMChannelName(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].ChannelName; };
  inline unsigned int GetChannelDefaultValue(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].DefaultValue; };
  inline unsigned int GetChannelMinValue(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].MinValue; };
  inline unsigned int GetChannelMaxValue(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].MaxValue; };

  
  enum ChannelTypes {NOT_IN_USE, VELOCITY, SWITCH, MULTISWITCH}; // Disabled, 0-100, 0 or 1, multiswitch
    inline ChannelTypes GetChannelType(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].ChannelType; };
    inline char GetVelocityChannelName(int VelocityIndex) const { return DSDroneSignal::ValidVelocityNames[VelocityIndex]; };

    
  /* Used to make sure UAV holds in case of a loss of input keeps channel values static. */
  bool ShouldChannelBeReset(int ChannelIndex) const;
  inline bool HasChannelBeenUpdated(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].bHasBeenUpdated; };
  inline unsigned long GetUpdateTime(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].UpdateTime; };
  inline unsigned long GetChannelUpdateDeadline(int ChannelIndex) const { return DSDroneSignal::PPMChannels[ChannelIndex].UpdateIntervalDeadline; };

  /* Hold functions */
  void SetHoldSignal(bool bToggleHoldOn, unsigned long HoldDuration);
  bool IsSignalOnHold() const { return DSDroneSignal::bIsHoldSignalOn; };
  bool IsHoldFinished();
  inline void SetCurrentHoldTime() { DSDroneSignal::CurrentHoldTime = millis(); };
  inline unsigned long GetCurrentHoldTime() const {return DSDroneSignal::CurrentHoldTime; };
  inline unsigned long GetHoldDuration() const { return DSDroneSignal::HoldDuration; };
  inline bool IsChannelOnHold(int ChannelIndex) const {return DSDroneSignal::PPMChannels[ChannelIndex].bIsChannelOnHold; };
  inline void ToggleChannelOnHold(int ChannelIndex, bool bIsOnHold) { DSDroneSignal::PPMChannels[ChannelIndex].bIsChannelOnHold = bIsOnHold; };

  

  
    
 private: 
  // Used to achieve better precision on the values. For instance, there would be an offset from 0 of about 5, and the min and max values would be offset with about 10
  int VelocityCalibration;
  int VelocityMappingCalibration;
 
  void ToggleChannelUpdated(int ChannelIndex, bool bIsUpdated);
  bool bIsHoldSignalOn;
    unsigned long CurrentHoldTime;    // in milliseconds
    unsigned long HoldDuration;   // in milliseconds
  
  
    enum VelocityTypes {UNSPECIFIED, ELEVATION, THROTTLE, RUDDER, AILERON};

  
  struct ChannelProperties
  {
    ChannelTypes ChannelType;
    String ChannelName;   // vs char[2]? In future, when implemented, (Multi) Switch names will be identified as S1, S2, MS1, etc...
    VelocityTypes VelocityType;
    unsigned int ChannelIndex;   // TODO: Sort channels when modular
    unsigned int DefaultValue;   // What is the default value of the channel in case needed?
    unsigned int Value;
    int MinValue;
    int MaxValue;
    bool bIsTimeWatchDogOn;   // Disable reset if undesired.
    bool bHasBeenUpdated;     // Flag. If false it will reverse to default value.
    bool bIsChannelOnHold;    // Will override reset if true
      unsigned long UpdateTime;  // Used to measure after bHasBeenUpdated
      unsigned long UpdateIntervalDeadline;
  };
    ChannelProperties PPMChannels[CHANNEL_COUNT];
    
  char ValidVelocityNames[VELOCITY_COUNT];
    const int Velocity_Min = 0;
    const int Velocity_Max = 100;
  char ValidGearNames[SWITCH_COUNT];

};

#endif
