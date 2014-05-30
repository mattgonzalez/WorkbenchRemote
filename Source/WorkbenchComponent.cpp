#include "WorkbenchComponent.h"
#include "Identifiers.h"
#include "MainComponent.h"

WorkbenchComponent::WorkbenchComponent(MainContentComponent* mainComponent_, WorkbenchClient* client_, Settings *settings_):
	mainComponent(mainComponent_),
	client(client_),
	settings(settings_),
	talkerStreamsTab(nullptr),
	listenerStreamsTab(nullptr)
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

	portEditor->setText(String(settings->getPort()));

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
	infoButton->setButtonText (TRANS("Get System Info"));
	infoButton->addListener (this);
	infoButton->setEnabled(false);

	addAndMakeVisible (getTalkersButton = new TextButton ("Get Talker Info"));
	getTalkersButton->setButtonText (TRANS("Get Talker Info"));
	getTalkersButton->addListener (this);
	getTalkersButton->setEnabled(false);

	addAndMakeVisible (getListenersButton = new TextButton ("Get Listener Info"));
	getListenersButton->setButtonText (TRANS("Get Listener Info"));
	getListenersButton->addListener (this);
	getListenersButton->setEnabled(false);

	readout.setReadOnly(true);
	readout.setMultiLine(true);
	addAndMakeVisible(&readout);

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	client->changeBroadcaster.addChangeListener(this);
	client->stringBroadcaster.addActionListener(this);
	settings->tree.addListener(this);

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
	client->stringBroadcaster.removeActionListener(this);
	client->changeBroadcaster.removeChangeListener(this);
	settings->tree.removeListener(this);
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
	infoButton->setBounds(96, y, 104, 24);
	r = infoButton->getBounds();
	getTalkersButton->setBounds(r.translated( r.getWidth() + 5, 0));
	r = getTalkersButton->getBounds();
	getListenersButton->setBounds(r.translated(r.getWidth() + 5, 0));
	r = getListenersButton->getBounds();

	y = infoButton->getBottom() + 10;
	int w = getWidth()/2 - 20;
	int h = getHeight() - y - 10;
	tabs->setBounds(10, y, w, h);
	readout.setBounds(tabs->getRight() + 20, y, w, h);
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
// 	if (&edit == addressEditor)
// 	{
// 		updateAddress();
// 		return;
// 	}
	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void WorkbenchComponent::textEditorEscapeKeyPressed( TextEditor& edit)
{
// 	if (&edit == addressEditor)
// 	{
// 		addressEditor->setText(settings->getAddress().toString());
// 		return;
// 	}
	if (&edit == portEditor)
	{
		portEditor->setText(String(settings->getPort()));
		return;
	}
}

void WorkbenchComponent::textEditorFocusLost( TextEditor& edit)
{
// 	if (&edit == addressEditor)
// 	{
// 		updateAddress();
// 		return;
// 	}

	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void WorkbenchComponent::updateAddress()
{
	//IPAddress address(addressEditor->getText());
	//addressEditor->setText(address.toString());
	//settings->storeAddress(address);
}

void WorkbenchComponent::updatePort()
{
	int port = portEditor->getText().getIntValue();
	port = jlimit(0, 65535, port);
	portEditor->setText(String(port));
	settings->storePort(port);

}

void WorkbenchComponent::actionListenerCallback( const String& message )
{
	int split = message.indexOfChar('{');

	readout.setCaretPosition(INT_MAX);
	readout.setColour(TextEditor::textColourId, Colours::slategrey);
	readout.insertTextAtCaret(message.substring(0, split));
	readout.insertTextAtCaret("\n");

	readout.setColour(TextEditor::textColourId, Colours::black);
	var json(JSON::parse(message.substring(split)));
	readout.insertTextAtCaret(JSON::toString(json));
	readout.insertTextAtCaret("\n\n");
}

void WorkbenchComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	//DBG("WorkbenchComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());
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
	ValueTree talkersTree(settings->tree.getChildWithName(Identifiers::Talkers));
	ValueTree listenersTree(settings->tree.getChildWithName(Identifiers::Listeners));

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

	ValueTree talkersTree(settings->tree.getChildWithName(Identifiers::Talkers));
	if (talkersTree.getNumChildren() != 0)
	{
		if (nullptr == talkerStreamsTab)
		{
			talkerStreamsTab = new StaticStreamViewport(talkersTree, settings->lock, client);
			tabs->addTab("Talkers", Colours:: white, talkerStreamsTab, true,TALKERS_TAB);
		}
	}
	else
	{
		tabs->removeTab(TALKERS_TAB);
		talkerStreamsTab = nullptr;
	}

	ValueTree listenersTree(settings->tree.getChildWithName(Identifiers::Listeners));
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