#include "base.h"
#include "Identifiers.h"
#include "ChannelViewport.h"
#include "OutputChannelViewport.h"
#include "OutputChannelComponent.h"

OutputChannelViewport::OutputChannelViewport(int const deviceIndex_, ValueTree channelsTree_, CriticalSection &lock_) :
	ChannelViewport(deviceIndex_, channelsTree_, lock_)
{
	channelsTree.addListener(this);
}

void OutputChannelViewport::buildChannelComponents()
{
	content.channelComponents.clearQuick(true);

	int channelCount = channelsTree.getProperty(Identifiers::CurrentChannelCount, 0);

	DBG("OutputChannelViewport::buildChannelComponents() channelCount:" << channelCount << " deviceIndex:" << deviceIndex);

	for (int i = 0; i < channelCount; ++i)
	{
		ValueTree channelTree(channelsTree.getChild(i));
		String name(channelTree[Identifiers::Name]);

		OutputChannelComponent *icc = new OutputChannelComponent(deviceIndex, channelTree, i, lock);
		icc->channelNameLabel.setText(name,dontSendNotification);

		content.channelComponents.add(icc);
		content.addAndMakeVisible(icc);
	}

	content.resized();
}

void OutputChannelViewport::timerCallback()
{
	for (int i = 0; i < content.channelComponents.size(); ++i)
	{
		content.channelComponents[i]->timerCallback();
	}
}
