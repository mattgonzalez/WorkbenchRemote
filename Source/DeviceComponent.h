#pragma once

#include "DeviceHeaderComponent.h"
#include "InputChannelComponent.h"
#include "OutputChannelComponent.h"
#include "InputChannelViewport.h"
#include "OutputChannelViewport.h"

class Controller;

class DeviceComponent : public Component, public Button::Listener, public ValueTree::Listener
{
public:
	DeviceComponent(int const deviceIndex_);
	~DeviceComponent();

	void paint (Graphics&);
	void resized();

	void timerCallback();

private:
	int const deviceIndex;
	ValueTree deviceTree;

	DeviceHeaderComponent header;
	InputChannelViewport inputChannelViewport;
 	OutputChannelViewport outputChannelViewport;

	virtual void buttonClicked( Button* button);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceComponent)

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );

	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );

	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );

	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );

	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

};
