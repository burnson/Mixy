/*
  This file is part of Mixy, copyright 2013 Andi Brae.
  
  Mixy is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  Mixy is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Mixy.  If not, see <http://www.gnu.org/licenses/>.
*/

#define DONT_SET_USING_JUCE_NAMESPACE 1
#define JUCE_DONT_DEFINE_MACROS 1
#include "../JUCELibraryCode/JuceHeader.h"

//-------//
//Logging//
//-------//

juce::FileLogger* logFile = 0;
int instanceID = 0;

void log(const juce::String& s)
{
  if(!logFile) return;
  juce::String t = juce::Time::getCurrentTime().toString(true, true);
  t << " (" << instanceID << "): " << s;
  logFile->logMessage(t);
}

//-----//
//State//
//-----//

static const int MaximumChannels = 64;
static const int MaximumBufferSize = 4096;

juce::CriticalSection StateLock;
juce::StringArray StateInputDevices;
juce::StringArray StateOutputDevices;
float StateIOMatrix[MaximumChannels][MaximumChannels];
juce::String StateSettings;

//-----------//
//Audio Mixer//
//-----------//

class AudioMixer : public juce::AudioIODeviceCallback
{

  float InputOutputMatrixUser[MaximumChannels][MaximumChannels];
  float InputOutputMatrixIncoming[MaximumChannels][MaximumChannels];
  float InputOutputMatrixCallbackCopy[MaximumChannels][MaximumChannels];
  float Buffer[MaximumChannels][MaximumBufferSize];
  
  enum ChannelState
  {
    NoSignal,
    Begin,
    End,
    Signal
  };
  
  ChannelState InputChannelStatePrevPrev[MaximumChannels];
  ChannelState InputChannelStatePrevious[MaximumChannels];
  ChannelState InputChannelStateCurrent[MaximumChannels];
  
  bool InputActiveUser[MaximumChannels];
  bool InputActiveCallbackCopy[MaximumChannels];
  
  float Ramp[MaximumBufferSize];
  int CurrentFrame;
  
  juce::CriticalSection InputOutputMatrixCopyLock;
  
  public:
  
  void Clear()
  {
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    memset(&InputOutputMatrixUser[0][0], 0, MaximumChannels * MaximumChannels *
      sizeof(float));
  }
  
  void Set(int InputChannel, int OutputChannel, float NewValue)
  {
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    InputOutputMatrixUser[InputChannel][OutputChannel] = NewValue;
  }
  
  float Get(int InputChannel, int OutputChannel)
  {
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    return InputOutputMatrixUser[InputChannel][OutputChannel];
  }
  
  bool Active(int InputChannel)
  {
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    return InputActiveUser[InputChannel];  
  }
  
  AudioMixer()
  {
    ClearAll();
  }
  
  virtual ~AudioMixer() {}
  
  virtual void audioDeviceAboutToStart(juce::AudioIODevice* device)
  {
    if(!device)
    {
      log("Error: No audio device to start.");
      return;
    }
    
    //Precalculate the ramp function.
    for(int i = 0, n = device->getCurrentBufferSizeSamples(); i < n; i++)
      Ramp[i] = ((float)i) / ((float)n);
  }

  private:
  
  void ClearAll()
  {
    CurrentFrame = 0;
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    memset(&InputChannelStatePrevPrev[0], 0,
      MaximumChannels * sizeof(ChannelState));
    memset(&InputChannelStatePrevious[0], 0,
      MaximumChannels * sizeof(ChannelState));
    memset(&InputChannelStateCurrent[0], 0,
      MaximumChannels * sizeof(ChannelState));
    memset(&Buffer[0][0], 0,
      MaximumBufferSize * MaximumChannels * sizeof(float));
    memset(&Ramp[0], 0, MaximumBufferSize * sizeof(float));
    memset(&InputActiveUser[0], 0, MaximumChannels * sizeof(bool));
    memset(&InputActiveCallbackCopy[0], 0, MaximumChannels * sizeof(bool));
    memset(&InputOutputMatrixUser[0][0], 0,
      MaximumChannels * MaximumChannels * sizeof(float));
    memset(&InputOutputMatrixIncoming[0][0], 0,
      MaximumChannels * MaximumChannels * sizeof(float));
    memset(&InputOutputMatrixCallbackCopy[0][0], 0,
      MaximumChannels * MaximumChannels * sizeof(float));
  }
  
