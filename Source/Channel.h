/*
  ==============================================================================

	ChannelComponent.h
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/

#ifndef CHANNEL_H_INCLUDED
#define CHANNEL_H_INCLUDED

//==============================================================================
/*
*/
class Controller;
class Channel;

class Channel   
{
public:
	Channel();

	float peakLinear;

protected:
private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Channel)
};

#endif  // CHANNEL_H_INCLUDED