/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "MainComponent.h"
#include "Identifiers.h"

MainContentComponent::MainContentComponent(WorkbenchClient* client_, AudioPatchbayClient* patchbayClient_, Settings *settings_):
	client(client_),
	patchbayClient(patchbayClient_),
	settings(settings_),
	talkerStreamsTab(nullptr),
	listenerStreamsTab(nullptr)
{
	addAndMakeVisible (addressEditor = new TextEditor ("addressEditor"));
	addressEditor->setMultiLine (false);
	addressEditor->setReturnKeyStartsNewLine (false);
	addressEditor->setReadOnly (false);
	addressEditor->setScrollbarsShown (true);
	addressEditor->setCaretVisible (true);
	addressEditor->setPopupMenuEnabled (true);
	addressEditor->setText ("127.0.0.1");
	addressEditor->setColour(TextEditor::highlightColourId,  Colours::orange);
	addressEditor->setColour(TextEditor::focusedOutlineColourId,  Colours::orange);
	addressEditor->setInputRestrictions(15, "0123456789.");
	addressEditor->addListener(this);
	
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

	addressEditor->setText(settings->getAddress().toString());
	portEditor->setText(String(settings->getPort()));

	addAndMakeVisible (addressLabel = new Label ("address label",
		TRANS("Address")));
	addressLabel->setFont (Font (15.00f, Font::plain));
	addressLabel->setJustificationType (Justification::centredRight);
	addressLabel->setEditable (false, false, false);
	addressLabel->setColour (TextEditor::textColourId, Colours::black);
	addressLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

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

	addAndMakeVisible (connectPatchbayButton = new TextButton ("connectPatchbayButton"));
	connectPatchbayButton->setButtonText (TRANS("ConnectPatchbay"));
	connectPatchbayButton->addListener (this);

	addAndMakeVisible (disconnectButton = new TextButton ("disconnectButton"));
	disconnectButton->setButtonText (TRANS("Disconnect"));
	disconnectButton->addListener (this);
	disconnectButton->setEnabled(false);

	addAndMakeVisible (disconnectPatchbayButton = new TextButton ("disconnectPatchbayButton"));
	disconnectPatchbayButton->setButtonText (TRANS("DisconnectPatchbay"));
	disconnectPatchbayButton->addListener (this);
	disconnectPatchbayButton->setEnabled(false);

	addAndMakeVisible (infoButton = new TextButton ("Get System Info"));
	infoButton->setButtonText (TRANS("Get System Info"));
	infoButton->addListener (this);
	infoButton->setEnabled(false);

	addAndMakeVisible (getTalkersButton = new TextButton ("Get Talker Info"));
	getTalkersButton->setButtonText (TRANS("Get Talker Info"));
	getTalkersButton->addListener (this);
	getTalkersButton->setEnabled(false);

	addAndMakeVisible(setTalkerButton = new TextButton ("Set Talker Info"));
	setTalkerButton->setButtonText( TRANS("Set Talker Info"));
	setTalkerButton->addListener(this);
	setTalkerButton->setEnabled(false);

	readout.setReadOnly(true);
	readout.setMultiLine(true);
	addAndMakeVisible(&readout);

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	client->changeBroadcaster.addChangeListener(this);
	client->stringBroadcaster.addActionListener(this);
	patchbayClient->changeBroadcaster.addChangeListener(this);
	settings->tree.addListener(this);

	setSize (600, 400);
}

MainContentComponent::~MainContentComponent()
{
	tabs = nullptr;
	addressEditor = nullptr;
	portEditor = nullptr;
	addressLabel = nullptr;
	portLabel = nullptr;
	connectButton = nullptr;
	connectPatchbayButton = nullptr;
	disconnectButton = nullptr;
	disconnectPatchbayButton = nullptr;
	infoButton = nullptr;
	getTalkersButton = nullptr;
	setTalkerButton = nullptr;
	client->stringBroadcaster.removeActionListener(this);
	client->changeBroadcaster.removeChangeListener(this);
	patchbayClient->changeBroadcaster.removeChangeListener(this);
	settings->tree.removeListener(this);
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);

}