  void CopySettings()
  {
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    memcpy(&InputOutputMatrixIncoming[0][0], &InputOutputMatrixUser[0][0],
      MaximumChannels * MaximumChannels * sizeof(float));
  }
  
  ///Performs a fast and reliable check to see whether the channel has signal.
  static ChannelState GetChannelState(const float* InputChannel, int Samples)
  {
    bool Left = InputChannel[0] != 0.0f || InputChannel[1] != 0.0f;
    bool Right = InputChannel[Samples - 2] != 0.0f ||
      InputChannel[Samples - 1] != 0.0f;
      
    if(Left && Right)
      return Signal;
    else if(!Left && !Right)
      return NoSignal;
    else if(Left)
      return End;
    else if(Right)
      return Begin;
      
    return NoSignal;
  }
  
  ///Mixes the audio.
  virtual void audioDeviceIOCallback(const float** inputChannelData,
    int numInputChannels, float** outputChannelData, int numOutputChannels,
    int numSamples)
  {
    //Immediately clear all the output channels.
    for(int i = 0, n = sizeof(float) * numSamples; i < numOutputChannels; i++)
      memset(outputChannelData[i], 0, n);

    //Keep track of the current frame for debugging.
    std::cout << CurrentFrame++ << ": " << numSamples << " x " <<
      numInputChannels << "/" << numOutputChannels << " :";
    
    //Get the latest settings.
    juce::ScopedLock l(InputOutputMatrixCopyLock);
    CopySettings();
    
    //Get the channel states.
    for(int i = 0; i < numInputChannels; i++)
    {
      InputChannelStateCurrent[i] =
        GetChannelState(inputChannelData[i], numSamples);
      //Slow-start
      if(CurrentFrame < 30)
        InputChannelStateCurrent[i] = NoSignal;

      if(InputChannelStatePrevious[i] == NoSignal &&
        InputChannelStateCurrent[i] == Signal)
          InputChannelStateCurrent[i] = Begin;
      InputActiveCallbackCopy[i] = InputChannelStateCurrent[i] == Signal;
    }
    
    //Loop through each output channel.
    for(int i = 0; i < numOutputChannels; i++)
    {
      float* Output = outputChannelData[i];
      
      //Loop through each input channel stepping by the output channels.
      for(int j = 0; j < numInputChannels; j++)
      {
        ChannelState PrevPrevState = InputChannelStatePrevPrev[j];
        ChannelState PreviousState = InputChannelStatePrevious[j];
        ChannelState CurrentState = InputChannelStateCurrent[j];
        
        std::cout << " " << (int)PrevPrevState << ":" << (int)PreviousState <<
          ":" << (int)CurrentState;
        
        //No signal so do not spend time mixing.
        if(PreviousState == NoSignal && CurrentState == NoSignal)
          continue;
        
        float OldVolume = InputOutputMatrixCallbackCopy[j][i];
        float NewVolume = InputOutputMatrixIncoming[j][i];
        
        //Keep leading signal hushed until first full frame.
        if(PreviousState == Begin || PreviousState == NoSignal)
          OldVolume = NewVolume = 0.0;
        
        //Ramp the buffer in.
        if(PrevPrevState == Begin)
          OldVolume = 0.0;
        
        //Ramp out.
        if(CurrentState == End)
          NewVolume = 0.0;
        
        //Keep trailing signal hushed after lead out.
        if(PreviousState == End)
          OldVolume = NewVolume = 0.0;
        
        if(OldVolume == 0.0 && NewVolume == 0.0)
          continue;
          
        std::cout << "|" << OldVolume << "|" << NewVolume;
        std::cout << " " << j;
        
        /*Mix the input signal into the output. Ramp from the old volume to the
        new volume to essentially guarantee that there will never be any clicks
        when changing the mixing state.*/
        const float* Input = Buffer[j];
        float Delta = NewVolume - OldVolume;
        
        for(int k = 0; k < numSamples; k++)
          Output[k] += Input[k] * ((Ramp[k] * Delta) + OldVolume);
      }
    }
    
    {
      memcpy(&InputOutputMatrixCallbackCopy[0][0],
        &InputOutputMatrixIncoming[0][0], MaximumChannels * MaximumChannels *
        sizeof(float));
      memcpy(&InputActiveUser[0], &InputActiveCallbackCopy[0],
        MaximumChannels * sizeof(bool));
    }
    memcpy(&InputChannelStatePrevPrev[0], &InputChannelStatePrevious[0],
      MaximumChannels * sizeof(ChannelState));
    memcpy(&InputChannelStatePrevious[0], &InputChannelStateCurrent[0],
      MaximumChannels * sizeof(ChannelState));
    for(int i = 0, n = sizeof(float) * numSamples; i < numInputChannels; i++)
      memcpy(&Buffer[i][0], inputChannelData[i], n);

    std::cout << std::endl;
  }
  
