#pragma once

class Controller;

class OutputChannelViewport : public ChannelViewport
{
public:
	OutputChannelViewport(int const deviceIndex_, ValueTree channelsTree_);

	virtual void buildChannelComponents();
	void timerCallback();
};

