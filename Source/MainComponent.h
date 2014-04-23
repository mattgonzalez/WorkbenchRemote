/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Client.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public ButtonListener
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	void buttonClicked (Button* buttonThatWasClicked);


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)

	StreamingSocket socket;
	StreamingSocket listenSocket;

	Client* client;

	ScopedPointer<TextEditor> addressEditor;
	ScopedPointer<Label> label;
	ScopedPointer<TextButton> connectButton;
	TextButton listenButton;
};


#endif  // MAINCOMPONENT_H_INCLUDED
