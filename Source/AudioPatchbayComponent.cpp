#include "AudioPatchBayComponent.h"
#include "Identifiers.h"
#include "MainComponent.h"

AudioPatchbayComponent::AudioPatchbayComponent(MainContentComponent* mainComponent_, AudioPatchbayClient* client_, Settings *settings_):
	mainComponent(mainComponent_),
	client(client_),
	settings(settings_),
	tree(settings_->getAudioDevicesTree())
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

	addAndMakeVisible (getAvailableAudioDevicesButton = new TextButton ("Get Available Devices"));
	getAvailableAudioDevicesButton->addListener (this);
	getAvailableAudioDevicesButton->setEnabled(false);

	addAndMakeVisible (getCurrentAudioDevicesButton = new TextButton ("Get Current Devices"));
	getCurrentAudioDevicesButton->addListener (this);
	getCurrentAudioDevicesButton->setEnabled(false);

	addAndMakeVisible (getInputButton = new TextButton ("Get Inputs"));
	getInputButton->addListener (this);
	getInputButton->setEnabled(false);

	addAndMakeVisible (getOutputButton = new TextButton ("Get Outputs"));
	getOutputButton->addListener (this);
	getOutputButton->setEnabled(false);

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
	tree.addListener(this);

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	setSize (600, 400);
}

AudioPatchbayComponent::~AudioPatchbayComponent()
{
	updatePort();

	tabs = nullptr;
	portEditor = nullptr;
	portLabel = nullptr;
	connectButton = nullptr;
	disconnectButton = nullptr;
	infoButton = nullptr;
	getAvailableAudioDevicesButton = nullptr;
	getCurrentAudioDevicesButton = nullptr;
	client->lastMessageSent.removeListener(this);
	client->lastMessageReceived.removeListener(this);
	client->changeBroadcaster.removeChangeListener(this);
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
	infoButton->setBounds(20, y, 150, 24);
	r = infoButton->getBounds();
	getAvailableAudioDevicesButton->setBounds(r.translated( r.getWidth() + 5, 0));
	r = getAvailableAudioDevicesButton->getBounds();
	getCurrentAudioDevicesButton->setBounds(r.translated(r.getWidth() + 5, 0));
	r = getCurrentAudioDevicesButton->getBounds();
	getInputButton->setBounds(r.translated(r.getWidth() + 5, 0));
	r = getInputButton->getBounds();
	getOutputButton->setBounds(r.translated(r.getWidth() + 5, 0));
	
	y = infoButton->getBottom() + 10;
	int w = proportionOfWidth(0.75f) - 20;
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

	if (buttonThatWasClicked == getAvailableAudioDevicesButton)
	{
		client->getAvailableAudioDevices();	
	}

	if (buttonThatWasClicked == getCurrentAudioDevicesButton)
	{
		client->getCurrentAudioDevices();	
		return;
	}

	if (buttonThatWasClicked == getInputButton)
	{
		ValueTree audioDevicesTree(settings->getAudioDevicesTree());
		for (int i = 0; i < audioDevicesTree.getNumChildren(); ++i)
		{
			ValueTree deviceTree(audioDevicesTree.getChild(i));
			String name(deviceTree[Identifiers::DeviceName].toString());
			if (name.isEmpty())
				continue;

			ValueTree inputsTree(deviceTree.getChildWithName(Identifiers::Input));
			int channels = inputsTree[Identifiers::MaxChannelCount];
			for (int channel = 0; channel < channels; ++channel)
			{
				client->getInputChannel(name, channel);
			}
		}
		return;
	}

	if (buttonThatWasClicked == getOutputButton)
	{
		ValueTree audioDevicesTree(settings->getAudioDevicesTree());
		for (int i = 0; i < audioDevicesTree.getNumChildren(); ++i)
		{
			ValueTree deviceTree(audioDevicesTree.getChild(i));
			String name(deviceTree[Identifiers::DeviceName].toString());
			if (name.isEmpty())
				continue;

			ValueTree outputsTree(deviceTree.getChildWithName(Identifiers::Output));
			int channels = outputsTree[Identifiers::MaxChannelCount];
			for (int channel = 0; channel < channels; ++channel)
			{
				client->getOutputChannel(name, channel);
			}
		}
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
	enableControls();
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
		portEditor->setText(tree[Identifiers::Port].toString());
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
	tree.setProperty(Identifiers::Port, port, nullptr);
	DBG("AudioPatchbayComponent::updatePort() " << port);
}

void AudioPatchbayComponent::actionListenerCallback( const String& message )
{

}

void AudioPatchbayComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	//DBG("AudioPatchbayComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());

	if (Identifiers::AvailableAudioDevices == property)
	{
		enableControls();
		return;
	}
}

void AudioPatchbayComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
	DBG("AudioPatchbayComponent::valueTreeChildAdded " << parentTree.getType().toString() << " " << childWhichHasBeenAdded.getType().toString());

	if (Identifiers::AudioDevices == parentTree.getType())
	{
		int index = childWhichHasBeenAdded.getProperty(Identifiers::Index);
		DeviceComponent* component = new DeviceComponent(childWhichHasBeenAdded, settings->lock);
		tabs->addTab("Device " + String(index + 1), Colours::white, component, false, index);
		deviceComponents.add(component);
	}
	
	enableControls();
}

void AudioPatchbayComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int )
{
	DBG("AudioPatchbayComponent::valueTreeChildRemoved " << parentTree.getType().toString() << " " << childWhichHasBeenRemoved.getType().toString());

	if (Identifiers::AudioDevices == parentTree.getType())
	{
		tabs->removeTab(childWhichHasBeenRemoved.getProperty(Identifiers::Index));
		for (int i = 0; i < deviceComponents.size(); ++i)
		{
			if (deviceComponents[i]->deviceTree == childWhichHasBeenRemoved)
			{
				deviceComponents.remove(i);
				break;
			}
		}
	}

	enableControls();
}

void AudioPatchbayComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int )
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

	getAvailableAudioDevicesButton->setEnabled(connected);

	ValueTree audioDevicesTree(settings->getAudioDevicesTree());
	bool enabled = audioDevicesTree.hasProperty(Identifiers::AvailableAudioDevices);
	getCurrentAudioDevicesButton->setEnabled(enabled);

	enabled = tabs->getNumTabs() != 0;
	getInputButton->setEnabled(enabled);
	getOutputButton->setEnabled(enabled);
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