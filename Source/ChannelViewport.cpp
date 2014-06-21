#include "base.h"
#include "Identifiers.h"
#include "ChannelViewport.h"
#include "ChannelComponent.h"
#include "Settings.h"

ChannelViewport::ChannelViewport(int const deviceIndex_, ValueTree channelsTree_, CriticalSection &lock_) :
	deviceIndex(deviceIndex_),
	channelsTree(channelsTree_),
	lock(lock_)
{
	setViewedComponent(&content);

	triggerAsyncUpdate();
}

void ChannelViewport::resized()
{
	int w = getWidth(); 
	int h = content.getTotalChildHeight() + 5;
	if (h > getHeight())
		w -= getScrollBarThickness();
	content.setSize(w, h);
	Viewport::resized();
}

void ChannelViewport::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	//DBG("ChannelViewport::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString() << " " << treeWhosePropertyHasChanged[property].toString());

	if(treeWhosePropertyHasChanged.isAChildOf(channelsTree))
	{
		ChannelComponent* channelComponent = content.channelComponents[treeWhosePropertyHasChanged[Identifiers::Channel]];
		if (channelComponent == nullptr)
		{
			return;
		}
		if (Identifiers::Name == property)
		{
			channelComponent->channelNameLabel.setText(treeWhosePropertyHasChanged[property].toString(),dontSendNotification);
			return;
		}
	}
	

#if 0
	if (Identifiers::MaxChannelCount == property)
	{
		triggerAsyncUpdate();
		return;
	}

	if (Identifiers::CurrentChannelCount == property)
	{
		setChannelComponentsVisible();
		return;
	}
#endif
}

void ChannelViewport::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
	triggerAsyncUpdate();
}

void ChannelViewport::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
	triggerAsyncUpdate();
}

void ChannelViewport::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
	//triggerAsyncUpdate();
}

void ChannelViewport::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
	//triggerAsyncUpdate();
}

void ChannelViewport::handleAsyncUpdate()
{
	buildChannelComponents();
	setChannelComponentsVisible();
	resized();
}

void ChannelViewport::paintOverChildren( Graphics& g )
{
	juce::Rectangle<int> shadow(getLocalBounds());

	shadow.setHeight(jmin(10, getViewPositionY() - 2));  // clamps shadow pos

	if (shadow.getHeight() > 0)
	{
		shadow.setWidth(getMaximumVisibleWidth());

		ColourGradient grad(Colours::black.withAlpha(0.6f), (float)shadow.getX(), (float)shadow.getY(),
			Colours::transparentBlack, (float)shadow.getX(), (float)shadow.getBottom(), false);
		g.setGradientFill(grad);
		g.fillRect(shadow);
	}
}

void ChannelViewport::setChannelComponentsVisible()
{
	for (int i = 0; i < content.channelComponents.size(); ++i)
	{
		ChannelComponent* cc = content.channelComponents[i];
		cc->setVisible((int)channelsTree[Identifiers::CurrentChannelCount] > i);
	}
}

void ChannelViewport::Content::paint( Graphics& g )
{
	//g.fillAll(Colours::purple);
}

void ChannelViewport::Content::resized()
{
	juce::Rectangle<int> r(0,0,getWidth(), 50);
	for (int i = 0; i < channelComponents.size(); ++i)
	{
		ChannelComponent* cc = channelComponents[i];
		cc->setBounds(r);
		r.translate(0,r.getHeight() + 2);

		if (0 == ((i + 1) % 8))
		{
			r.translate(0,5);
		}
	}
}

int ChannelViewport::Content::getTotalChildHeight()
{
	return channelComponents.size() * 52 + (channelComponents.size() / 8) * 5;
}
