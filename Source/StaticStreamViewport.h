#pragma once

class WorkbenchClient;

class StaticStreamViewport : public Viewport
{
public:
	StaticStreamViewport(ValueTree tree_, CriticalSection &lock_, WorkbenchClient* client_);
	~StaticStreamViewport();

protected:

	class StaticStreamComponent;
	class ContentComponent : public Component, public ChangeListener
	{
	public:
		ContentComponent(ValueTree tree_, CriticalSection &lock_, WorkbenchClient* client_);
		~ContentComponent();

		OwnedArray<StaticStreamComponent> streamBoxes;

		virtual void resized();

		virtual void changeListenerCallback( ChangeBroadcaster* source );

		virtual void paint( Graphics& g );
		void positionStaticStreamComponents();

		ValueTree tree;
		CriticalSection &lock;

	private:
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
		
		ToggleButton clockReferenceButton;

		Label streamIdLabel;
		Label multicastAddressLabel;
		Label channelsLabel;

		bool injectionEnabled;

		class MetricsButton : public Button
		{
		public:
			MetricsButton();
			virtual void paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown );
		} metricsButton;

		class InjectButton : public Button
		{
		public:
			InjectButton();
			virtual void paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown );
		} faultCalloutButton;


	protected:
		void userInputMulticastAddress();
		void userInputStreamID();
		void injectionCheck();
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
		virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
		virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
		virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
	};

	virtual void resized() override;
};