  virtual void audioDeviceStopped() {}

  virtual void audioDeviceError(const juce::String &errorMessage)
  {
    log(juce::String("Audio Device Error: ") + errorMessage);
    log("Will try to reboot server in 15 seconds.");
    juce::Thread::sleep(15000);
    juce::JUCEApplication::quit();
  }
};

//-------------//
//Audio Manager//
//-------------//

struct AudioManager
{
  juce::AudioDeviceManager Devices;
  
  juce::AudioIODevice* InOut;
  
  int Inputs, Outputs;
  
  AudioMixer Mixer;
  
  AudioManager() : InOut(0), Inputs(0), Outputs(0) {}
  
  ~AudioManager()
  {
    //delete InOut;
  }
  
  bool Initialize(juce::String settingsData = "")
  {
    juce::File f("/Users/Shared/Mixy.settings");
    juce::StringArray a;
    if(settingsData == "")
      settingsData = f.loadFileAsString();
    a.addTokens(settingsData, "|", juce::String::empty);
    const juce::OwnedArray<juce::AudioIODeviceType>& DeviceList =
      Devices.getAvailableDeviceTypes();
    
    for(int i = 0; i < DeviceList.size(); i++)
    {      
      if(DeviceList[i]->getTypeName() == "CoreAudio")
      {
        {
          juce::String s;
          s << "Device Name: " << DeviceList[i]->getTypeName() << juce::newLine;
          juce::StringArray InDeviceNames = DeviceList[i]->getDeviceNames(true);
          s << "In:" << juce::newLine;
          for(int j = 0; j < InDeviceNames.size(); j++)
            s << " * " << InDeviceNames[j] << juce::newLine;
          juce::StringArray OutDeviceNames =
            DeviceList[i]->getDeviceNames(false);
          s << "Out:" << juce::newLine;
          for(int j = 0; j < OutDeviceNames.size(); j++)
            s << " * " << OutDeviceNames[j] << juce::newLine;
          {
            juce::ScopedLock scopedLock(StateLock);
            StateInputDevices = InDeviceNames;
            StateOutputDevices = OutDeviceNames;
          }
          log(s);
        }

        juce::String s;
        DeviceList[i]->scanForDevices();
        juce::AudioDeviceManager::AudioDeviceSetup ads;
        ads.inputDeviceName = a[0];
        ads.outputDeviceName = a[1];
        ads.sampleRate = (float)a[2].getIntValue();
        log(juce::String("Input: ") + a[0]);
        log(juce::String("Output: ") + a[1]);
        log(juce::String("Sample Rate: ") + a[2]);
        InOut = DeviceList[i]->createDevice(ads.outputDeviceName,
          ads.inputDeviceName);
        if(!InOut)
        {
          s << "Could not open input and output devices." << juce::newLine;
          {
            juce::ScopedLock scopedLock(StateLock);
            StateSettings = "";
          }
          log(s);
          return false;
        }
        int InChans = InOut->getInputChannelNames().size();
        int OutChans = InOut->getOutputChannelNames().size();
        if(!InChans || !OutChans)
        {
          log("Input or output device has no channels or is not available.");
          return false;
        }
        s << InChans << " " << OutChans << juce::newLine;
        ads.useDefaultInputChannels = false;
        ads.useDefaultOutputChannels = false;
        ads.inputChannels.setRange(0, InChans, true);
        ads.outputChannels.setRange(0, OutChans, true);
        ads.bufferSize = InOut->getBufferSizeSamples(
          InOut->getNumBufferSizesAvailable() - 1);
        delete InOut;
        InOut = 0;
        juce::String t =
          Devices.initialise(InChans, OutChans, 0, false, juce::String::empty,
          &ads);
        //juce::String s = Devices.setAudioDeviceSetup(ads, true);
        if(t != juce::String::empty)
        {
          s << t << juce::newLine;
          log(s);
          {
            juce::ScopedLock scopedLock(StateLock);
            StateSettings = "";
          }
         return false;
        }
        InOut = Devices.getCurrentAudioDevice();
        if(!InOut)
          return false;
        log(s);
        
        {
          juce::ScopedLock scopedLock(StateLock);
          StateSettings = "";
          StateSettings << ads.inputDeviceName << "|" << InChans << "|" <<
            ads.outputDeviceName << "|" << OutChans << "|" <<
            ads.sampleRate;
          Inputs = InChans;
          Outputs = OutChans;
        }
      }
    }
    
    Devices.addAudioCallback(&Mixer);
    
    for(int i = 5; i < a.size(); i += 3)
      Mixer.Set(a[i - 2].getIntValue(), a[i - 1].getIntValue(),
        a[i].getFloatValue());
    
    return true;
  }
};

