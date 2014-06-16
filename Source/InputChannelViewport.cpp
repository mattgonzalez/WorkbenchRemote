#include "base.h"
#include "Controller.h"
#include "Identifiers.h"
#include "ChannelViewport.h"
#include "InputChannelViewport.h"
#include "InputChannelComponent.h"

InputChannelViewport::InputChannelViewport(int const deviceIndex_, ValueTree channelsTree_ ) :
	ChannelViewport(deviceIndex_, channelsTree_)
{
	channelsTree.addListener(this);
}

void InputChannelViewport::buildChannelComponents()
{
	DBG("InputChannelViewport::buildChannelComponents()");

	content.channelComponents.clearQuick(true);

	int channelCount = channelsTree.getProperty(Identifiers::CurrentChannelCount, 0);

	for (int i = 0; i < channelCount; ++i)
	{
		ValueTree channelTree(channelsTree.getChild(i));
		String name(channelTree[Identifiers::Name]);

		InputChannelComponent *icc = new InputChannelComponent(deviceIndex, i);
		icc->channelNameLabel.setText(name,dontSendNotification);

		content.channelComponents.add(icc);
		content.addAndMakeVisible(icc);
	}

	content.resized();
}

void InputChannelViewport::timerCallback()
{
	for (int i = 0; i < content.channelComponents.size(); ++i)
	{
		content.channelComponents[i]->timerCallback();
	}
}
