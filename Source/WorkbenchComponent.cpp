#include "WorkbenchComponent.h"
#include "Identifiers.h"
#include "MainComponent.h"

WorkbenchComponent::WorkbenchComponent(MainContentComponent* mainComponent_, WorkbenchClient* client_, Settings *settings_):
	mainComponent(mainComponent_),
	client(client_),
	settings(settings_),
	talkerStreamsTab(nullptr),
	listenerStreamsTab(nullptr),
	tree(settings_->getStreamsTree())
{
	addAndMakeVisible (portEditor = new TextEditor ("portEditor"));
	portEditor->setMultiLine (false);
	portEditor->setReturnKeyStartsNewLine (false);
	portEditor->setReadOnly (false);
	portEditor->setScrollbarsShown (true);
	portEditor->setCaretVisible (true);
	portEditor->setPopupMenuEnabled (true);
	portEditor->setColour(TextEditor::highlightColourId,  Colours::orange);
	portEditor->setColour(TextEditor::focusedOutlineColourId,  Colours::orange);
	portEditor->setInputRestrictions(5, "0123456789");
	portEditor->addListener(this);

	portEditor->setText(tree[Identifiers::Port].toString());

	addAndMakeVisible (portLabel = new Label ("port label",
		TRANS("Port")));
	portLabel->setFont (Font (15.00f, Font::plain));
	portLabel->setJustificationType (Justification::centredRight);
	portLabel->setEditable (false, false, false);
	portLabel->setColour (TextEditor::textColourId, Colours::black);
	portLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));
	
	addAndMakeVisible (connectButton = new TextButton ("connectButton"));
	connectButton->setButtonText (TRANS("Connect"));
	connectButton->addListener (this);
	
	addAndMakeVisible (disconnectButton = new TextButton ("disconnectButton"));
	disconnectButton->setButtonText (TRANS("Disconnect"));
	disconnectButton->addListener (this);
	disconnectButton->setEnabled(false);

	addAndMakeVisible (infoButton = new TextButton ("Get System Info"));
	infoButton->addListener (this);
	infoButton->setEnabled(false);

	addAndMakeVisible (getTalkersButton = new TextButton ("Get Talker Info"));
	getTalkersButton->addListener (this);
	getTalkersButton->setEnabled(false);

	addAndMakeVisible (getListenersButton = new TextButton ("Get Listener Info"));
	getListenersButton->addListener (this);
	getListenersButton->setEnabled(false);

	addAndMakeVisible(&sendReadout);
	sendReadout.setColour(TextEditor::outlineColourId, Colours::lightgrey);
	sendReadout.setReadOnly(true);
	sendReadout.setMultiLine(true);
	addAndMakeVisible(&receiveReadout);
	receiveReadout.setColour(TextEditor::outlineColourId, Colours::lightgrey);
	receiveReadout.setReadOnly(true);
	receiveReadout.setMultiLine(true);

	client->lastMessageSent.addListener(this);
	client->lastMessageReceived.addListener(this);

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	client->changeBroadcaster.addChangeListener(this);
	tree.addListener(this);

	setSize (600, 400);
}

WorkbenchComponent::~WorkbenchComponent()
{
	tabs = nullptr;
	portEditor = nullptr;
	portLabel = nullptr;
	connectButton = nullptr;
	disconnectButton = nullptr;
	infoButton = nullptr;
	getTalkersButton = nullptr;
	getListenersButton = nullptr;
	client->lastMessageSent.removeListener(this);
	client->lastMessageReceived.removeListener(this);
	client->changeBroadcaster.removeChangeListener(this);
}

void WorkbenchComponent::paint (Graphics& g)
{
	g.fillAll (Colours::white);

}

void WorkbenchComponent::resized()
{
	portLabel->setBounds (10, 20, 40, 24);
	portEditor->setBounds (portLabel->getRight() + 5, 20, 50, 24);

	connectButton->setBounds (portEditor->getRight() + 20, 20, 104, 24);
	juce::Rectangle<int> r(connectButton->getBounds());
	disconnectButton->setBounds(r.translated( r.getWidth() + 5, 0));

	int y = portLabel->getBottom() + 10;
	infoButton->setBounds(20, y, 104, 24);
	r = infoButton->getBounds();
	getTalkersButton->setBounds(r.translated( r.getWidth() + 5, 0));
	r = getTalkersButton->getBounds();
	getListenersButton->setBounds(r.translated(r.getWidth() + 5, 0));
	r = getListenersButton->getBounds();

	y = infoButton->getBottom() + 10;
	int w = getWidth()/2 - 20;
	int h = getHeight() - y - 10;
	tabs->setBounds(10, y, w, h);
	h /= 2;
	sendReadout.setBounds(tabs->getRight() + 20, y, w, h);
	receiveReadout.setBounds(sendReadout.getBounds().translated(0, h));
}

void WorkbenchComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == infoButton)
	{
		client->getSystemInfo();
		return;
	}

	if (buttonThatWasClicked == getTalkersButton)
	{
		client->getTalkerStreams();
		return;
	}

	if (buttonThatWasClicked == getListenersButton)
	{
		client->getListenerStreams();
		return;
	}

	if (buttonThatWasClicked == connectButton)
	{
		client->connectToSocket(mainComponent->getAddress(), portEditor->getText().getIntValue() ,1000);
		return;
	}

	if (buttonThatWasClicked == disconnectButton)
	{
		client->disconnect();
		return;
	}
}

void WorkbenchComponent::changeListenerCallback( ChangeBroadcaster* /*source*/ )
{
	triggerAsyncUpdate();
}

void WorkbenchComponent::textEditorTextChanged( TextEditor& )
{
}

void WorkbenchComponent::textEditorReturnKeyPressed( TextEditor& edit)
{

	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void WorkbenchComponent::textEditorEscapeKeyPressed( TextEditor& edit)
{

	if (&edit == portEditor)
	{
		portEditor->setText(tree[Identifiers::Port].toString());
		return;
	}
}

void WorkbenchComponent::textEditorFocusLost( TextEditor& edit)
{
	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void WorkbenchComponent::updatePort()
{
	int port = portEditor->getText().getIntValue();
	port = jlimit(0, 65535, port);
	portEditor->setText(String(port));
	tree.setProperty(Identifiers::Port, port, nullptr);
}

void WorkbenchComponent::actionListenerCallback( const String& message )
{
	
}

void WorkbenchComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	DBG("WorkbenchComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());
}

void WorkbenchComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
	//DBG("WorkbenchComponent::valueTreeChildAdded " << parentTree.getType().toString() << " " << childWhichHasBeenAdded.getType().toString());

	triggerAsyncUpdate();
}

void WorkbenchComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
	//DBG("WorkbenchComponent::valueTreeChildRemoved " << parentTree.getType().toString() << " " << childWhichHasBeenRemoved.getType().toString());

	triggerAsyncUpdate();
}

void WorkbenchComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{

}

void WorkbenchComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{

}

void WorkbenchComponent::enableControls()
{
	bool connected = client->isConnected();
	connectButton->setEnabled(!connected);
	disconnectButton->setEnabled(connected);
	infoButton->setEnabled(connected);

	ScopedLock locker(settings->lock);
	ValueTree talkersTree(settings->getStreamsTree().getChildWithName(Identifiers::Talkers));
	ValueTree listenersTree(settings->getStreamsTree().getChildWithName(Identifiers::Listeners));

	getTalkersButton->setEnabled(talkersTree.isValid());
	getListenersButton->setEnabled(listenersTree.isValid());
}

void WorkbenchComponent::handleAsyncUpdate()
{
	enableControls();
	updateStreamControls();
}

void WorkbenchComponent::updateStreamControls()
{
	ScopedLock locker(settings->lock);

	ValueTree talkersTree(settings->getStreamsTree().getChildWithName(Identifiers::Talkers));
	if (talkersTree.getNumChildren() != 0)
	{
		if (nullptr == talkerStreamsTab)
		{
			talkerStreamsTab = new StaticStreamViewport(talkersTree, settings->lock, client);
			tabs->addTab("Talkers", Colours::white, talkerStreamsTab, true, TALKERS_TAB);
		}
	}
	else
	{
		tabs->removeTab(TALKERS_TAB);
		talkerStreamsTab = nullptr;
	}

	ValueTree listenersTree(settings->getStreamsTree().getChildWithName(Identifiers::Listeners));
	if (listenersTree.getNumChildren() != 0)
	{
		if (nullptr == listenerStreamsTab)
		{
			listenerStreamsTab = new StaticStreamViewport(listenersTree, settings->lock, client);
			tabs->addTab("Listeners", Colours:: white, listenerStreamsTab, true, LISTENERS_TAB);
		}
	}
	else
	{
		tabs->removeTab(LISTENERS_TAB);
		listenerStreamsTab = nullptr;
	}
}

void WorkbenchComponent::valueChanged( Value& value )
{
	TextEditor* editor;

	if (value.refersToSameSourceAs(client->lastMessageSent))
	{
		editor = &sendReadout;
	}
	else if (value.refersToSameSourceAs(client->lastMessageReceived))
	{
		editor = &receiveReadout;
	}
	else
	{
		return;
	}

	var json(JSON::parse(value.toString()));
	editor->setText(JSON::toString(json));
}
