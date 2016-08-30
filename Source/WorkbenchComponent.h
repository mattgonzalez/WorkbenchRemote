#pragma once

#include "base.h"
#include "WorkbenchClient.h"
#include "Settings.h"
#include "StaticStreamViewport.h"
#include "SettingsComponent.h"
#include "PTPViewport.h"

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
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int );
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
	ScopedPointer<TextButton> getLinkStateButton;
	ScopedPointer<TextButton> getSettingsButton;
	ScopedPointer<TextButton> getPTPInfoButton;

	TextEditor sendReadout;
	TextEditor receiveReadout;

	ScopedPointer<TabbedComponent> tabs;
	StaticStreamViewport *talkerStreamsTab;
	StaticStreamViewport *listenerStreamsTab;
	SettingsComponent *settingsTab;
	PTPViewport *ptpTab;

	class StatusBarComponent : public Component, ValueTree::Listener
	{
	public:
		StatusBarComponent(Settings* settings_);
		~StatusBarComponent();
		void paint(Graphics &g);
		void resized();

		void updateLinkStateLabel();

		virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
		virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
		virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int );
		virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int );
		virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
		String toString();

		Label linkStateLabel;
	protected:
		Settings* settings;
		String speedToString(uint64 speed) const;
		ValueTree tree;
	} statusBar;
	enum
	{
		TALKERS_TAB,
		LISTENERS_TAB,
		PTP_TAB,
		SETTINGS_TAB,

		MediaDuplexStateUnknown = 0,
		MediaDuplexStateHalf,
		MediaDuplexStateFull,

		MediaConnectStateUnknown = 0,
		MediaConnectStateConnected,
		MediaConnectStateDisconnected,

		ANALYZERBR_USB_ETHERNET_MODE_STANDARD = 1,
		ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER,
		ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE,
		ANALYZERBR_USB_ETHERNET_MODE_BR	
	};
};
