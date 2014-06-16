#include "base.h"
#include "ChannelComponent.h"
#include "OutputChannelComponent.h"
#include "OutputChannelCallout.h"
#include "Identifiers.h"

OutputChannelComponent::OutputChannelComponent(int deviceIndex_, ValueTree outputChannelTree_, int channelNumber_) :
	ChannelComponent(channelNumber_),
	outputChannelTree(outputChannelTree_),
	gainSlider(Slider:: LinearBar, Slider::TextBoxRight),
	muteButton("M", "Mute this output channel"),
	toneFrequencySlider (Slider::LinearBar, Slider::TextBoxRight)
{
#if 0
	modeCombo.addListener(this);
	addAndMakeVisible(&modeCombo);
	fillModeCombo();
#else
	modeButton.addListener(this);
	addAndMakeVisible(&modeButton);
#endif

	outputChannelTree.addListener(this);

	gainSlider.setTextValueSuffix(" dB");
	gainSlider.setRange(-60.0, 0.0, 0.1);
	gainSlider.setDoubleClickReturnValue(true, 0.0);
	gainSlider.addListener(this);
	addAndMakeVisible(&gainSlider);

	muteButton.setClickingTogglesState(true);
	muteButton.addListener(this);
	muteButton.setColour(TextButton:: buttonOnColourId, Colours::red);
	addAndMakeVisible(&muteButton);

	toneFrequencySlider.setTextValueSuffix(" Hz");
	toneFrequencySlider.setRange(20.0, 20000.0, 1);
	toneFrequencySlider.setDoubleClickReturnValue(true, 200.0);
	toneFrequencySlider.setSkewFactor(0.5);
	toneFrequencySlider.addListener(this);
	addAndMakeVisible(&toneFrequencySlider);

	updateMode();
	
	// Need to pull in tree
	//devicesTree.addListener(this);
}

OutputChannelComponent::~OutputChannelComponent()
{

}

void OutputChannelComponent::timerCallback()
{
	//float peakLinear = controller->deviceManager.getDevice(deviceIndex)->processor.outputs[channelNumber].peakLinear;

	//meter.setLinear (peakLinear);
}

void OutputChannelComponent::mouseUp( const MouseEvent& event )
{
#if 0
	OutputChannelCallout* content = new OutputChannelCallout(controller, channelTree);

	juce::Rectangle<int> b(getBoundsInParent());
	Component* parent = getParentComponent();
	b.setWidth(b.getWidth() >> 2);
	b.reduce(0, b.getHeight() >> 2);
	CallOutBox & box (CallOutBox::launchAsynchronously (content, b, parent));
	box.setAlpha(0.9f);
#endif
}

void OutputChannelComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (treeWhosePropertyHasChanged == outputChannelTree)
	{
		updateMode();
		return;
	}

	if (Identifiers::DeviceName == property)
	{
		updateMode();
		return;
	}
}

void OutputChannelComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void OutputChannelComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void OutputChannelComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void OutputChannelComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

void OutputChannelComponent::comboBoxChanged( ComboBox* comboBoxThatHasChanged )
{
#if 0
	int mode = modeCombo.getSelectedId();
	if (mode >= OutputChannel::MODE_INPUT)
	{
		int source = mode - OutputChannel::MODE_INPUT;
		outputChannelTree.setProperty(Identifiers::Source, source, nullptr);
		outputChannelTree.setProperty(Identifiers::Mode, OutputChannel::MODE_INPUT, nullptr);
		return;
	}

	outputChannelTree.setProperty(Identifiers::Source, DeviceManager::NO_SELECTION, nullptr);
	outputChannelTree.setProperty(Identifiers::Mode, mode, nullptr);
#endif
}

