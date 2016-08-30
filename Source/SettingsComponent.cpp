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

	lf.setColour(Slider::thumbColourId, /*JUCE_LIVE_CONSTANT*/(Colour(0xc01a53ff)));
	lf.setColour(TextEditor::backgroundColourId, Colour(0xfff2f2f2));
	lf.setColour(TextEditor::highlightColourId, Colour(0x55CC0000));
	lf.setColour(TextEditor::focusedOutlineColourId, findColour (TextButton::buttonColourId).withAlpha(0.5f));
	lf.setColour(TextEditor::focusedOutlineColourId, Colour(0xffE80000));
	setLookAndFeel(&lf);

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

void SettingsComponent::LocalLookAndFeel::drawPropertyPanelSectionHeader( Graphics&g, const String& name, bool isOpen, int width, int height )
{
	normUnchecked = ImageCache::getFromMemory(BinaryData::normalunchecked_png, BinaryData::normalunchecked_pngSize);
	normChecked = ImageCache::getFromMemory(BinaryData::normalchecked_png, BinaryData::normalchecked_pngSize);
	overUnchecked = ImageCache::getFromMemory(BinaryData::overunchecked_png, BinaryData::overunchecked_pngSize);
	overChecked = ImageCache::getFromMemory(BinaryData::overchecked_png, BinaryData::overchecked_pngSize);
	downUnchecked = ImageCache::getFromMemory(BinaryData::downunchecked_png, BinaryData::downunchecked_pngSize);
	downChecked = ImageCache::getFromMemory(BinaryData::downchecked_png, BinaryData::downchecked_pngSize);
	disUnchecked = ImageCache::getFromMemory(BinaryData::disunchecked_png, BinaryData::disunchecked_pngSize);
	disChecked = ImageCache::getFromMemory(BinaryData::dischecked_png, BinaryData::dischecked_pngSize);
	comboBoxUp = ImageCache::getFromMemory(BinaryData::combobox_editableupskin_png, BinaryData::combobox_editableupskin_pngSize);
	comboBoxDown = ImageCache::getFromMemory(BinaryData::combobox_editabledownskin_png, BinaryData::combobox_editabledownskin_pngSize);
	comboBoxOver = ImageCache::getFromMemory(BinaryData::combobox_editableoverskin_png, BinaryData::combobox_editableoverskin_pngSize);
	comboBoxDis = ImageCache::getFromMemory(BinaryData::combobox_editabledisabledskin_png, BinaryData::combobox_editabledisabledskin_pngSize);

	Colour c(Colours::lightgrey);
	g.setColour(c);
	g.fillRect(0,0,width,height);
	LookAndFeel_V2::drawPropertyPanelSectionHeader(g,name,isOpen,width,height);
}

void SettingsComponent::LocalLookAndFeel::drawToggleButton( Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown )
{
	int destWidth, destHeight;

	destHeight = button.getHeight();
	if (button.getButtonText().length() > 0)
	{
		destWidth = destHeight;
	}
	else
	{
		destWidth = button.getWidth();
	}
	int y;

	Image img;
	if (button.getToggleState())
	{
		if (button.isEnabled())
		{
			img = normChecked; 

			if (isMouseOverButton == true)
			{
				img = overChecked;
			}
			if (isButtonDown == true)
			{
				img = downChecked;
			}
		}
		else
		{
			img = disChecked;
		}
	}
	else
	{
		if (button.isEnabled())
		{
			img = normUnchecked; 

			if (isMouseOverButton == true)
			{
				img = overUnchecked;
			}
			if (isButtonDown == true)
			{
				img = downUnchecked;
			}
		}
		else
		{
			img = disUnchecked;
		}
	}
	y = (button.getHeight() - img.getHeight()) >> 1;
	g.drawImageAt(img,2,y);

	//
	//	Button text
	//
	if (button.getButtonText().length() > 0) 
	{
		g.setFont (14.0f);
		Colour c(Colours::black);
		if (false == button.isEnabled())
			c = c.withAlpha(0.5f);
		g.setColour (c);

		int textWid = button.getButtonText().length();

		g.drawText (button.getButtonText(), destWidth + 5, 0, button.getWidth() - textWid, button.getHeight(), Justification::centredLeft, false);
	}
}

void SettingsComponent::LocalLookAndFeel::drawComboBox( Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box )
{
	int w = 20;
	int x = width - w;
	int border = 1;

	g.fillAll (box.findColour (ComboBox::backgroundColourId));
	Image image(comboBoxUp);

	g.setColour (box.findColour (ComboBox::outlineColourId));
	g.setOpacity (1.0f);
	const Colour shadowColour (box.findColour (TextEditor::shadowColourId).withMultipliedAlpha (0.75f));
	drawBevel (g, 0, 0, width - w, height + 2, border + 2, shadowColour, shadowColour);

	if (box.isMouseOver(true))
	{
		image = comboBoxOver;
	}

	if (isButtonDown)
	{
		image = comboBoxDown;
		g.setColour (box.findColour (TextEditor::focusedOutlineColourId));
		g.drawRect (0, 0, width - w, height, border);

		g.setOpacity (1.0f);
		drawBevel (g, 0, 0, width - w, height + 2, border + 2, shadowColour, shadowColour);
	}

	g.drawImage(image, x, 0, w, height, 0,0,image.getWidth(), image.getHeight(), false);
	g.drawRect (0, 0, width - w, height);
}

void SettingsComponent::LocalLookAndFeel::positionComboBoxText( ComboBox& box, Label& label )
{
	label.setBounds (0, 0, box.getWidth()- 20, box.getHeight());

	label.setFont (getComboBoxFont (box));
}

SliderWithUnitsPropertyComponent::SliderWithUnitsPropertyComponent( const Value& valueToControl, const String& propertyName, double rangeMin, double rangeMax, double interval, double skewFactor /*= 1.0*/, String units /*= String::empty*/ ) :
	SliderPropertyComponent(valueToControl,propertyName, rangeMin, rangeMax, interval, skewFactor)
{
	slider.setTextValueSuffix(units);
	slider.setSliderStyle(Slider::LinearHorizontal);
}
