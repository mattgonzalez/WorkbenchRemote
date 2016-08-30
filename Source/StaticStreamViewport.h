/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

class WorkbenchClient;

class StaticStreamViewport : public Viewport
{
public:
	StaticStreamViewport(ValueTree tree_, CriticalSection &lock_, WorkbenchClient* client_);
	~StaticStreamViewport();

	void streamActiveStateChanged();

protected:

	class StaticStreamComponent;
	class ContentComponent : public Component
	{
	public:
		ContentComponent(ValueTree tree_, CriticalSection &lock_, WorkbenchClient* client_);
		~ContentComponent();

		OwnedArray<StaticStreamComponent> streamBoxes;

		virtual void resized();
		virtual void paint( Graphics& g );
		void positionStaticStreamComponents();
		void streamActiveStateChanged();

		ValueTree tree;
		CriticalSection &lock;
	};

	ScopedPointer<ContentComponent> content;

	class StaticStreamComponent :
		public Component,
		public TextEditor::Listener,
		public ComboBox::Listener,
		public Button::Listener,
		public ValueTree::Listener
	{
	public:
		StaticStreamComponent(ValueTree tree_, CriticalSection &lock_, ContentComponent* parent_, WorkbenchClient* client_);
		~StaticStreamComponent();

		ValueTree tree;
		CriticalSection &lock;
		WorkbenchClient* client;
		ContentComponent *parentContentComponent;

		TextEditor streamIdEditor;
		TextEditor multicastAddressEditor;
		ComboBox channelsCombo;
		TextButton startButton;
		TextButton stopButton;
		TextButton faultButton;
		ToggleButton clockReferenceButton;
		ToggleButton autoStartButton;

		Label streamIdLabel;
		Label multicastAddressLabel;
		Label channelsLabel;

		class MetricsButton : public Button
		{
		public:
			MetricsButton();
			virtual void paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown );
		} metricsButton;

		void enableClockReferenceButton();

	protected:
		void userInputMulticastAddress();
		void userInputStreamID();
		void enableControls(bool started);
		void setChannelsVisible();

		virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );
		virtual void buttonClicked( Button* );
		virtual void paint( Graphics& g );
		virtual void resized();
		virtual void textEditorTextChanged( TextEditor& );
		virtual void textEditorReturnKeyPressed( TextEditor& );
		virtual void textEditorEscapeKeyPressed( TextEditor& );
		virtual void textEditorFocusLost( TextEditor& );
		virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
		virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
		virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int );
		virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int );
		virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
	};

	virtual void resized() override;
};