void OutputChannelComponent::fillModeCombo()
{
#if 0
	modeCombo.clear();
	modeCombo.addItem("Muted", OutputChannel::MODE_MUTE);
	modeCombo.addItem("Tone", OutputChannel::MODE_TONE);
	ValueTree devicesTree(controller->settings.root.getChildWithName(Identifiers::AudioDevices));
	for (int i = 0; i < devicesTree.getNumChildren(); i++)
	{
		ValueTree deviceTree(devicesTree.getChild(i));
		ValueTree inputChannelsTree(deviceTree.getChildWithName(Identifiers::Input));
		int channelCount = inputChannelsTree.getProperty(Identifiers::CurrentChannelCount, 0);
		if (channelCount <= 0)
		{
			continue;
		}
		modeCombo.addSeparator();
		modeCombo.addSectionHeading(deviceTree.getProperty(Identifiers::DeviceName));
		for (int input = 0; input < channelCount; ++input)
		{
			ValueTree inputChannelTree(inputChannelsTree.getChild(input));
			modeCombo.addItem(inputChannelTree[Identifiers::Name], OutputChannel::MODE_INPUT + input + (i * 1000));
		}
#if 0
		int mode = outputChannelTree[Identifiers::Mode];
		if (mode >= OutputChannel::MODE_INPUT)
		{
			int source = outputChannelTree[Identifiers::Source];
			mode += source;
		}
		modeCombo.setSelectedId(mode, dontSendNotification);
#endif
	}
#endif
}

void OutputChannelComponent::buttonClicked( Button* button)
{
	if (&muteButton == button)
	{
// 		outputChannelTree.setProperty(Identifiers::Mute, muteButton.getToggleState(), nullptr);
// 		return;
	}

	if (&modeButton == button)
	{
		showModePopup();
		return;
	}
}

void OutputChannelComponent::sliderValueChanged( Slider* slider )
{
	if (&toneFrequencySlider == slider)
	{
// 		outputChannelTree.setProperty(Identifiers::ToneFrequency, slider->getValue(), nullptr);
// 		return;
	}
	if (&gainSlider == slider)
	{
// 		outputChannelTree.setProperty(Identifiers::GainDecibels, slider->getValue(), nullptr);
// 		return;
	}
}

void OutputChannelComponent::resized()
{
	channelNameLabel.setBoundsRelative(0.05f, 0.025f, 0.45f, 0.45f);
	meter.setBoundsRelative(0.5f, 0.025f, 0.45f, 0.45f);

	modeButton.setBoundsRelative(0.05f, 0.525f, 0.35f, 0.45f);
	
	juce::Rectangle<int> r(modeButton.getBounds());
	r.reduce(0,2);
	r.translate( r.getWidth() + 5, 0);
	float w = 0.24f;
	r.setWidth(proportionOfWidth(w));
	toneFrequencySlider.setBounds(r);
	r.translate( r.getWidth() + 5, 0);
	gainSlider.setBounds(r);
	r.translate( r.getWidth() + 5, 0);
	r.setWidth(30);
	if (r.getRight() > getWidth() - 2)
	{
		r.setWidth(getWidth() - 2 - r.getX());
	}
	muteButton.setBounds(r);

	updateMode();
}

void OutputChannelComponent::valueChanged( Value& value )
{
	if (value.refersToSameSourceAs(currentInputChannelCount))
	{
		fillModeCombo();
		return;
	}
}

int OutputChannelComponent::getDeviceIndexFromMenuID( int const menuID )
{
	return (menuID >> 16) - 1;
}

int OutputChannelComponent::getChannelFromMenuID( int const menuID )
{
	return menuID & 0xffff;
}

int OutputChannelComponent::makeMenuID( int const deviceIndex, int const channel )
{
	return ((deviceIndex + 1) << 16) + channel;
}

