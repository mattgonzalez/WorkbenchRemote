/*
  ==============================================================================

	ChannelComponent.h
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/

#ifndef INPUTCHANNEL_H_INCLUDED
#define INPUTCHANNEL_H_INCLUDED

#include "Channel.h"

//==============================================================================
/*
*/

class InputChannel   : public Channel
{
public:
	InputChannel();

private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InputChannel)
};

#endif  // CHANNEL_H_INCLUDED