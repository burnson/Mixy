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

//[Headers] You can add your own extra header files here...
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
//[/Headers]

#include "MixyWindow.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Content::Content ()
    : Component ("Content")
{
    addAndMakeVisible (GroupServer = new GroupComponent ("GroupServer",
                                                         "Server"));

    addAndMakeVisible (GroupSettings = new GroupComponent ("GroupSettings",
                                                           "Settings"));

    addAndMakeVisible (InputDevices = new ComboBox ("InputDevices"));
    InputDevices->setEditableText (false);
    InputDevices->setJustificationType (Justification::centredLeft);
    InputDevices->setTextWhenNothingSelected ("(no server)");
    InputDevices->setTextWhenNoChoicesAvailable ("(no server)");
    InputDevices->addListener (this);

    addAndMakeVisible (LabelInputDevice = new Label ("LabelInputDevice",
                                                     "Input Device:"));
    LabelInputDevice->setFont (Font (15.00f, Font::plain));
    LabelInputDevice->setJustificationType (Justification::centredRight);
    LabelInputDevice->setEditable (false, false, false);
    LabelInputDevice->setColour (TextEditor::textColourId, Colours::black);
    LabelInputDevice->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (OutputDevices = new ComboBox ("OutputDevices"));
    OutputDevices->setEditableText (false);
    OutputDevices->setJustificationType (Justification::centredLeft);
    OutputDevices->setTextWhenNothingSelected ("(no server)");
    OutputDevices->setTextWhenNoChoicesAvailable ("(no server)");
    OutputDevices->addListener (this);

    addAndMakeVisible (LabelOutputDevice = new Label ("LabelOutputDevice",
                                                      "Output Device:"));
    LabelOutputDevice->setFont (Font (15.00f, Font::plain));
    LabelOutputDevice->setJustificationType (Justification::centredRight);
    LabelOutputDevice->setEditable (false, false, false);
    LabelOutputDevice->setColour (TextEditor::textColourId, Colours::black);
    LabelOutputDevice->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (SampleRate = new ComboBox ("SampleRate"));
    SampleRate->setEditableText (false);
    SampleRate->setJustificationType (Justification::centredLeft);
    SampleRate->setTextWhenNothingSelected ("(no server)");
    SampleRate->setTextWhenNoChoicesAvailable ("(no choices)");
    SampleRate->addItem ("44100", 1);
    SampleRate->addItem ("48000", 2);
    SampleRate->addItem ("88200", 3);
    SampleRate->addItem ("96000", 4);
    SampleRate->addListener (this);

    addAndMakeVisible (LabelSampleRate = new Label ("LabelSampleRate",
                                                    "Sample Rate:"));
    LabelSampleRate->setFont (Font (15.00f, Font::plain));
    LabelSampleRate->setJustificationType (Justification::centredRight);
    LabelSampleRate->setEditable (false, false, false);
    LabelSampleRate->setColour (TextEditor::textColourId, Colours::black);
    LabelSampleRate->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (LabelServerAddress = new Label ("LabelServerAddress",
                                                       "Server Address:"));
    LabelServerAddress->setFont (Font (15.00f, Font::plain));
    LabelServerAddress->setJustificationType (Justification::centredRight);
    LabelServerAddress->setEditable (false, false, false);
    LabelServerAddress->setColour (TextEditor::textColourId, Colours::black);
    LabelServerAddress->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (Server = new ComboBox ("Server"));
    Server->setEditableText (true);
    Server->setJustificationType (Justification::centredLeft);
    Server->setTextWhenNothingSelected (String::empty);
    Server->setTextWhenNoChoicesAvailable ("(no choices)");
    Server->addItem ("127.0.0.1", 1);
    Server->addListener (this);

    addAndMakeVisible (LabelInput = new Label ("LabelInput",
                                               "I N P U T S"));
    LabelInput->setFont (Font (15.00f, Font::plain));
    LabelInput->setJustificationType (Justification::centredLeft);
    LabelInput->setEditable (false, false, false);
    LabelInput->setColour (TextEditor::textColourId, Colours::black);
    LabelInput->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (LabelInput2 = new Label ("LabelInput",
                                                "O\nU\nT\nP\nU\nT\nS"));
    LabelInput2->setFont (Font (15.00f, Font::plain));
    LabelInput2->setJustificationType (Justification::centredTop);
    LabelInput2->setEditable (false, false, false);
    LabelInput2->setColour (TextEditor::textColourId, Colours::black);
    LabelInput2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (textButton = new TextButton ("new button"));
    textButton->setButtonText ("Resync");
    textButton->addListener (this);
    textButton->setColour (TextButton::buttonColourId, Colour (0xffff8a8a));
    textButton->setColour (TextButton::buttonOnColourId, Colour (0xffff7373));


    //[UserPreSize]
    addAndMakeVisible (matrixView = new MatrixView());
    matrixView->setName ("MatrixView");
    matrixView->setBounds (368, 32, 224, 152);
    //[/UserPreSize]

    setSize (450, 250);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Content::~Content()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    GroupServer = nullptr;
    GroupSettings = nullptr;
    InputDevices = nullptr;
    LabelInputDevice = nullptr;
    OutputDevices = nullptr;
    LabelOutputDevice = nullptr;
    SampleRate = nullptr;
    LabelSampleRate = nullptr;
    LabelServerAddress = nullptr;
    Server = nullptr;
    LabelInput = nullptr;
    LabelInput2 = nullptr;
    textButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    matrixView = nullptr;
    //[/Destructor]
}

