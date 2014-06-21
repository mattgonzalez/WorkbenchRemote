#include "base.h"
#include "DeviceComponent.h"
#include "Identifiers.h"
#include "BinaryData.h"

DeviceComponent::DeviceComponent(ValueTree tree_, CriticalSection &lock_) :
	deviceTree(tree_),
	lock(lock_),
	header(tree_, lock_),
 	inputChannelViewport(0, deviceTree.getChildWithName(Identifiers::Input), lock_),
 	outputChannelViewport(0, deviceTree.getChildWithName(Identifiers::Output), lock_)
{
	addAndMakeVisible(&header);
	addAndMakeVisible(&inputChannelViewport);
	addAndMakeVisible(&outputChannelViewport);
	deviceTree.addListener(this);
}

DeviceComponent::~DeviceComponent()
{

}

void DeviceComponent::paint( Graphics& g)
{
	g.fillAll(Colour(0xffcfe4ff));
}

void DeviceComponent::resized()
{
	Image gear(ImageCache::getFromMemory(BinaryData::gearbutton_png, BinaryData::gearbutton_pngSize));
	int h = gear.getHeight() + 10;
	header.setBounds(0,0,getWidth(),h);

	int margin = 2;
	int y = header.getBottom() + margin;
	int w = proportionOfWidth(0.3f);
	juce::Rectangle<int> r(margin, y, w, getHeight() - y);
	inputChannelViewport.setBounds(r);
	r.translate(r.getWidth() + margin * 2, 0);
	r.setWidth(getWidth() - r.getX() - margin);
	outputChannelViewport.setBounds(r);
}

void DeviceComponent::buttonClicked( Button* button )
{

}

void DeviceComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	//DBG("DeviceComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());
}

void DeviceComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void DeviceComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void DeviceComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void DeviceComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

void DeviceComponent::timerCallback()
{
	inputChannelViewport.timerCallback();
	outputChannelViewport.timerCallback();
}