void MainContentComponent::resized()
{
	addressLabel->setBounds (10, 20, 60, 24);
	addressEditor->setBounds (addressLabel->getRight() + 5, 20, 110, 24);
	portLabel->setBounds (addressEditor->getRight() + 5, 20, 40, 24);
	portEditor->setBounds (portLabel->getRight() + 5, 20, 50, 24);

	connectButton->setBounds (portEditor->getRight() + 20, 20, 104, 24);
	juce::Rectangle<int> r(connectButton->getBounds());
	disconnectButton->setBounds(r.translated( r.getWidth() + 5, 0));
	r = disconnectButton->getBounds();
	connectPatchbayButton->setBounds(r.translated( r.getWidth() + 5, 0));
	r = connectPatchbayButton->getBounds();
	disconnectPatchbayButton->setBounds(r.translated( r.getWidth() + 5, 0));

	int y = addressLabel->getBottom() + 10;
	infoButton->setBounds(96, y, 104, 24);
	r = infoButton->getBounds();
	getTalkersButton->setBounds(r.translated( r.getWidth() + 5, 0));
	r = getTalkersButton->getBounds();
	setTalkerButton->setBounds(r.translated( r.getWidth() + 5, 0));

	y = infoButton->getBottom() + 10;
	int w = getWidth()/2 - 20;
	int h = getHeight() - y - 10;
	tabs->setBounds(10, y, w, h);
	readout.setBounds(tabs->getRight() + 20, y, w, h);
}

void MainContentComponent::buttonClicked (Button* buttonThatWasClicked)
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

	if (buttonThatWasClicked == connectButton)
	{
		client->connectToSocket(addressEditor->getText(), portEditor->getText().getIntValue() ,1000);
		return;
	}

	if (buttonThatWasClicked == connectPatchbayButton)
	{
		patchbayClient->connectToSocket(addressEditor->getText(), portEditor->getText().getIntValue(), 1000);
		return;
	}

	if (buttonThatWasClicked == disconnectButton)
	{
		client->disconnect();
		return;
	}

	if (buttonThatWasClicked == disconnectPatchbayButton)
	{
		patchbayClient->disconnect();
		return;
	}
}

void MainContentComponent::changeListenerCallback( ChangeBroadcaster* /*source*/ )
{
	triggerAsyncUpdate();
}

void MainContentComponent::textEditorTextChanged( TextEditor& )
{
}

void MainContentComponent::textEditorReturnKeyPressed( TextEditor& edit)
{
	if (&edit == addressEditor)
	{
		updateAddress();
		return;
	}
	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void MainContentComponent::textEditorEscapeKeyPressed( TextEditor& edit)
{
	if (&edit == addressEditor)
	{
		addressEditor->setText(settings->getAddress().toString());
		return;
	}
	if (&edit == portEditor)
	{
		portEditor->setText(String(settings->getPort()));
		return;
	}
}

void MainContentComponent::textEditorFocusLost( TextEditor& edit)
{
	if (&edit == addressEditor)
	{
		updateAddress();
		return;
	}

	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void MainContentComponent::updateAddress()
{
	IPAddress address(addressEditor->getText());
	addressEditor->setText(address.toString());
	settings->storeAddress(address);
}

void MainContentComponent::updatePort()
{
	int port = portEditor->getText().getIntValue();
	port = jlimit(0, 65535, port);
	portEditor->setText(String(port));
	settings->storePort(port);

}

void MainContentComponent::actionListenerCallback( const String& message )
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

void MainContentComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	//DBG("MainContentComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());
}

void MainContentComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
	//DBG("MainContentComponent::valueTreeChildAdded " << parentTree.getType().toString() << " " << childWhichHasBeenAdded.getType().toString());

	triggerAsyncUpdate();
}

void MainContentComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
	//DBG("MainContentComponent::valueTreeChildRemoved " << parentTree.getType().toString() << " " << childWhichHasBeenRemoved.getType().toString());

	triggerAsyncUpdate();
}

void MainContentComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
	
}

void MainContentComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
	
}

void MainContentComponent::enableControls()
{
	bool connected = client->isConnected();
	bool patchbayConnected = patchbayClient->isConnected();
	connectButton->setEnabled(!connected);
	connectPatchbayButton->setEnabled(!patchbayConnected);
	disconnectButton->setEnabled(connected);
	disconnectPatchbayButton->setEnabled(patchbayConnected);
	infoButton->setEnabled(connected);

	ScopedLock locker(settings->lock);
	ValueTree talkersTree(settings->tree.getChildWithName(Identifiers::Talkers));
	getTalkersButton->setEnabled(talkersTree.isValid());
	setTalkerButton->setEnabled(talkersTree.isValid());
}

void MainContentComponent::handleAsyncUpdate()
{
	enableControls();
	updateStreamControls();
}

void MainContentComponent::updateStreamControls()
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
}
