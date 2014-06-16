#pragma once


#include "base.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"

class MainContentComponent;

class AudioPatchbayComponent   : public Component, 
	public ButtonListener, 
	public ChangeListener, 
	public TextEditor::Listener,
	public ActionListener,
	public ValueTree::Listener,
	public AsyncUpdater,
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
	virtual void handleAsyncUpdate();

	virtual void valueChanged( Value& value );

	AudioPatchbayClient* client;
	Settings *settings;
	MainContentComponent *mainComponent;

	ScopedPointer<TextEditor> portEditor;
	ScopedPointer<Label> portLabel;
	ScopedPointer<TextButton> connectButton;
	ScopedPointer<TextButton> disconnectButton;
	ScopedPointer<TextButton> infoButton;
	ScopedPointer<TextButton> getTalkersButton;
	ScopedPointer<TextButton> getListenersButton;

	TextEditor sendReadout;
	TextEditor receiveReadout;

	//ScopedPointer<TabbedComponent> tabs;

	enum
	{
		TALKERS_TAB,
		LISTENERS_TAB
	};
};