//==============================================================================
void Content::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    g.setGradientFill(ColourGradient(Colours::silver, 0, 0,
      Colours::grey, getWidth(), getHeight(), false));
    g.fillAll ();


    //[/UserPaint]
}

void Content::resized()
{
    GroupServer->setBounds (8, 136, 336, 104);
    GroupSettings->setBounds (8, 8, 336, 128);
    InputDevices->setBounds (128, 32, 200, 24);
    LabelInputDevice->setBounds (24, 32, 104, 24);
    OutputDevices->setBounds (128, 64, 200, 24);
    LabelOutputDevice->setBounds (24, 64, 104, 24);
    SampleRate->setBounds (128, 96, 200, 24);
    LabelSampleRate->setBounds (24, 96, 104, 24);
    LabelServerAddress->setBounds (16, 160, 112, 24);
    Server->setBounds (128, 160, 200, 24);
    LabelInput->setBounds (364, 8, 150, 24);
    LabelInput2->setBounds (348, 30, 24, 128);
    textButton->setBounds (24, 200, 302, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void Content::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == InputDevices)
    {
        //[UserComboBoxCode_InputDevices] -- add your combo box handling code here..
      pushSettings();
        //[/UserComboBoxCode_InputDevices]
    }
    else if (comboBoxThatHasChanged == OutputDevices)
    {
        //[UserComboBoxCode_OutputDevices] -- add your combo box handling code here..
      pushSettings();
        //[/UserComboBoxCode_OutputDevices]
    }
    else if (comboBoxThatHasChanged == SampleRate)
    {
        //[UserComboBoxCode_SampleRate] -- add your combo box handling code here..
      pushSettings();
        //[/UserComboBoxCode_SampleRate]
    }
    else if (comboBoxThatHasChanged == Server)
    {
        //[UserComboBoxCode_Server] -- add your combo box handling code here..
      connectToServer(Server->getText());
        //[/UserComboBoxCode_Server]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void Content::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == textButton)
    {
        //[UserButtonCode_textButton] -- add your button handler code here..
        resync();
        //[/UserButtonCode_textButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Content" componentName="Content"
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330000013"
                 fixedSize="0" initialWidth="450" initialHeight="250">
  <BACKGROUND backgroundColour="ffffffff"/>
  <GROUPCOMPONENT name="GroupServer" id="5c62e8b06634289a" memberName="GroupServer"
                  virtualName="" explicitFocusOrder="0" pos="8 136 336 104" title="Server"/>
  <GROUPCOMPONENT name="GroupSettings" id="45126cd1cebb956f" memberName="GroupSettings"
                  virtualName="" explicitFocusOrder="0" pos="8 8 336 128" title="Settings"/>
  <COMBOBOX name="InputDevices" id="3fb30b9465750396" memberName="InputDevices"
            virtualName="" explicitFocusOrder="0" pos="128 32 200 24" editable="0"
            layout="33" items="" textWhenNonSelected="(no server)" textWhenNoItems="(no server)"/>
  <LABEL name="LabelInputDevice" id="83bb2227734f4c97" memberName="LabelInputDevice"
         virtualName="" explicitFocusOrder="0" pos="24 32 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Input Device:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="OutputDevices" id="5ebe95ccb4b797a6" memberName="OutputDevices"
            virtualName="" explicitFocusOrder="0" pos="128 64 200 24" editable="0"
            layout="33" items="" textWhenNonSelected="(no server)" textWhenNoItems="(no server)"/>
  <LABEL name="LabelOutputDevice" id="8fa31be9516ee249" memberName="LabelOutputDevice"
         virtualName="" explicitFocusOrder="0" pos="24 64 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Output Device:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="SampleRate" id="ebcaae75f6e88ade" memberName="SampleRate"
            virtualName="" explicitFocusOrder="0" pos="128 96 200 24" editable="0"
            layout="33" items="44100&#10;48000&#10;88200&#10;96000" textWhenNonSelected="(no server)"
            textWhenNoItems="(no choices)"/>
  <LABEL name="LabelSampleRate" id="2d17c5db4f30ac83" memberName="LabelSampleRate"
         virtualName="" explicitFocusOrder="0" pos="24 96 104 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Sample Rate:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="34"/>
  <LABEL name="LabelServerAddress" id="6b9f56c7c2d0aca7" memberName="LabelServerAddress"
         virtualName="" explicitFocusOrder="0" pos="16 160 112 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Server Address:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="34"/>
  <COMBOBOX name="Server" id="ca05aeeeb71e3f3e" memberName="Server" virtualName=""
            explicitFocusOrder="0" pos="128 160 200 24" editable="1" layout="33"
            items="127.0.0.1" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="LabelInput" id="1addf7b81a33dbac" memberName="LabelInput"
         virtualName="" explicitFocusOrder="0" pos="364 8 150 24" edTextCol="ff000000"
         edBkgCol="0" labelText="I N P U T S" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="33"/>
  <LABEL name="LabelInput" id="39e32f52b0f98a3c" memberName="LabelInput2"
         virtualName="" explicitFocusOrder="0" pos="348 30 24 128" edTextCol="ff000000"
         edBkgCol="0" labelText="O&#10;U&#10;T&#10;P&#10;U&#10;T&#10;S"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="12"/>
  <TEXTBUTTON name="new button" id="8ad734bc01fdfa1e" memberName="textButton"
              virtualName="" explicitFocusOrder="0" pos="24 200 302 24" bgColOff="ffff8a8a"
              bgColOn="ffff7373" buttonText="Resync" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
