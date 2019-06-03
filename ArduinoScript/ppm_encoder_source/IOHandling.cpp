#include "IOHandling.h"

IOHandler::IOHandler()
{
  
  SerialInput = ' ';
  CurrentTime = millis();
  InputTime = CurrentTime;

  InvalidCharsTolerance = 10;     // Used to determine how many characters before a real message is too much
  InvalidMessageCount = 0;  // How many times have we had these tolerances surpassed.
  InvalidMessagesTolerance = 3; // Howm any of these before we start flushing, if flagged
  bClearOutputAtLimit = true;   // Should we clear the buffer if the messag tolerance has been overrridden.

  // Hardcode specific macros
    // Halt
  strncpy(IOHandler::MacroList[0].MacroID, "M000", 5);  // Size 5, null terminator
  strncpy(IOHandler::MacroList[0].MacroToControls, "E050T050R050A050H0", SIGNAL_LENGTH + 1 );   // + 1 due to  null termination
    // Depart
  strncpy(IOHandler::MacroList[1].MacroID, "M001", 5);
  strncpy(IOHandler::MacroList[1].MacroToControls, "E050T050R050A050H5000", SIGNAL_LENGTH  + 1);
};




void IOHandler::UpdateCurrentTime()
{
  IOHandler::CurrentTime = millis();
};

void IOHandler::SetInputTime()
{
  IOHandler::InputTime = millis();
};


/* OutputTimeoutCheck
 *  Will check if a timeout has occured, indicating a loss of signal.
 *  
 *  params:
 *    UAV: of class DSDroneSignal, since the timeout is specific to the UAV used.
 *    
 *  Dependent on the timeout constant RESET_INTERVAL. If RESET_INTERVAL is set to a negative value then this feature is considered disabled.
 * 
 */
void IOHandler::OutputTimeoutCheck(DSDroneSignal& UAV)
{
  if (RESET_INTERVAL < 0)
  {
    return;
  }
  else if ( (unsigned long)(IOHandler::CurrentTime - IOHandler::InputTime) >= RESET_INTERVAL )
  {
      unsigned int ChannelDefaultValue;
      for (int i = 0; i < CHANNEL_COUNT; i++)
      {
        UAV.ResetChannel(i);
      }
      Serial.println( F("[SYS00]") );    // Signal all channels has been reset due to lack of input to Arduino
      InputTime = millis();

      return;
  }
  else
  {
    
    return;
  }
  /*
   * Outcommented due to not deemd useful for current ideas.
  else    // See if each individual channel needs a reset
  {
    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
      if ( UAV.ShouldChannelBeReset(i) )
      {
        UAV.ResetChannel(i);
        
        String SerialOutput = "[SYS01: " + UAV.GetPPMChannelName(i) + ']';
            
        Serial.println(SerialOutput);
        // TODO: Show name of channel that has been reset
      }
    }
  }
  */
};

void IOHandler::SerialMonitor(DSDroneSignal& UAV)
{ 
  char InputMessage[SIGNAL_LENGTH + 1]; // minus the '<' and '>', null terminate in the end

  char InputBuffer;
  bool bHasMessageStarted = false;
  bool bIsMessageComplete = false;    // Has signal_length or '>' been reached
  bool bIsInputValid = true;
  int InvalidCharCount = 0;     // Count how many chars before a '<'.
  int MessageCharCount = 0;     // How many chars do we have in the actual message, between the '<' and '>'. Can't be longer than SIGNAL_LENGTH
  while ( (Serial.available() > 0 ) && !bIsMessageComplete  && bIsInputValid)
  {
    InputBuffer = Serial.read();

    
    // Check for '<' and '>'
    if (!bHasMessageStarted && ( InputBuffer == '<') )
    {
      bHasMessageStarted = true;
    }
    else if ( bHasMessageStarted && (InputBuffer == '>') )
    {
      bIsMessageComplete = true;
      InputMessage[MessageCharCount] = 0;    // Null terminate char array
    }
    else if (bHasMessageStarted)    // Read message as normally
    {
      InputMessage[MessageCharCount] = InputBuffer;
      MessageCharCount++;
    }
    else    // Message hasn't begun yet, but a character has been registered.
    {
      InvalidCharCount++;   
    }

    // Error handling
    if (!bIsMessageComplete && (MessageCharCount > SIGNAL_LENGTH) )
    {
      bIsInputValid = false;
      Serial.println(F("[EI:00]"));   // Message is too long for some reason
    }
    else if (InvalidCharCount > IOHandler::GetInvalidCharsTolerance())
    {
      bIsInputValid = false;
      Serial.println(F("[EI:01]"));   // No valid message within the tolerance
      InvalidCharCount = 0;
    }
      
    delay(10);
  }
  if (bIsMessageComplete)
  { 
    IOHandler::InputProcess(InputMessage, UAV);
    IOHandler::SetInputTime();  // Set input timestamp to new time
  }
  else if (bHasMessageStarted)
  {
    Serial.println( F("[EI:02]") );   // Message was started but never ended before serial buffer was emptied.
  }

  return;
};




