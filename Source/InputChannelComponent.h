#include "ChannelComponent.h"
/*
  ==============================================================================

	ChannelComponent.h
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/

#ifndef INPUTCHANNELCOMPONENT_H_INCLUDED
#define INPUTCHANNELCOMPONENT_H_INCLUDED


//==============================================================================
/*
*/
class InputChannelComponent : public ChannelComponent
{
public:
	InputChannelComponent(int deviceIndex_, int channelNumber_);
	~InputChannelComponent();

	virtual void timerCallback();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputChannelComponent)

		virtual void resized();

};


#endif  // INPUTCHANNELCOMPONENT_H_INCLUDED