//----------------//
//Socket Messenger//
//----------------//

//handshake|message
//getInputDevices
//inputDevices|...
//getOutputDevices
//outputDevices|...
//getState
//state|in|inchan|out|outchan|sr|...(in|out|volume|active)
//setSettings|in|out|sr|...(in|out|volume)

class SocketMessenger : public juce::InterprocessConnection
{  
public:
  void connectionMade() {log("Socket connection made");}
  void connectionLost() {log("Socket connection lost");}
  
  void send(const juce::String& s)
  {
    log(juce::String("Sending message: ") + s);
    sendMessage(stringToBlock(s));
  }
  
  void received(const juce::String& s);

  void messageReceived(const juce::MemoryBlock& message)
  {
    juce::String m = message.toString();
    received(m);
  }
  
  juce::MemoryBlock stringToBlock(const juce::String& s)
  {
    return juce::MemoryBlock(s.toRawUTF8(),
      juce::CharPointer_UTF8::getBytesRequiredFor(s.getCharPointer()));
  }
    
  SocketMessenger() {log("Creating interprocess connection socket");}
  ~SocketMessenger() {}
};

//-------------//
//Socket Server//
//-------------//

class SocketServer : public juce::InterprocessConnectionServer
{
public:
  juce::InterprocessConnection* createConnectionObject()
  {
    return new SocketMessenger;
  }
  
  SocketServer() {beginWaitingForSocket(54321);}
  ~SocketServer() {stop();}
};

//-----------//
//Application//
//-----------//

class Application;
Application* app;

class Application : public juce::JUCEApplication
{
  public:
  
  AudioManager* audioManager;
  SocketServer* socketServer;
  
  //Initializes the app.
  void initialise(const juce::String& commandLine)
  {
    instanceID = juce::Random::getSystemRandom().nextInt(8999) + 1000;
    app = this;
    logFile = new juce::FileLogger(
      juce::File("/Users/Shared/Mixy.log"), "=======Mixy=======");
    
    log("Starting socket server");
    socketServer = new SocketServer;
    log("Starting audio manager");
    audioManager = new AudioManager;
    log("Initializing audio manager");
    bool Result = audioManager->Initialize();
    if(!Result)
    {
      log("Server could not be initialized with original settings.");
      log("Will try to reboot server in 15 seconds.");
      juce::Thread::sleep(15000);
      juce::JUCEApplication::quit();
    }
    log("Finished starting up");
  }
  
  //Shuts down the app.
  void shutdown()
  {
    log("Closing normally");
  }
  
  //Initialize window manager.
  Application() : audioManager(0), socketServer(0) {}
  
  //Quits the app.
  void systemRequestedQuit() {quit();}
  