/* InputProcess:
 *  Will process the received input. Two types of input, in following priority:
 *    Macros: Macros can contain several commands in one. 
 *            Due to this, only one macro will be accepted, 
 *            and they will performed until done or until a 
 *            specific stop macro has been issued, <M000>.
 *      Example: <M001>
 *     
 *    Control: The regular controls: velocities and switches.
 *      Example: <T050E077>
 * 
 */
int IOHandler::InputProcess(char* MessageTmp, DSDroneSignal &UAV)
{
  char Message[SIGNAL_LENGTH + 1];    // Work with this message instead
  
  strncpy(Message, MessageTmp, SIGNAL_LENGTH + 1 );
  bool bIsInputVelocity = false;
  bool bIsInputHoldCommand = false;
  bool bIsHoldCommandToggled = false;
  bool bIsInputSwitch = false;
  int ChannelIndex = 0;   // Simply set because of undefined behaviour when left uinitialized.
  
  bool bIsChannelModifiedToHold[CHANNEL_COUNT];   // Flag for each channel, in order of channel index
    int ChannelModIndex = 0;
  for (int i; i < CHANNEL_COUNT; i++)
  {
    bIsChannelModifiedToHold[i] = false;
  }
  
  int MessageLength = strlen(Message);
  int HoldDuration = 0;

  char ConfirmationMessage[100];    // Just make it a safe size.
  ConfirmationMessage[0] = 0;   // Null termination since strncat is \0-dependent
  
  strncat(ConfirmationMessage, "[", 2);     // strncat throughoutly used to concatenate output message for simplicity. Add extra count for null padding

  

  bool bIsMacroOverrideMacro = false;    // Can Macro override hold, such as M000?
  // See if control is macro, and if so, substitute macro with controls
  
  //Serial.println(toupper(Message[0]));
  //Serial.println(strlen(Message));
  
  if (toupper(Message[0]) == 'M' && strlen(Message) == 4)
  {
    char Macro[4];
    Macro[0] = 'M';
    // Get next three characters and see if numbers otherwise return error.
    for (int i = 1; i < 4; i++)
    {
      if( !isdigit(Message[i]) )
      {
        strncat(ConfirmationMessage, "EI:03", 5);    // ID | Invalid Macro ID.
        return 2;   // Return, expecting nothing after a macro.
      }
      else
      {
        Macro[i] = Message[i];
      }


    }

    // Make sure macro ID actually exist
    for (int i = 0; i < sizeof(IOHandler::MacroList); i++)
    {
      // strcmp returns 0 if exact match.
      if( strncmp(IOHandler::GetMacroID(i), Macro, 4) == 0)
      {
        
        // In case of "M000", it  should be able to override anything
        if( strncmp("M000", Macro, 4) == 0)
        {
          bIsMacroOverrideMacro = true;
        }
        
        strncpy(Message, IOHandler::MacroList[i].MacroToControls, SIGNAL_LENGTH + 1 );
        MessageLength = strlen(Message);    // Message changed, find new length
      }
    }
  } 


  
  if ( UAV.IsSignalOnHold() && !bIsMacroOverrideMacro )
  {
    Serial.println( F("[ONHOLD]") );
    return 0;   // Not an error par se.
  }
  

  //Cycle through all characters
  int i = 0;
  beginning:    // goto in case of EI:3
  for (i; i < MessageLength; i++)
  {
    
    //Serial.println(i); // DEBUG: Doesn't work without for whatever reason;
    
    DSDroneSignal::ChannelTypes ChannelType;    // Check which kind of control this is
    // See if it is a control command
    for (int j = 0; j < CHANNEL_COUNT; j++)
    {
      // See if control exists
      if ( toupper(Message[i]) == UAV.GetPPMChannelName(j)[0] )
      {
        ChannelIndex = j;
        if (UAV.GetChannelType(ChannelIndex) == UAV.VELOCITY)
        {
          bIsInputVelocity = true;

        }
        break;  // Stop searching for a name, already found a match.
      }
      
    }
    
    if ( (toupper(Message[i]) == 'H') && !bIsInputVelocity ) // See if it is a hold command
    {
      // process the hold command.
      bIsInputHoldCommand = true;
      bIsHoldCommandToggled = true;

      char HoldDurationAsChar[5]; HoldDurationAsChar[5] = 0;
      int CharIndex = 0;
      int j = i + 1;    // Must be kept outside of loop to reset i
      do  // Go through the next characters until they stop being numbers.
      {
        
        if ( isdigit(Message[j]) )
        {
          HoldDurationAsChar[CharIndex] = Message[j];
          CharIndex++; j++;
        }
        else
        {
          break;
        }
      } while (CharIndex < 5);  // Set a hard limit of 4 characters, a max of 9999ms holding the signal ~10s. Could be removed, but still.
      i = j - 1;      // Set starting point of message to after value of previous velocity.
      if (CharIndex > 0)  // If the CharIndex is larger than zero then at least one number has been registered
      {
        HoldDuration = atoi(HoldDurationAsChar);
      }
    
      

    }
    if (bIsInputVelocity)
    {
      char VelocityRateAsChar[4]; 
      VelocityRateAsChar[4] = 0;    // Null Terminate
      int CharIndex = 0;
      
      // Look at three next values and see if numbers
      int j;    // Must be kept outside of loop to reset i
      for (j = i + 1; j < i + 4; j++)
      {
        
        if ( isdigit(Message[j]) )
        {
          
          VelocityRateAsChar[CharIndex] = Message[j];
          CharIndex++;
        }
        else
        {
          strncat(ConfirmationMessage, "EI:04", 5);   // Not at valid number
          i = j;    // Set starting point to errornous message line, in case this is a real command.
          bIsInputVelocity = false;
          goto beginning;
        }
      }
      i = j - 1;      // Set starting point of message to after value of previous velocity

      int VelocityRateAsInt = atoi(VelocityRateAsChar);
      

      // map value from 0-100 to 988-2012 and assign to channel.
      if ( (VelocityRateAsInt >= UAV.GetPPMMinValue() ) && (VelocityRateAsInt <= UAV.GetPPMMaxValue() ) )
      {
        
        const int NewFloorValue = UAV.GetChannelMinValue(ChannelIndex) - UAV.GetVelocityMappingCalibration();
        const int NewCeilValue = UAV.GetChannelMaxValue(ChannelIndex) + UAV.GetVelocityMappingCalibration();
        const int NewVelocity = map(VelocityRateAsInt, 0, 100, NewFloorValue , NewCeilValue );
        UAV.ChangeVelocity(ChannelIndex, NewVelocity);

        // Confirm in message
        strncat(ConfirmationMessage, UAV.GetPPMChannelName(ChannelIndex).c_str(), 1);
        strncat(ConfirmationMessage, VelocityRateAsChar, 3);

        // Set channel as modified
        bIsChannelModifiedToHold[ChannelIndex] = true;
          
          
      }
      else
      {  
          strncat(ConfirmationMessage, "EI:05", 5);   // Value outside valid range.
      }
      bIsInputVelocity = false;

      //continue;
    }
    else if (bIsInputHoldCommand)
    {
      bIsInputHoldCommand = false;
      continue;
    }
    else
    {
      strncat(ConfirmationMessage, "EI:06" , 5);  // If command is not recognized
    }

  }


  if (bIsHoldCommandToggled)
  {
    if (HoldDuration > 0)
    {
      UAV.SetHoldSignal(true, HoldDuration);
      strncat(ConfirmationMessage, "H", 1);
      char HoldDurationAsChar[5]; 
      sprintf(HoldDurationAsChar, "%d", HoldDuration);    // Convert from int to char*
      strncat(ConfirmationMessage, HoldDurationAsChar, 4);
    }
    else
    {
      UAV.SetHoldSignal(false, HoldDuration);
      for (int i; i < CHANNEL_COUNT; i++)
      {
          UAV.ToggleChannelOnHold(i ,false);
      }
      
      strncat(ConfirmationMessage, "H0", 2 );
    }

  }

  strncat(ConfirmationMessage, "]", 2);

  Serial.println(ConfirmationMessage);
  
  return 0;
  
};
