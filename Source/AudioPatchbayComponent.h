#pragma once


#include "base.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"
#include "DeviceComponent.h"

class MainContentComponent;

class AudioPatchbayComponent   : public Component, 
	public ButtonListener, 
	public ChangeListener, 
	public TextEditor::Listener,
	public ActionListener,
	public ValueTree::Listener,
	public Value::Listener
{
public:
	//==============================================================================
	AudioPatchbayComponent(MainContentComponent *mainComponent_, AudioPatchbayClient * client_,  Settings *settings_);
	~AudioPatchbayComponent();

	void paint (Graphics&);
	void resized();
	void buttonClicked (Button* buttonThatWasClicked);

private:
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPatchbayComponent)

	void updatePort();

	virtual void changeListenerCallback( ChangeBroadcaster* source );

	void enableControls();

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

	virtual void valueChanged( Value& value );
	
	ValueTree tree;
	AudioPatchbayClient* client;
	Settings *settings;
	MainContentComponent *mainComponent;

	ScopedPointer<TextEditor> portEditor;
	ScopedPointer<Label> portLabel;
	ScopedPointer<TextButton> connectButton;
	ScopedPointer<TextButton> disconnectButton;
	ScopedPointer<TextButton> infoButton;
	ScopedPointer<TextButton> getAvailableAudioDevicesButton;
	ScopedPointer<TextButton> getCurrentAudioDevicesButton;
	ScopedPointer<TextButton> getInputButton;
	ScopedPointer<TextButton> getOutputButton;

	TextEditor sendReadout;
	TextEditor receiveReadout;

	ScopedPointer<TabbedComponent> tabs;
	OwnedArray<DeviceComponent> deviceComponents;

	enum
	{
		TALKERS_TAB,
		LISTENERS_TAB
	};
};