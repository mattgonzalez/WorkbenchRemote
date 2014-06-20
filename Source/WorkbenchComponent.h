#pragma once

#include "base.h"
#include "WorkbenchClient.h"
#include "Settings.h"
#include "StaticStreamViewport.h"

class MainContentComponent;

class WorkbenchComponent   : public Component, 
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
	WorkbenchComponent(MainContentComponent *mainComponent_, WorkbenchClient * client_,  Settings *settings_);
	~WorkbenchComponent();

	void paint (Graphics&);
	void resized();
	void buttonClicked (Button* buttonThatWasClicked);

private:
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WorkbenchComponent)

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

	virtual void valueChanged( Value& value );

	WorkbenchClient* client;
	Settings *settings;
	MainContentComponent *mainComponent;
	ValueTree tree;

	ScopedPointer<TextEditor> portEditor;
	ScopedPointer<Label> portLabel;
	ScopedPointer<TextButton> connectButton;
	ScopedPointer<TextButton> disconnectButton;
	ScopedPointer<TextButton> infoButton;
	ScopedPointer<TextButton> getTalkersButton;
	ScopedPointer<TextButton> getListenersButton;
	
	TextEditor sendReadout;
	TextEditor receiveReadout;

	ScopedPointer<TabbedComponent> tabs;
	StaticStreamViewport *talkerStreamsTab;
	StaticStreamViewport *listenerStreamsTab;

	enum
	{
		TALKERS_TAB,
		LISTENERS_TAB
	};
};
