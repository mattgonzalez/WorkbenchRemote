/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#pragma once

class WorkbenchClient;

class SliderWithUnitsPropertyComponent : public SliderPropertyComponent
{
public:
	SliderWithUnitsPropertyComponent(const Value& valueToControl,
		const String& propertyName,
		double rangeMin,
		double rangeMax,
		double interval,
		double skewFactor = 1.0,
		String units = String::empty);
};

class LabelPropertyComponent;
class SettingsComponent : public Component, public Value::Listener
{
public:
	SettingsComponent(ValueTree tree_, WorkbenchClient * client_);
	~SettingsComponent();
	void resized();
	
	enum 
	{
		CONFIG_FOLLOWER = 100,
		CONFIG_GRANDMASTER,
		CONFIG_BMCA,
		CONFIG_DELAY_REQUESTS_DISABLED = -1,
		MIN_DELAY_REQUEST_INTERVAL_MILLISECONDS = 125,
		MAX_DELAY_REQUEST_INTERVAL_MILLISECONDS = 4000,

		ANALYZERBR_USB_ETHERNET_MODE_STANDARD = 1,
		ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER,
		ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE

	}; 
	
	
protected:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent);

	virtual void paint( Graphics& g );

	void initialize();

	virtual void valueChanged( Value& value );

	Value broadRReachSupportedValue;
	Value SpdifLockedValue;

	LabelPropertyComponent * spdifLockComp;

	SliderWithUnitsPropertyComponent *talkerTimestampOffsetPropertyComponent;
	SliderWithUnitsPropertyComponent *listenerTimestampOffsetPropertyComponent;
	SliderWithUnitsPropertyComponent *timestampTolerancePercentPropertyComponent;
	
	ChoicePropertyComponent *staticPTPRoleChoicePropertyComponent;
	ChoicePropertyComponent *delayRequestIntervalMsecPropertyComponent;
	ChoicePropertyComponent *analyzerBREthernetMode;

	BooleanPropertyComponent *followupTLVPropertyComponent;
	BooleanPropertyComponent *announceBooleanPropertyComponent;
	BooleanPropertyComponent *sendSignalingFlagPropertyComponent;
	PropertyPanel panel;
	ValueTree tree;
	WorkbenchClient* client;
#if 0

	class LocalLookAndFeel : public LookAndFeel_V3
	{
	public:

		//toggle
		Image normUnchecked;
		Image normChecked;
		Image overUnchecked;
		Image overChecked;
		Image downUnchecked;
		Image downChecked;
		Image disUnchecked;
		Image disChecked;
		//combo box
		Image comboBoxUp;
		Image comboBoxDown;
		Image comboBoxOver;
		Image comboBoxDis;
		void drawPropertyPanelSectionHeader(Graphics&, const String& name, bool isOpen, int width, int height);
		void drawToggleButton(Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown);
		void positionComboBoxText(ComboBox& box, Label& label);
		void drawComboBox(Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box);
	}lf;
#endif
};
