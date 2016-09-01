/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#include "base.h"
#include "WorkbenchClient.h"
#include "SettingsComponent.h"
#include "BinaryData.h"
#include "Identifiers.h"

class LabelPropertyComponent : public PropertyComponent
{
public:
	LabelPropertyComponent(const String &propertyName) :
		PropertyComponent(propertyName)
	{
		addAndMakeVisible(&label);
		label.setColour(Label::outlineColourId, Colours::black);
	}

	virtual void refresh()
	{
	}

	virtual void paint( Graphics& g) override
	{
		PropertyComponent::paint(g);
	}

	Label label;
};

SettingsComponent::SettingsComponent(ValueTree tree_, WorkbenchClient * client_):
	tree(tree_),
	client(client_)
{
	const String msecString(" msec");

	//
	// PTP settings
	//
	{
		StringArray choices;
		Array<var> temp;
		Array<PropertyComponent *> time_controls;
	
		choices.add("PTP Follower");
		choices.add("PTP Grandmaster");
		choices.add("Use best master clock algorithm (BMCA)");
		temp.add(CONFIG_FOLLOWER);
		temp.add(CONFIG_GRANDMASTER);
		temp.add(CONFIG_BMCA);

		staticPTPRoleChoicePropertyComponent = new ChoicePropertyComponent(tree.getPropertyAsValue(Identifiers::StaticPTPRole, nullptr),
			"PTP role",
			choices,
			temp);
		time_controls.add(staticPTPRoleChoicePropertyComponent);
	
		followupTLVPropertyComponent = new BooleanPropertyComponent(tree.getPropertyAsValue(Identifiers::PTPSendFollowupTLV, nullptr),
			"Follow_Up TLV",
			"Send Follow_Up messages with TLV");
		time_controls.add(followupTLVPropertyComponent);
	
		announceBooleanPropertyComponent = new BooleanPropertyComponent(tree.getPropertyAsValue(Identifiers::PTPSendAnnounce, nullptr),
			"Announce",
			"Send Announce messages");
		time_controls.add(announceBooleanPropertyComponent);

		sendSignalingFlagPropertyComponent = new BooleanPropertyComponent(tree.getPropertyAsValue(Identifiers::PTPSendSignalingFlag, nullptr),
			"Send Signaling",
			"Send Signaling once PTP locked");
		time_controls.add(sendSignalingFlagPropertyComponent);

		choices.clearQuick();
		temp.clearQuick();
		choices.add("Disabled");
		temp.add(CONFIG_DELAY_REQUESTS_DISABLED);
		for (int msec = MIN_DELAY_REQUEST_INTERVAL_MILLISECONDS; 
			msec <= MAX_DELAY_REQUEST_INTERVAL_MILLISECONDS; 
			msec <<= 1)
		{
			choices.add("Every " + String(msec) + " milliseconds");
			temp.add(msec);
		}
		delayRequestIntervalMsecPropertyComponent = new ChoicePropertyComponent(tree.getPropertyAsValue(Identifiers::PTPDelayRequest, nullptr),
			"Send PDelay_Req",
			choices,
			temp);
		time_controls.add(delayRequestIntervalMsecPropertyComponent);
	
		panel.addSection("PTP", time_controls);
	}

	{
		//
		// AVTP
		//
		SliderWithUnitsPropertyComponent *slider;
		Array<PropertyComponent *> avtp_controls;

		slider = new SliderWithUnitsPropertyComponent(tree.getPropertyAsValue(Identifiers::TalkerPresentationOffsetMsec, nullptr),
			"Talker presentation offset",
			0, 
			20, 
			0.1,
			1.0,
			msecString);	
		avtp_controls.add(slider);
		talkerTimestampOffsetPropertyComponent = slider;

		slider = new SliderWithUnitsPropertyComponent(tree.getPropertyAsValue(Identifiers::ListenerPresentationOffsetMsec, nullptr),
			"Listener presentation offset",
			0, 
			20, 
			0.1,
			1.0,
			msecString);
		avtp_controls.add(slider);
		listenerTimestampOffsetPropertyComponent = slider;

		panel.addSection("AVTP", avtp_controls);
	}

	{
		SliderWithUnitsPropertyComponent *slider;
		Array<PropertyComponent *> faultLoggingControls;

		slider = new SliderWithUnitsPropertyComponent(tree.getPropertyAsValue(Identifiers::TimestampTolerancePercent, nullptr),
			"Timestamp tolerance",
			0, 
			10, 
			0.1,
			1.0,
			"%");
		faultLoggingControls.add(slider);
		timestampTolerancePercentPropertyComponent = slider;

		panel.addSection("AVTP fault logging", faultLoggingControls);
	}

	{
		StringArray choices;
		Array<var> temp;
		Array<PropertyComponent *> analyzerControls;

		choices.add("Standard Ethernet");
		choices.add("BroadR-Reach master");
		choices.add("BroadR-Reach slave");
		temp.add(ANALYZERBR_USB_ETHERNET_MODE_STANDARD);
		temp.add(ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER);
		temp.add(ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE);
		
		broadRReachSupportedValue.referTo(tree.getPropertyAsValue(Identifiers::BroadRReachSupported, nullptr));
		analyzerBREthernetMode = new ChoicePropertyComponent(tree.getPropertyAsValue(Identifiers::BroadRReachMode, nullptr),
			"Ethernet mode",
			choices,
			temp);
		analyzerBREthernetMode->setEnabled((bool)broadRReachSupportedValue.getValue());
		analyzerControls.add(analyzerBREthernetMode);
		SpdifLockedValue.referTo(tree.getPropertyAsValue(Identifiers::SpdifLocked, nullptr));

		spdifLockComp = new LabelPropertyComponent("S/PDIF input lock");
		spdifLockComp->setEnabled(false);
		spdifLockComp->label.setText(SpdifLockedValue.getValue(), dontSendNotification);
		analyzerControls.add(spdifLockComp);

		panel.addSection("Analyzer BR", analyzerControls);
	}

	addAndMakeVisible(&panel);

	LookAndFeel& lf(getLookAndFeel());
	lf.setColour(Slider::thumbColourId, /*JUCE_LIVE_CONSTANT*/(Colour(0xc01a53ff)));
	lf.setColour(TextEditor::backgroundColourId, Colour(0xfff2f2f2));
	lf.setColour(TextEditor::highlightColourId, Colour(0x55CC0000));
	lf.setColour(TextEditor::focusedOutlineColourId, findColour (TextButton::buttonColourId).withAlpha(0.5f));
	lf.setColour(TextEditor::focusedOutlineColourId, Colour(0xffE80000));

	initialize();
}

