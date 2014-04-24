/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

const char *test ="{\"menu\": {	\"id\": \"file\",	\"value\": \"File\",	\"popup\": {	\"menuitem\": [{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}	]}}}";

//==============================================================================
MainContentComponent::MainContentComponent(WorkbenchClient* client_):
	client(client_)
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
	
	addAndMakeVisible (portEditor = new TextEditor ("portEditor"));
	portEditor->setMultiLine (false);
	portEditor->setReturnKeyStartsNewLine (false);
	portEditor->setReadOnly (false);
	portEditor->setScrollbarsShown (true);
	portEditor->setCaretVisible (true);
	portEditor->setPopupMenuEnabled (true);
	portEditor->setText ("5678");

	addAndMakeVisible (label = new Label ("new label",
		TRANS("Address")));
	label->setFont (Font (15.00f, Font::plain));
	label->setJustificationType (Justification::centredLeft);
	label->setEditable (false, false, false);
	label->setColour (TextEditor::textColourId, Colours::black);
	label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (connectButton = new TextButton ("connectButton"));
	connectButton->setButtonText (TRANS("Connect"));
	connectButton->addListener (this);

	addAndMakeVisible (disconnectButton = new TextButton ("disconnectButton"));
	disconnectButton->setButtonText (TRANS("Disconnect"));
	disconnectButton->addListener (this);

	setSize (600, 400);
}

MainContentComponent::~MainContentComponent()
{
	addressEditor = nullptr;
	portEditor = nullptr;
	label = nullptr;
	connectButton = nullptr;
	disconnectButton = nullptr;
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
	label->setBounds (24, 40, 88, 24);
	connectButton->setBounds (264, 40, 104, 24);
	disconnectButton->setBounds (264, 80, 104, 24);
}

void MainContentComponent::buttonClicked (Button* buttonThatWasClicked)
{
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
#endif

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
		return;
		//[/UserButtonCode_connectButton]
	}

	if (buttonThatWasClicked == disconnectButton)
	{
		DBG("disconnected");
		client->disconnect();
	}
}
