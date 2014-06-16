#pragma once
#include "GearButton.h"

class Controller;

class DeviceHeaderComponent: public Component, public ComboBox::Listener, public Value::Listener, public Button::Listener
{
public:
	DeviceHeaderComponent(int const deviceIndex_);
	~DeviceHeaderComponent();	
	
	void resized();
	virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );
	virtual void buttonClicked(Button *button);

protected:
	int const deviceIndex;
	ValueTree deviceTree;

	ComboBox deviceCombo;
	GearButton gearButton;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceHeaderComponent)

	virtual void valueChanged( Value& value );

	virtual void paint( Graphics& g );

	Value deviceNameValue;
};