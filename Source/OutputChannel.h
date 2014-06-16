/*
  ==============================================================================

	ChannelComponent.h
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/

#ifndef OutputChannel_H_INCLUDED
#define OutputChannel_H_INCLUDED

#include "Channel.h"

//==============================================================================
/*
*/
class Controller;
class OutputChannel   : public Channel
{
public:
	OutputChannel();

	float currentGainLinear;
	float targetGainLinear;
	int mode;
	int sourceDeviceIndex;
	int sourceInputChannel;

	enum
	{
		MODE_NONE = 0,
		MODE_MUTE,
		MODE_TONE,
		MODE_INPUT
	};

	void setMute(bool mute_);
	void setGainLinear(float target_gain);

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputChannel)
	bool mute;
	float gainLinear;
};

#endif  // CHANNEL_H_INCLUDED