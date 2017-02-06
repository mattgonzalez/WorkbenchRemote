/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/

#pragma once

class Settings;
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
	SettingsComponent(Settings* settings_, WorkbenchClient * client_);
	~SettingsComponent();
	void resized();
	
protected:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent);

	virtual void paint( Graphics& g );

	void initialize();

	virtual void valueChanged( Value& value );

	Value broadRReachSupportedValue;
	Value SpdifLockedValue;

	LabelPropertyComponent * spdifLockComp;

	ChoicePropertyComponent* streamClassPropertyComponent;
	ChoicePropertyComponent* draft1722aPropertyComponent;
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
	
	Settings* settings;
	WorkbenchClient* client;
};
