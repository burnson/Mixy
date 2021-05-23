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

//#define DONT_SET_USING_JUCE_NAMESPACE 1
#define JUCE_DONT_DEFINE_MACROS 1
#include "JuceHeader.h"

int Ins = 1, Outs = 1;
bool Active[128][128];
float Assigned[128][128];
juce::String currentServer;
void connectToServer(juce::String newServer);
void channels(int& in, int& out);
bool active(int i, int out);
float& assigned(int i, int out);
void pushSettings();
void resync();

#include "MixyWindow.h"

//------//
//Window//
//------//
class Window : public juce::DocumentWindow
{
public:
  
  ScopedPointer<Content> contentComponent;
  
  //Constructor to initialize document handler.
  Window() : juce::DocumentWindow(
    juce::JUCEApplication::getInstance()->getApplicationName(),
    juce::Colours::lightgrey, juce::DocumentWindow::allButtons, true)
  {
    contentComponent = new Content;
    centreWithSize(450, 216);
    setResizable(false, false);
    setUsingNativeTitleBar(true);
#ifdef JUCE_MAC
    setDropShadowEnabled(true);
#else
    setDropShadowEnabled(false);
#endif
    setVisible(true);
    setContentOwned(contentComponent, true);
  }
  
  //Closes the application.
  void closeButtonPressed()
  {
    juce::JUCEApplication::quit();
  }
};

//----------------//
//Socket Messenger//
//----------------//

class SocketMessenger : public juce::InterprocessConnection
{
  bool hasHandshake;
  
public:
  bool handshaked()
  {
    return hasHandshake;
  }
  
  void connectionMade()
  {
    std::cout << "Connection made" << std::endl;
  }
  
  void connectionLost()
  {
    std::cout << "Connection lost" << std::endl;
  }

  void send(const juce::String& s)
  {
    std::cout << "Sending: " << s << std::endl;
    sendMessage(stringToBlock(s));
  }
  
  void received(const juce::String& s);
  
  void unestablish();
  
  void establish(juce::String address)
  {
    currentServer = address;
    unestablish();
    if(address == "localhost")
      address = "127.0.0.1";
    std::cout << "Connecting to interprocess connection socket" << std::endl;
    if(!connectToSocket(address, 54321, 1000))
      std::cerr << "Error: pipe connection failed" << std::endl;
    send("handshake|This is a test socket message.");
  }
  
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
  
  SocketMessenger() : hasHandshake(false) {}
  ~SocketMessenger() {unestablish();}
};

//-----------//
//Application//
//-----------//
class Application;
Application* app;

class Application : public juce::JUCEApplication, public juce::Timer
{
public:
  ScopedPointer<SocketMessenger> socketMessenger;
  ScopedPointer<Window> window;
  
  //Creates a new window on startup.
  void initialise(const juce::String& commandLine)
  {
    app = this;
    socketMessenger = new SocketMessenger;
    window = new Window;
    window->contentComponent->Server->setSelectedItemIndex(0);
    startTimer(100);
  }
  
  ///Shuts down the app.
  void shutdown()
  {
    stopTimer();
    socketMessenger = nullptr;
    window = nullptr;
  }

  //Refresh the state.
  void timerCallback()
  {
    if(socketMessenger)
      socketMessenger->send("getState");
  }
  
  //Constructs the application instance.
  Application() {}

  //Quits the application.
  void systemRequestedQuit() {quit();}
  
  ///Returns the name of the application as it pertains to JUCE.
  const juce::String getApplicationName() {return "Mixy";}
  
  ///Returns the version of the application as it pertains to JUCE.
  const juce::String getApplicationVersion()
  {
    return ProjectInfo::versionString;
  }
  
  ///Disable multiple-instance checker on Mac.
  bool moreThanOneInstanceAllowed() {return false;}
  
}; START_JUCE_APPLICATION(Application)

