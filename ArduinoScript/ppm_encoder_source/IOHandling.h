#ifndef IOHandling
#define IOHandling

#include "Config.h"
#include "DroneSignal.h"
#include <Arduino.h>

class IOHandler
{
 public:
  IOHandler();
  void SerialMonitor(DSDroneSignal& UAV);
  int InputProcess(char* Message, DSDroneSignal &UAV);
  void UpdateCurrentTime();
  void SetInputTime();
  void OutputTimeoutCheck(DSDroneSignal& UAV);
  void GetMacroTranslation();
  inline char* GetMacroID(int Index) { return IOHandler::MacroList[Index].MacroID; };

  inline int GetInvalidCharsTolerance() const { return IOHandler::InvalidCharsTolerance; };
  void IncrementInvalidMessageCount();
  

 private:
  int InvalidCharsTolerance;     // Used to determine how many characters before a real message is too much
      int InvalidMessageCount;  // How many times have we had these tolerances surpassed.
      int InvalidMessagesTolerance; // Howm any of these before we start flushing, if flagged
        bool bClearOutputAtLimit;
  char SerialInput;
  unsigned long CurrentTime;
  unsigned long InputTime;
  struct MacroInterpretation
  {
    char MacroID[5];  // Include null termination
    char MacroToControls[SIGNAL_LENGTH + 1];    // Will be null terminated at end of final command, so may vary in length anyways.
  };
  MacroInterpretation MacroList[2 + EXT_MACRO_COUNT];

  int ChannelsOnHold[CHANNEL_COUNT];

  
};

#endif
