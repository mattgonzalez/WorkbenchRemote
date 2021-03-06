#pragma once
#include "GearButton.h"

class DeviceHeaderComponent: public Component, public ComboBox::Listener, public ValueTree::Listener, public Button::Listener
{
public:
	DeviceHeaderComponent(ValueTree tree_, CriticalSection& lock_);
	~DeviceHeaderComponent();	
	
	void resized();
	virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );
	virtual void buttonClicked(Button *button);

protected:
	int deviceIndex;
	ValueTree audioDevicesTree;
	ValueTree deviceTree;
	CriticalSection& lock;

	ComboBox deviceCombo;
	GearButton gearButton;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceHeaderComponent)

	virtual void paint( Graphics& g );

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
};