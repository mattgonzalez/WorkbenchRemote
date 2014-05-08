/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "base.h"
#include "WorkbenchClient.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"
#include "StaticStreamViewport.h"


class MainContentComponent   : public Component, 
	public ButtonListener, 
	public ChangeListener, 
	public TextEditor::Listener,
	public ActionListener,
	public ValueTree::Listener,
	public AsyncUpdater
{
public:
    //==============================================================================
    MainContentComponent(WorkbenchClient * client_, AudioPatchbayClient* patchbyClient_, Settings *settings_);
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	void buttonClicked (Button* buttonThatWasClicked);

private:
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)

	void updateAddress();
	void updatePort();

	virtual void changeListenerCallback( ChangeBroadcaster* source );

	void enableControls();
	void updateStreamControls();

	virtual void textEditorTextChanged( TextEditor& );
	virtual void textEditorReturnKeyPressed( TextEditor& );
	virtual void textEditorEscapeKeyPressed( TextEditor& );
	virtual void textEditorFocusLost( TextEditor& );
	virtual void actionListenerCallback( const String& message );

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
	virtual void handleAsyncUpdate();
	
	WorkbenchClient* client;
	AudioPatchbayClient* patchbayClient;
	Settings *settings;

	ScopedPointer<TextEditor> addressEditor;
	ScopedPointer<TextEditor> portEditor;
	ScopedPointer<Label> addressLabel;
	ScopedPointer<Label> portLabel;
	ScopedPointer<TextButton> connectButton;
	ScopedPointer<TextButton> connectPatchbayButton;
	ScopedPointer<TextButton> disconnectButton;
	ScopedPointer<TextButton> disconnectPatchbayButton;
	ScopedPointer<TextButton> infoButton;
	ScopedPointer<TextButton> getTalkersButton;
	ScopedPointer<TextButton> setTalkerButton;
	TextEditor readout;
	ScopedPointer<TabbedComponent> tabs;
	StaticStreamViewport *talkerStreamsTab;
	StaticStreamViewport *listenerStreamsTab;

	enum
	{
		TALKERS_TAB,
		LISTENERS_TAB
	};
};


#endif  // MAINCOMPONENT_H_INCLUDED
