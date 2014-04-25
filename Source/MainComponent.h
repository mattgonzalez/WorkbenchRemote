/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "WorkbenchClient.h"
#include "Settings.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component, public ButtonListener, public ChangeListener, public TextEditor::Listener
{
public:
    //==============================================================================
    MainContentComponent(WorkbenchClient * client_, Settings *settings_);
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	void buttonClicked (Button* buttonThatWasClicked);


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)

	virtual void changeListenerCallback( ChangeBroadcaster* source );

	virtual void textEditorTextChanged( TextEditor& );

	virtual void textEditorReturnKeyPressed( TextEditor& );

	virtual void textEditorEscapeKeyPressed( TextEditor& );

	virtual void textEditorFocusLost( TextEditor& );
	void updateAddress();
	void updatePort();

	WorkbenchClient* client;
	Settings *settings;

	ScopedPointer<TextEditor> addressEditor;
	ScopedPointer<TextEditor> portEditor;
	ScopedPointer<TextEditor> streamIDEditor;
	ScopedPointer<Label> addressLabel;
	ScopedPointer<Label> portLabel;
	ScopedPointer<Label> streamIDLabel;
	ScopedPointer<TextButton> connectButton;
	ScopedPointer<TextButton> disconnectButton;
	ScopedPointer<TextButton> infoButton;
	ScopedPointer<TextButton> getTalkersButton;
	ScopedPointer<TextButton> setTalkerButton;
	TextEditor readout;
};


#endif  // MAINCOMPONENT_H_INCLUDED
