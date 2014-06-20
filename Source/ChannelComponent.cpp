/*
  ==============================================================================

	ChannelComponent.cpp
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/

#include "base.h"
#include "ChannelComponent.h"

//==============================================================================

ChannelComponent::ChannelComponent(int deviceIndex_, int channelNumber_) :
	deviceIndex(deviceIndex_),
	channelNumber(channelNumber_)
{
	addAndMakeVisible(&channelNameLabel);
	//addAndMakeVisible(&meter);

	channelNameLabel.setInterceptsMouseClicks(false,false);
	meter.setInterceptsMouseClicks(false,false);
}

ChannelComponent::~ChannelComponent()
{
}

void ChannelComponent::paint (Graphics& g)
{
	Colour c(Colours::white);
// 	if (channelNumber & 1)
// 		c = Colour(0xffe3e3e3);
	
	juce::Rectangle<float> r(getLocalBounds().toFloat());
	g.setColour(c);
	g.fillRoundedRectangle(r, r.getHeight() * 0.125f);
}