void OutputChannelComponent::showModePopup()
{
	PopupMenu menu;
	int mode = outputChannelTree[Identifiers::Mode];
	Image blank;

	menu.addItem( OutputChannel::MODE_MUTE, "Muted", true, OutputChannel::MODE_MUTE == mode);
	menu.addItem( OutputChannel::MODE_TONE, "Tone", true, OutputChannel::MODE_TONE == mode);

	ValueTree devicesTree(controller->settings.root.getChildWithName(Identifiers::AudioDevices));
	for (int i = 0; i < devicesTree.getNumChildren(); i++)
	{
		ValueTree otherDeviceTree(devicesTree.getChild(i));
		ValueTree inputChannelsTree(otherDeviceTree.getChildWithName(Identifiers::Input));
		int channelCount = inputChannelsTree.getProperty(Identifiers::CurrentChannelCount, 0);
		if (channelCount <= 0)
		{
			continue;
		}

		var otherDeviceName (otherDeviceTree[Identifiers::DeviceName]);
		String otherDeviceFriendlyName(controller->deviceManager.getFriendlyName(otherDeviceName));

		PopupMenu deviceMenu;
		bool submenuTicked = false;
		for (int input = 0; input < channelCount; ++input)
		{
			ValueTree inputChannelTree(inputChannelsTree.getChild(input));
			bool ticked = OutputChannel::MODE_INPUT == mode;
			
			ticked &= otherDeviceName == outputChannelTree[Identifiers::SourceDeviceName];
			ticked &= input == (int)outputChannelTree[Identifiers::SourceChannel];
			submenuTicked |= ticked;

			int menuID = makeMenuID(i, input);
			//DBG(" showModePopup device:" << i << " menuID" << menuID);
			deviceMenu.addItem(menuID, inputChannelTree[Identifiers::Name], true, ticked);
		}

		menu.addSubMenu(otherDeviceFriendlyName,deviceMenu, true, blank, submenuTicked);
	}

	menu.showMenuAsync (PopupMenu::Options().withTargetComponent (&modeButton)
		.withMinimumWidth (modeButton.getWidth())
		.withMaximumNumColumns (1)
		.withStandardItemHeight (jlimit (12, 24, getHeight())),
		ModalCallbackFunction::forComponent (popupMenuFinishedCallback, this));
}

void OutputChannelComponent::popupMenuFinishedCallback( int menuID, OutputChannelComponent* that)
{
	//DBG("OutputChannelComponent::popupMenuFinishedCallback " << menuID);

	switch (menuID)
	{
	case 0:
		// user dismissed the menu
		break;

	case OutputChannel::MODE_MUTE:
	case OutputChannel::MODE_TONE:
		that->outputChannelTree.setProperty(Identifiers::Mode, menuID, nullptr);
		break;

	default:
		{
			int sourceDeviceIndex;
			ValueTree devicesTree(that->controller->settings.root.getChildWithName(Identifiers::AudioDevices));
			
			sourceDeviceIndex = getDeviceIndexFromMenuID(menuID);
			if (sourceDeviceIndex >= 0)
			{
				int sourceChannel = getChannelFromMenuID(menuID);

				String sourceDeviceFriendlyName(devicesTree.getChild(sourceDeviceIndex)[Identifiers::DeviceName]);
				String sourceDeviceActualName(that->controller->deviceManager.getActualName(sourceDeviceFriendlyName));
				that->outputChannelTree.setProperty(Identifiers::SourceDeviceName, sourceDeviceActualName, nullptr);
				that->outputChannelTree.setProperty(Identifiers::SourceChannel, sourceChannel, nullptr);
				that->outputChannelTree.setProperty(Identifiers::Mode, OutputChannel::MODE_INPUT, nullptr);
				that->outputChannelTree.setProperty(Identifiers::SourceDeviceIndex, sourceDeviceIndex, nullptr);
			}
		}
		break;
	}

	that->modeButton.grabKeyboardFocus();
}

