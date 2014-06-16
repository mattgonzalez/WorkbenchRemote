#pragma once
#include "ChannelViewport.h"

class Controller;

class InputChannelViewport : public ChannelViewport
{
public:
	InputChannelViewport(int const deviceIndex_, ValueTree channelsTree_);

	virtual void buildChannelComponents();
	void timerCallback();
};

