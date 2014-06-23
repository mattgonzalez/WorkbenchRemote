#include "base.h"
#include "InputChannelComponent.h"

InputChannelComponent::InputChannelComponent(int deviceIndex_, int channelNumber_) :
	ChannelComponent(deviceIndex_,channelNumber_)
{
	//channelNameLabel.setColour(Label::outlineColourId,Colours::aquamarine);
}

InputChannelComponent::~InputChannelComponent()
{
}

void InputChannelComponent::timerCallback()
{
	//float peakLinear = controller->deviceManager.getDevice(deviceIndex)->processor.inputs[channelNumber].peakLinear;
	//meter.setLinear (peakLinear);
}

void InputChannelComponent::resized()
{
	channelNameLabel.setBoundsRelative(0.05f, 0.025f, 0.9f, 0.45f);
	//meter.setBoundsRelative(0.05f, 0.525f, 0.8f, 0.45f);
}
