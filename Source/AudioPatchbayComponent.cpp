#include "AudioPatchBayComponent.h"
#include "Identifiers.h"
#include "MainComponent.h"

AudioPatchbayComponent::AudioPatchbayComponent(MainContentComponent* mainComponent_, AudioPatchbayClient* client_, Settings *settings_):
	mainComponent(mainComponent_),
	client(client_),
	settings(settings_)
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

	client->changeBroadcaster.addChangeListener(this);
	settings->tree.addListener(this);

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	setSize (600, 400);
}

AudioPatchbayComponent::~AudioPatchbayComponent()
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
	settings->tree.removeListener(this);
}

void AudioPatchbayComponent::paint (Graphics& g)
{
	g.fillAll (Colours::white);

}

void AudioPatchbayComponent::resized()
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
	h /= 2;
	sendReadout.setBounds(tabs->getRight() + 20, y, w, h);
	receiveReadout.setBounds(sendReadout.getBounds().translated(0, h));
}

void AudioPatchbayComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == infoButton)
	{
		client->getSystemInfo();
		return;
	}

	if (buttonThatWasClicked == getTalkersButton)
	{
		ValueTree tree;
		deviceComponentTab = new DeviceComponent(tree, settings->lock, client);
		tabs->addTab("Device", Colours::white, deviceComponentTab, true, 0);
		return;
	}

	if (buttonThatWasClicked == getListenersButton)
	{
		//client->getListenerStreams();
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

void AudioPatchbayComponent::changeListenerCallback( ChangeBroadcaster* /*source*/ )
{
	triggerAsyncUpdate();
}

void AudioPatchbayComponent::textEditorTextChanged( TextEditor& )
{
}

void AudioPatchbayComponent::textEditorReturnKeyPressed( TextEditor& edit)
{

	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void AudioPatchbayComponent::textEditorEscapeKeyPressed( TextEditor& edit)
{

	if (&edit == portEditor)
	{
		portEditor->setText(String(settings->getPort()));
		return;
	}
}

void AudioPatchbayComponent::textEditorFocusLost( TextEditor& edit)
{
	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void AudioPatchbayComponent::updatePort()
{
	int port = portEditor->getText().getIntValue();
	port = jlimit(0, 65535, port);
	portEditor->setText(String(port));
	settings->storePort(port);

}

void AudioPatchbayComponent::actionListenerCallback( const String& message )
{

}

void AudioPatchbayComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	//DBG("AudioPatchbayComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());
}

void AudioPatchbayComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
	//DBG("AudioPatchbayComponent::valueTreeChildAdded " << parentTree.getType().toString() << " " << childWhichHasBeenAdded.getType().toString());

	triggerAsyncUpdate();
}

void AudioPatchbayComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
	//DBG("AudioPatchbayComponent::valueTreeChildRemoved " << parentTree.getType().toString() << " " << childWhichHasBeenRemoved.getType().toString());

	triggerAsyncUpdate();
}

void AudioPatchbayComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{

}

void AudioPatchbayComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{

}

void AudioPatchbayComponent::enableControls()
{
	bool connected = client->isConnected();
	connectButton->setEnabled(!connected);
	disconnectButton->setEnabled(connected);
	infoButton->setEnabled(connected);

	getTalkersButton->setEnabled(connected);
	getListenersButton->setEnabled(connected);
}

void AudioPatchbayComponent::handleAsyncUpdate()
{
	enableControls();
}

void AudioPatchbayComponent::valueChanged( Value& value )
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