void OutputChannelComponent::setModeButtonText()
{
	String text;
	String tooltip;
	Colour c(Colours::black);

	int mode = outputChannelTree[Identifiers::Mode];
	
	switch (mode)
	{
	case OutputChannel::MODE_MUTE:
		text = "Muted";
		break;

	case OutputChannel::MODE_TONE:
		text = "Tone";
		break;

	case OutputChannel::MODE_INPUT:
		{
			String inputDeviceActualName(outputChannelTree[Identifiers::SourceDeviceName]);
			String inputDeviceFriendlyName(controller->deviceManager.getFriendlyName(inputDeviceActualName));
			int inputChannel = outputChannelTree[Identifiers::SourceChannel];

			ValueTree inputDeviceTree(controller->settings.getDeviceTree(inputDeviceActualName));
			ValueTree inputDeviceInputsTree(inputDeviceTree.getChildWithName(Identifiers::Input));
			ValueTree inputChannelTree(inputDeviceInputsTree.getChild(inputChannel));

			//dumpTree(inputDeviceTree);

			if (inputChannelTree.isValid())
			{
				tooltip = inputDeviceFriendlyName;
				tooltip += "\n" + inputChannelTree[Identifiers::Name].toString();
				text = inputDeviceFriendlyName + " / " + inputChannelTree[Identifiers::Name].toString();
			}
			else
			{
				text = modeButton.getButtonText();
				c = Colours::grey;
			}

			//DBG("MODE_INPUT " + text);
		}
		break;

	default:
		DBG("unknown mode " << mode);
		//dumpTree(outputChannelTree);
		break;
	}

	modeButton.setButtonText(text);
	modeButton.setColour(ComboBox::textColourId, c);
	modeButton.setTooltip(tooltip);
	modeButton.repaint();
}

void OutputChannelComponent::updateMode()
{
	gainSlider.setValue(outputChannelTree.getProperty(Identifiers::GainDecibels, 0), dontSendNotification);
	muteButton.setToggleState(outputChannelTree.getProperty(Identifiers::Mute, 0), dontSendNotification);
	toneFrequencySlider.setValue(outputChannelTree.getProperty(Identifiers::ToneFrequency, 200.0), dontSendNotification);

	int mode = outputChannelTree[Identifiers::Mode];
	toneFrequencySlider.setVisible(OutputChannel::MODE_TONE == mode);

	int w = toneFrequencySlider.getX() - 5 - modeButton.getX();
	if (OutputChannel::MODE_INPUT == mode)
		w = gainSlider.getX() - 5 - modeButton.getX();
	modeButton.setSize( w, modeButton.getHeight());

	bool gainVisible = OutputChannel::MODE_MUTE != mode;
	gainSlider.setVisible(gainVisible);
	muteButton.setVisible(gainVisible);

	setModeButtonText();
}


OutputChannelComponent::ModeButton::ModeButton() :
	Button(String::empty)
{
}


void OutputChannelComponent::ModeButton::paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown )
{
	int buttonX, buttonY, buttonW, buttonH;

	g.fillAll (findColour (ComboBox::backgroundColourId));

	const Colour outlineColor (findColour (ComboBox::outlineColourId));

	if (isEnabled())
	{
		if (hasKeyboardFocus(false))
		{
			g.fillAll(Colours::lightgrey.withAlpha(0.1f));
		}

		g.setColour (outlineColor);
		g.drawRect (0, 0, getWidth(), getHeight(), 1);
	}
	else
	{
		g.setColour (outlineColor.withMultipliedAlpha(0.3f));
		g.drawRect (0, 0, getWidth(), getHeight());
	}


	const float arrowX = 0.3f;
	const float arrowH = 0.2f;

	Path p;
	buttonW = /*JUCE_LIVE_CONSTANT*/(15);
	buttonX = getWidth() - buttonW - 1;
	buttonY = proportionOfHeight(0.1f);
	buttonH = proportionOfHeight(0.8f);

	p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
		buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
		buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);

	p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
		buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
		buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);

	float alpha = isEnabled() ? 1.0f : 0.3f;
	g.setColour (findColour (ComboBox::arrowColourId).withMultipliedAlpha (alpha));
	g.fillPath (p);

	const Colour textColour (findColour (ComboBox::textColourId).withMultipliedAlpha(alpha));
	g.setColour(textColour);
	g.drawText(getButtonText(), 0, 0, buttonX, getHeight(), Justification::centred, true);
}