  //Returns the name of the application as it pertains to JUCE.
  const juce::String getApplicationName() {return "MixyServer";}
  
  //Returns the version of the application as it pertains to JUCE.
  const juce::String getApplicationVersion()
  {
    return ProjectInfo::versionString;
  }
  
  //Disable multiple-instance checker on Mac.
  bool moreThanOneInstanceAllowed() {return true;}
  
}; START_JUCE_APPLICATION(Application)

void SocketMessenger::received(const juce::String& s)
{
  log(juce::String("Received message: ") + s);
  juce::StringArray a;
  a.addTokens(s, "|", "");
  if(a.size() < 1)
  {
    log("Warning: received empty message");
    return;
  }
  if(a[0] == "handshake")
    send("handshake|This is a test reply from the server.");
  else if(a[0] == "getInputDevices")
  {
    juce::String m = "inputDevices";
    {
      juce::ScopedLock scopedLock(StateLock);
      for(int i = 0; i < StateInputDevices.size(); i++)
        m << "|" << StateInputDevices[i];
    }
    send(m);
  }
  else if(a[0] == "getOutputDevices")
  {
    juce::String m = "outputDevices";
    {
      juce::ScopedLock scopedLock(StateLock);
      for(int i = 0; i < StateOutputDevices.size(); i++)
        m << "|" << StateOutputDevices[i];
    }
    send(m);
  }
  else if(a[0] == "terminate")
  {
    log("Received message to terminate. Server will be rebooted.");
    log("Going down now!");
    juce::JUCEApplication::quit();
  }
  else if(a[0] == "getState")
  {
    if(!app->audioManager)
      return;
    juce::String s;
    int Inputs = 0, Outputs = 0;
    {
      juce::ScopedLock scopedLock(StateLock);
      s << "state|" << StateSettings;
      Inputs = app->audioManager->Inputs;
      Outputs = app->audioManager->Outputs;
    }
    for(int i = 0; i < Inputs; i++)
    {
      for(int j = 0; j < Outputs; j++)
      {
        s << "|" << i << "|" << j << "|" << app->audioManager->Mixer.Get(i, j)
          << "|" << (app->audioManager->Mixer.Active(i) ? "1" : "0");
      }
    }
    send(s);
  }
  else if(a[0] == "setSettings")
  {
    if(!app->audioManager)
      return;
    //setSettings|in|out|sr|...
    if(a.size() < 4)
    {
      log("Warning: received incomplete settings");
      return;
    }
    
    juce::String d;
    bool changingDevices = false;
    d << a[1] << "|" << app->audioManager->Inputs << "|" <<
      a[2] << "|" << app->audioManager->Outputs << "|" << a[3];
    log(juce::String("Incoming settings: ") + d);
    {
      juce::ScopedLock scopedLock(StateLock);
      log(juce::String("Original settings: ") + StateSettings);
      if(d != StateSettings)
        changingDevices = true;
    }
    
    if(!changingDevices)
    {
      log("Updating matrix settings");
      app->audioManager->Mixer.Clear();
      for(int i = 6; i < a.size(); i += 3)
      {
        app->audioManager->Mixer.Set(a[i - 2].getIntValue(),
          a[i - 1].getIntValue(), a[i].getFloatValue());
      }
    }
    {
      log("Updating Mixy settings");
      juce::String f;
      for(int i = 1; i < a.size(); i++)
      {
        if(i > 1) f << "|";
        f << a[i];
      }
      
      log("Restarting MixyServer");
      delete app->audioManager;
      app->audioManager = new AudioManager;
      bool Result = app->audioManager->Initialize(f);
      
      if(Result)
      {
        juce::File fi("/Users/Shared/Mixy.settings");
        fi.replaceWithText(f);
      }
      else
      {
        log("Invalid settings. Will not save new settings.");
        delete app->audioManager;
        app->audioManager = new AudioManager;
        bool Result = app->audioManager->Initialize();
        if(!Result)
        {
          log("Server could not be initialized with original settings.");
          log("Will try to reboot server in 15 seconds.");
          juce::Thread::sleep(15000);
          juce::JUCEApplication::quit();
        }
      }
    }
  }
}