SettingsComponent::~SettingsComponent()
{
	setLookAndFeel(nullptr);
}

void SettingsComponent::resized()
{
	panel.setBounds(proportionOfWidth(0.02f), proportionOfHeight(0.05f), 365, proportionOfHeight(0.8f));
}

void SettingsComponent::paint( Graphics& g )
{
	g.fillAll(Colours::white);
}

void SettingsComponent::initialize()
{
	talkerTimestampOffsetPropertyComponent->setValue(0);
	listenerTimestampOffsetPropertyComponent->setValue(0);
	timestampTolerancePercentPropertyComponent->setValue(0);
	broadRReachSupportedValue.addListener(this);
	SpdifLockedValue.addListener(this);
}

void SettingsComponent::valueChanged( Value& value )
{
	if (value.refersToSameSourceAs(broadRReachSupportedValue))
	{
		analyzerBREthernetMode->setEnabled((bool)broadRReachSupportedValue.getValue());
	}

	if (value.refersToSameSourceAs(SpdifLockedValue))
	{
		spdifLockComp->label.setText(SpdifLockedValue.getValue(), dontSendNotification);
	}
}

SliderWithUnitsPropertyComponent::SliderWithUnitsPropertyComponent( const Value& valueToControl, const String& propertyName, double rangeMin, double rangeMax, double interval, double skewFactor /*= 1.0*/, String units /*= String::empty*/ ) :
	SliderPropertyComponent(valueToControl,propertyName, rangeMin, rangeMax, interval, skewFactor)
{
	slider.setTextValueSuffix(units);
	slider.setSliderStyle(Slider::LinearHorizontal);
}
