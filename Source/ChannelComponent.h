#include "MeterComponent.h"
/*
  ==============================================================================

	ChannelComponent.h
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/

#ifndef CHANNELCOMPONENT_H_INCLUDED
#define CHANNELCOMPONENT_H_INCLUDED

#include "MeterComponent.h"


//==============================================================================
/*
*/
class ChannelComponent : public Component
{
public:
	ChannelComponent(int deviceIndex_, int channelNumber_);
	~ChannelComponent();

	virtual void timerCallback() = 0;

	void paint (Graphics&);

	Label channelNameLabel;
	//MeterComponent meter;

protected:
	int const deviceIndex;
	int const channelNumber;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelComponent)
};

#endif  // CHANNELCOMPONENT_H_INCLUDED