#include "base.h"
#include "OutputChannelCallout.h"
#include "Identifiers.h"
#include "OutputChannel.h"

OutputChannelCallout::OutputChannelCallout(ValueTree channelTree_ ) : 
	channelTree(channelTree_),
	gainSlider(Slider:: LinearHorizontal, Slider:: TextBoxRight),
	muteButton("M", "Mute this output channel"),
	toneFrequencySlider (Slider::LinearHorizontal, Slider:: TextBoxRight),
	allOutputsToggle("Use same settings for all output channels")
{
	setSize(400,200);

	topLabel.setJustificationType (Justification::centred);
	Font f(topLabel.getFont());
	f.setBold(true);
	topLabel.setFont(f);
	addAndMakeVisible(& topLabel);
	topLabel.setText(channelTree[Identifiers::Name], dontSendNotification);

	fillModeCombo();

	int mode = channelTree[Identifiers::Mode];
	if (mode >= OutputChannel::MODE_INPUT)
	{
		int source = channelTree[Identifiers::Source];
		mode += source;
	}
	modeCombo.setSelectedId(mode, dontSendNotification);
	modeCombo.addListener(this);
	addAndMakeVisible(&modeCombo);

	gainSlider.setTextValueSuffix(" dB");
	gainSlider.setRange(-60.0, 0.0, 0.1);
	gainSlider.setDoubleClickReturnValue(true, 0.0);
	gainSlider.addListener(this);
	gainSlider.setValue(channelTree.getProperty(Identifiers::GainDecibels, 0), dontSendNotification);
	addAndMakeVisible(&gainSlider);

	muteButton.setClickingTogglesState(true);
	muteButton.addListener(this);
	muteButton.setColour(TextButton:: buttonOnColourId, Colours::red);
	muteButton.setToggleState(channelTree.getProperty(Identifiers::Mute, 0), dontSendNotification);
	addAndMakeVisible(&muteButton);

	toneFrequencySlider.setTextValueSuffix(" Hz");
	toneFrequencySlider.setRange(20.0, 20000.0, 1);
	toneFrequencySlider.setDoubleClickReturnValue(true, 200.0);
	toneFrequencySlider.setSkewFactor(0.5);
	toneFrequencySlider.addListener(this);
	toneFrequencySlider.setValue(channelTree.getProperty(Identifiers::ToneFrequency, 200.0), dontSendNotification);
	addAndMakeVisible(&toneFrequencySlider);

	allOutputsToggle.addListener(this);
	//addAndMakeVisible(&allOutputsToggle);
}

void OutputChannelCallout::paint( Graphics& g )
{
	g.fillAll(Colours::white);
}

void OutputChannelCallout::resized()
{
	int x,w,modeComboW, buttonW;
	int h = 40;
	int offset = 5;
	topLabel.setBounds(0, 0, getWidth(), h);

	modeComboW = proportionOfWidth(0.8f);
	buttonW = 40;
	w = modeComboW - buttonW - offset;
	h = 30;
	x = (getWidth() - modeComboW) >> 1;
	juce::Rectangle<int> r( x, topLabel.getBottom() + offset, w, h);
	gainSlider.setBounds(r);

	juce::Rectangle<int> rMute(r);
	rMute.translate( r.getWidth() + offset*2, 0);
	rMute.reduce( 0, /*JUCE_LIVE_CONSTANT*/(5));
	rMute.setWidth(30);
	muteButton.setBounds( rMute);

	r.translate(0, r.getHeight() + offset*4);
	r.setWidth(modeComboW);
	modeCombo.setBounds(r);

	r.translate(0, h + offset);
	toneFrequencySlider.setBounds(r);

	r.translate(0, h + offset);
	allOutputsToggle.setBounds(r);
}

void OutputChannelCallout::timerCallback()
{

}

void OutputChannelCallout::comboBoxChanged( ComboBox* comboBoxThatHasChanged )
{
	int mode = modeCombo.getSelectedId();
	if (mode >= OutputChannel::MODE_INPUT)
	{
		int source = mode - OutputChannel::MODE_INPUT;
		channelTree.setProperty(Identifiers::Source, source, nullptr);
		channelTree.setProperty(Identifiers::Mode, OutputChannel::MODE_INPUT, nullptr);
		return;
	}

	channelTree.setProperty(Identifiers::Source, -1, nullptr);
	channelTree.setProperty(Identifiers::Mode, mode, nullptr);
}

void OutputChannelCallout::sliderValueChanged( Slider* slider )
{
	if (&toneFrequencySlider == slider)
	{
		channelTree.setProperty(Identifiers::ToneFrequency, slider->getValue(), nullptr);
		return;
	}
	if (&gainSlider == slider)
	{
		channelTree.setProperty(Identifiers::GainDecibels, slider->getValue(), nullptr);
		return;
	}
}

void OutputChannelCallout::valueChanged( Value& value )
{

}

void OutputChannelCallout::fillModeCombo()
{
	modeCombo.clear();
	modeCombo.addItem("Muted", OutputChannel::MODE_MUTE);
	modeCombo.addItem("Tone", OutputChannel::MODE_TONE);

	//ValueTree inputDeviceTree(controller->settings.root.getChildWithName(Identifiers::AudioInput));
	//ValueTree inputChannelsTree(inputDeviceTree.getChildWithName(Identifiers::Channels));

// 	int channelCount = inputChannelsTree.getProperty(Identifiers::CurrentChannelCount, 0);
// 
// 	for (int input = 0; input < channelCount; ++input)
// 	{
// 		ValueTree inputChannelTree(inputChannelsTree.getChild(input));
// 		modeCombo.addItem(inputChannelTree[Identifiers::Name], OutputChannel::MODE_INPUT + input);
// 	}
}

void OutputChannelCallout::buttonClicked( Button* )
{
	channelTree.setProperty(Identifiers::Mute, muteButton.getToggleState(), nullptr);
}
