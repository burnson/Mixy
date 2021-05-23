/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_2E4391BF96F98D7A__
#define __JUCE_HEADER_2E4391BF96F98D7A__

//[Headers]     -- You can add your own extra header files here --
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
#include "JuceHeader.h"

void connectToServer(juce::String newServer);
void channels(int& in, int& out);
bool active(int in, int out);
float& assigned(int in, int out);
void pushSettings();
void resync();

class MatrixView : public Component
{
  public:
  static const int CellSize = 16;

  void mouseDown(const juce::MouseEvent& e)
  {
    int input = e.x / CellSize, output = e.y / CellSize;

    float& x = assigned(input, output);
    if(x > 0.0)
      x = 0;
    else
      x = 1.0;
    pushSettings();
  }

  void paint(juce::Graphics& g)
  {
    int Inputs, Outputs;
    channels(Inputs, Outputs);
    g.fillAll(juce::Colours::darkgrey);
    for(int i = 0; i < Inputs; i++)
    {
      float x1 = (float)(i * CellSize), x2 = x1;
      float y1 = 0, y2 = (float)(Outputs * CellSize);

      g.setColour(juce::Colours::grey);
      g.drawLine(x1, y1, x2, y2, 1.0f);
    }
    for(int i = 0; i < Outputs; i++)
    {
      float y1 = (float)(i * CellSize), y2 = y1;
      float x1 = 0, x2 = (float)(Inputs * CellSize);

      g.setColour(juce::Colours::grey);
      g.drawLine(x1, y1, x2, y2, 1.0f);
    }
    for(int i = 0; i < Inputs; i++)
    {
      for(int j = 0; j < Outputs; j++)
      {
        float x1 = (float)(i * CellSize), y1 = (float)(j * CellSize);
        float x2 = x1 + CellSize, y2 = y1 + CellSize;
        float w = x2 - x1, h = y2 - y1;

        bool ChannelAssigned = assigned(i, j) > 0.0;
        bool ChannelActive = active(i, j);

        if(ChannelAssigned)
        {
          if(ChannelActive)
            g.setColour(juce::Colours::greenyellow);
          else
            g.setColour(juce::Colours::lightsteelblue);
          g.fillEllipse(x1, y1, w, h);
        }
      }
    }
    if(getWidth() != Inputs * CellSize || getHeight() != Outputs * CellSize)
    {
      setSize(Inputs * CellSize, Outputs * CellSize);
    }
  }
};
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Content  : public Component,
                 public ComboBoxListener,
                 public ButtonListener
{
public:
    //==============================================================================
    Content ();
    ~Content();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
public:
    ScopedPointer<MatrixView> matrixView;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<GroupComponent> GroupServer;
    ScopedPointer<GroupComponent> GroupSettings;
    ScopedPointer<ComboBox> InputDevices;
    ScopedPointer<Label> LabelInputDevice;
    ScopedPointer<ComboBox> OutputDevices;
    ScopedPointer<Label> LabelOutputDevice;
    ScopedPointer<ComboBox> SampleRate;
    ScopedPointer<Label> LabelSampleRate;
    ScopedPointer<Label> LabelServerAddress;
    ScopedPointer<ComboBox> Server;
    ScopedPointer<Label> LabelInput;
    ScopedPointer<Label> LabelInput2;
    ScopedPointer<TextButton> textButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Content)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_2E4391BF96F98D7A__
