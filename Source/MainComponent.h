/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "WorkbenchClient.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public ButtonListener
{
public:
    //==============================================================================
    MainContentComponent(WorkbenchClient * client_);
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	void buttonClicked (Button* buttonThatWasClicked);


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)

	StreamingSocket socket;
	StreamingSocket listenSocket;

	WorkbenchClient* client;

	ScopedPointer<TextEditor> addressEditor;
	ScopedPointer<TextEditor> portEditor;
	ScopedPointer<Label> label;
	ScopedPointer<TextButton> connectButton;
	ScopedPointer<TextButton> disconnectButton;

	TextButton listenButton;
};


#endif  // MAINCOMPONENT_H_INCLUDED