void SocketMessenger::received(const juce::String& s)
{
  if(!app->window || !app->window->contentComponent)
    return;
  
  std::cout << "Received: " << s << std::endl;
  juce::StringArray a;
  a.addTokens(s, "|", "");
  if(a.size() < 1)
  {
    std::cout << "Warning: received empty message" << std::endl;
    return;
  }
  if(a[0] == "handshake")
  {
    hasHandshake = true;
    std::cout << "Handshake received" << std::endl;
    
    //Get the input and output devices
    send("getInputDevices");
    send("getOutputDevices");
    send("getState");
  }
  else if(a[0] == "inputDevices")
  {
    a.remove(0);
    app->window->contentComponent->InputDevices->clear(true);
    app->window->contentComponent->InputDevices->addItemList(a, 100);
  }
  else if(a[0] == "outputDevices")
  {
    a.remove(0);
    app->window->contentComponent->OutputDevices->clear(true);
    app->window->contentComponent->OutputDevices->addItemList(a, 100);
  }
  else if(a[0] == "state")
  {
    if(a.size() < 6)
    {
      std::cout << "Warning: small state received" << std::endl;
      return;
    }
    juce::String In = a[1], InChans = a[2], Out = a[3], OutChans = a[4],
      SampleRate = a[5];
    
    Ins = InChans.getIntValue();
    Outs = OutChans.getIntValue();
    
    juce::ComboBox* inputDevices = app->window->contentComponent->InputDevices;
    juce::ComboBox* outputDevices =
      app->window->contentComponent->OutputDevices;
    juce::ComboBox* sampleRate =
    app->window->contentComponent->SampleRate;
    
    for(int i = 0; i < inputDevices->getNumItems(); i++)
      if(inputDevices->getItemText(i) == In)
        inputDevices->setSelectedItemIndex(i, true);
    for(int i = 0; i < outputDevices->getNumItems(); i++)
      if(outputDevices->getItemText(i) == Out)
        outputDevices->setSelectedItemIndex(i, true);
    for(int i = 0; i < sampleRate->getNumItems(); i++)
      if(sampleRate->getItemText(i) == SampleRate)
        sampleRate->setSelectedItemIndex(i, true);
    memset(Active, sizeof(Active), 0);
    memset(Assigned, sizeof(Assigned), 0);
    for(int i = 9; i < a.size(); i += 4)
    {
      int in = a[i - 3].getIntValue(), out = a[i - 2].getIntValue();
      Assigned[in][out] = a[i - 1].getFloatValue();
      Active[in][out] = (a[i].getIntValue() != 0);
    }

    app->window->contentComponent->matrixView->repaint();

    {
      juce::Component* mv = app->window->contentComponent->matrixView;
      juce::Component* window = app->window;
      int w = mv->getRight() + 20;
      if(w < 450) w = 450;
      int h = mv->getBottom() + 20;
      if(h < 250) h = 250;
      
      if(window->getWidth() != w || window->getHeight() != h)
      {
        window->centreWithSize(w, h);
      }
    }
    
    //state|in|inchan|out|outchan|sr|...
  }
}

void SocketMessenger::unestablish()
{
  disconnect();
  hasHandshake = false;
  if(app->window)
  {
    app->window->contentComponent->InputDevices->clear(true);
    app->window->contentComponent->InputDevices->setSelectedItemIndex(-1);
    app->window->contentComponent->OutputDevices->clear(true);
    app->window->contentComponent->OutputDevices->setSelectedItemIndex(-1);
    app->window->contentComponent->SampleRate->setSelectedItemIndex(-1);
  }
}

void pushSettings()
{
  if(!app->window || !app->socketMessenger) return;
  juce::String s = "setSettings";
  if(app->window->contentComponent->InputDevices->getText() == ""
    || app->window->contentComponent->OutputDevices->getText() == ""
    || app->window->contentComponent->SampleRate->getText() == "")
  {
    return;
  }
  s << "|" << app->window->contentComponent->InputDevices->getText();
  s << "|" << app->window->contentComponent->OutputDevices->getText();
  s << "|" << app->window->contentComponent->SampleRate->getText();
  std::cout << "Ins: " << Ins << " Outs: " << Outs << std::endl;
  for(int i = 0; i < Ins; i++)
  {
    for(int j = 0; j < Outs; j++)
    {
      if(Assigned[i][j] > 0.0)
      {
        s << "|" << i << "|" << j << "|" << Assigned[i][j];
      }
    }
  }
  app->socketMessenger->send(s);
}

void resync()
{
  app->socketMessenger->send("terminate");
  juce::Thread::sleep(5000);
  connectToServer(currentServer);
}    

void connectToServer(juce::String newServer)
{
  app->socketMessenger->establish(newServer);
}

void channels(int& in, int& out)
{
  in = Ins;
  out = Outs;
}

bool active(int in, int out)
{
  return Active[in][out];
}

float& assigned(int in, int out)
{
  return Assigned[in][out];
}
