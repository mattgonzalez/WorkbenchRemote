#pragma once
#include "ChannelViewport.h"


class InputChannelViewport : public ChannelViewport
{
public:
	InputChannelViewport(int const deviceIndex_, ValueTree channelsTree_, CriticalSection &lock_);

	virtual void buildChannelComponents();
	void timerCallback();
};

