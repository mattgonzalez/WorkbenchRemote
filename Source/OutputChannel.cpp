#include "base.h"
#include "OutputChannel.h"

OutputChannel::OutputChannel()
{
	targetGainLinear = 1.0f;
	currentGainLinear = 1.0f;
	gainLinear = 1.0f;
	mute = false;
	mode = MODE_NONE;
	sourceDeviceIndex = -1;
	sourceInputChannel = -1;
}

void OutputChannel::setMute( bool mute_ )
{
	mute = mute_;

	setGainLinear(gainLinear);
}

void OutputChannel::setGainLinear( float target_gain )
{
	gainLinear = target_gain;
	if (mute == true)
	{
		targetGainLinear = 0.0f;
	}
	else
	{
		targetGainLinear = target_gain;
	}
}

