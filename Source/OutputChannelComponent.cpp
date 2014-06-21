#include "base.h"
#include "ChannelComponent.h"
#include "OutputChannelComponent.h"
#include "OutputChannelCallout.h"
#include "Identifiers.h"
#include "OutputChannel.h"

OutputChannelComponent::OutputChannelComponent(int deviceIndex_, ValueTree outputChannelTree_, int channelNumber_, CriticalSection &lock_) :
	ChannelComponent(deviceIndex_, channelNumber_),
	outputChannelTree(outputChannelTree_),
	lock(lock_),
	gainSlider(Slider:: LinearBar, Slider::TextBoxRight),
	muteButton("M", "Mute this output channel"),
	toneFrequencySlider (Slider::LinearBar, Slider::TextBoxRight)
{
	modeButton.addListener(this);
	addAndMakeVisible(&modeButton);

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

void OutputChannelComponent::buttonClicked( Button* button)
{
	if (&muteButton == button)
	{
 		outputChannelTree.setProperty(Identifiers::Mute, muteButton.getToggleState(), nullptr);
 		return;
	}

	if (&modeButton == button)
	{
		showModePopup();
		return;
	}
}

void OutputChannelComponent::sliderValueChanged( Slider* slider )
{
	ScopedLock locker(lock);

	if (&toneFrequencySlider == slider)
	{
 		outputChannelTree.setProperty(Identifiers::ToneFrequency, slider->getValue(), nullptr);
 		return;
	}
	if (&gainSlider == slider)
	{
 		outputChannelTree.setProperty(Identifiers::GainDecibels, slider->getValue(), nullptr);
 		return;
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
	r.setWidth(proportionOfWidth(0.24f));
	toneFrequencySlider.setBounds(r);
	r.translate( r.getWidth() + 5, 0);
	r.setWidth( getWidth() - r.getX() - 5);
	gainSlider.setBounds(r);

	int w = 20;
	int y = (getHeight() / 2 - r.getHeight()) / 2;
	muteButton.setBounds( getWidth() - w - 5, y, r.getHeight(), w);

	updateMode();
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

	ValueTree devicesTree(outputChannelTree.getParent().getParent().getParent());
	for (int i = 0; i < devicesTree.getNumChildren(); i++)
	{
		ValueTree otherDeviceTree(devicesTree.getChild(i));
		ValueTree inputChannelsTree(otherDeviceTree.getChildWithName(Identifiers::Input));
		int channelCount = inputChannelsTree.getProperty(Identifiers::CurrentChannelCount, 0);
		if (channelCount <= 0)
		{
			continue;
		}

		String otherDeviceName(otherDeviceTree[Identifiers::DeviceName]);

		PopupMenu deviceMenu;
		bool submenuTicked = false;
		for (int input = 0; input < channelCount; ++input)
		{
			ValueTree inputChannelTree(inputChannelsTree.getChild(input));
			bool ticked = OutputChannel::MODE_INPUT == mode;
			
			ticked &= otherDeviceName == outputChannelTree[Identifiers::SourceDeviceName].toString();
			ticked &= input == (int)outputChannelTree[Identifiers::SourceChannel];
			submenuTicked |= ticked;

			int menuID = makeMenuID(i, input);
			//DBG(" showModePopup device:" << i << " menuID" << menuID);
			deviceMenu.addItem(menuID, inputChannelTree[Identifiers::Name], true, ticked);
		}

		menu.addSubMenu(otherDeviceName,deviceMenu, true, blank, submenuTicked);
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
			ValueTree devicesTree(that->outputChannelTree.getParent().getParent().getParent());
			
			sourceDeviceIndex = getDeviceIndexFromMenuID(menuID);
			if (sourceDeviceIndex >= 0)
			{
				int sourceChannel = getChannelFromMenuID(menuID);

				String sourceDeviceName(devicesTree.getChild(sourceDeviceIndex)[Identifiers::DeviceName]);
				that->outputChannelTree.setProperty(Identifiers::SourceDeviceName, sourceDeviceName, nullptr);
				that->outputChannelTree.setProperty(Identifiers::SourceChannel, sourceChannel, nullptr);
				that->outputChannelTree.setProperty(Identifiers::Mode, OutputChannel::MODE_INPUT, nullptr);
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
 			String inputDeviceName(outputChannelTree[Identifiers::SourceDeviceName]);
 			int inputChannel = outputChannelTree[Identifiers::SourceChannel];

			ValueTree deviceTree;
			ValueTree devicesTree(outputChannelTree.getParent().getParent().getParent());
			for (int deviceIndex = 0; deviceIndex < devicesTree.getNumChildren(); deviceIndex++)
			{
				if (devicesTree.getChild(deviceIndex).getProperty(Identifiers::DeviceName) == inputDeviceName)
				{
					deviceTree = devicesTree.getChild(deviceIndex);
					break;
				}
			}
			if (false == deviceTree.isValid())
			{
				break;
			}

			ValueTree inputChannelsTree(deviceTree.getChildWithName(Identifiers::Input));
			ValueTree inputChannelTree(inputChannelsTree.getChild(inputChannel));

			//dumpTree(inputDeviceTree);
			
			if (inputChannelTree.isValid())
			{
				tooltip = inputDeviceName;
				tooltip += "\n" + inputChannelTree[Identifiers::Name].toString();
				text = inputDeviceName + " / " + inputChannelTree[Identifiers::Name].toString();
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
		//DBG("unknown mode " << mode);
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
