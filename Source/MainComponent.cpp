/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

const char *test ="{\"menu\": {	\"id\": \"file\",	\"value\": \"File\",	\"popup\": {	\"menuitem\": [{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}	]}}}";

//==============================================================================
MainContentComponent::MainContentComponent(WorkbenchClient* client_, Settings *settings_):
	client(client_),
	settings(settings_)
{
	//addAndMakeVisible(&socketComponent);
	

	addAndMakeVisible (addressEditor = new TextEditor ("addressEditor"));
	addressEditor->setMultiLine (false);
	addressEditor->setReturnKeyStartsNewLine (false);
	addressEditor->setReadOnly (false);
	addressEditor->setScrollbarsShown (true);
	addressEditor->setCaretVisible (true);
	addressEditor->setPopupMenuEnabled (true);
	addressEditor->setText ("127.0.0.1");
	addressEditor->setColour(TextEditor::outlineColourId, Colours::black);
	addressEditor->setColour(TextEditor::highlightColourId,  Colours::orange);
	addressEditor->setColour(TextEditor::focusedOutlineColourId,  Colours::orange);
	addressEditor->addListener(this);
	
	addAndMakeVisible (portEditor = new TextEditor ("portEditor"));
	portEditor->setMultiLine (false);
	portEditor->setReturnKeyStartsNewLine (false);
	portEditor->setReadOnly (false);
	portEditor->setScrollbarsShown (true);
	portEditor->setCaretVisible (true);
	portEditor->setPopupMenuEnabled (true);
	portEditor->setText ("5678");
	portEditor->setColour(TextEditor::outlineColourId, Colours::black);
	portEditor->setColour(TextEditor::highlightColourId,  Colours::orange);
	portEditor->setColour(TextEditor::focusedOutlineColourId,  Colours::orange);
	portEditor->addListener(this);

	addressEditor->setText(settings->getAddress().toString());
	portEditor->setText(String(settings->getPort()));


	addAndMakeVisible (addressLabel = new Label ("address label",
		TRANS("Address")));
	addressLabel->setFont (Font (15.00f, Font::plain));
	addressLabel->setJustificationType (Justification::centredLeft);
	addressLabel->setEditable (false, false, false);
	addressLabel->setColour (TextEditor::textColourId, Colours::black);
	addressLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (portLabel = new Label ("port label",
		TRANS("Port")));
	portLabel->setFont (Font (15.00f, Font::plain));
	portLabel->setJustificationType (Justification::centredLeft);
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

	client->broadcaster.addChangeListener(this);

	setSize (600, 400);
}

MainContentComponent::~MainContentComponent()
{
	addressEditor = nullptr;
	portEditor = nullptr;
	addressLabel = nullptr;
	portLabel = nullptr;
	connectButton = nullptr;
	disconnectButton = nullptr;
	infoButton = nullptr;
	getTalkersButton = nullptr;
	client->broadcaster.removeChangeListener(this);
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);

}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	addressEditor->setBounds (96, 40, 150, 24);
	portEditor->setBounds (96, 80, 150, 24);
	addressLabel->setBounds (24, 40, 88, 24);
	portLabel->setBounds (24, 80, 88, 24);
	connectButton->setBounds (264, 40, 104, 24);
	juce::Rectangle<int> r(connectButton->getBounds());
	disconnectButton->setBounds(r.translated( r.getWidth() + 5, 0));

	infoButton->setBounds(96, 130, 104, 24);
	r = infoButton->getBounds();
	getTalkersButton->setBounds(r.translated( r.getWidth() + 5, 0));
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
		//[UserButtonCode_connectButton] -- add your button handler code here..

		//bool start = listenSocket.createListener(123456);
#if 0
		listenSocket.bindToPort(123456);
		bool connected = socket.connect( addressEditor->getText(), 123456);
		DBG("connect " << (int)connected);

		zerostruct(in);

		int written = socket.write(out,sizeof(out));
		int ready = listenSocket.waitUntilReady(true, 1000);
		int read = listenSocket.read(in, sizeof(in), false);
		DBG("read " << read << "  ready " << ready);


		bool connected = client->connectToSocket(addressEditor->getText(), portEditor->getText().getIntValue() ,1000);
		DBG("connect " << (int)connected);

		MemoryBlock block(8);
		uint8 *data = (uint8*)block.getData();
		for (size_t i = 0; i < block.getSize(); i++)
		{
			data[i] = (uint8)i;
		}
		bool sent = client->sendMessage(block);
		DBG("sent " << (int)sent);
		//client->disconnect();
#endif
		client->connectToSocket(addressEditor->getText(), portEditor->getText().getIntValue() ,1000);
		return;
		//[/UserButtonCode_connectButton]
	}

	if (buttonThatWasClicked == disconnectButton)
	{
		client->disconnect();
		return;
	}
}

void MainContentComponent::changeListenerCallback( ChangeBroadcaster* /*source*/ )
{
	bool connected = client->isConnected();

	connectButton->setEnabled(!connected);
	infoButton->setEnabled(connected);
	getTalkersButton->setEnabled(connected);
	disconnectButton->setEnabled(connected);
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