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

#if WORKBENCH
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
#endif

SettingsComponent::SettingsComponent(ValueTree tree_, WorkbenchClient * client_):
	tree(tree_),
	client(client_)
{
	const String msec(" msec");

	//
	// PTP settings
	//
	{
		BooleanPropertyComponent *toggle;
		ChoicePropertyComponent *choice;
		StringArray choices;
		Array<var> temp;
		Array<PropertyComponent *> time_controls;
	
		choices.add("PTP Follower");
		choices.add("PTP Grandmaster");
		choices.add("Use best master clock algorithm (BMCA)");
		temp.add(CONFIG_FOLLOWER);
		temp.add(CONFIG_GRANDMASTER);
		temp.add(CONFIG_BMCA);

		choice = new ChoicePropertyComponent(tree.getPropertyAsValue(Identifiers::StaticPTPRole, nullptr),
			"PTP role",
			choices,
			temp);
		choice->refresh();
		choice->getProperties().set(Identifiers::HelpText,
			"PTP Grandmaster: Workbench will send PTP Sync and Follow_Up messages\n\n"
			"PTP Follower: Workbench will wait to receive PTP Sync and Follow_Up messages\n\n"
			"BMCA mode: Workbench will automatically select a master clock using the 802.1AS best master clock algorithm");
		time_controls.add(choice);
	
		toggle = new BooleanPropertyComponent(tree.getPropertyAsValue(Identifiers::PTPSendFollowupTLV, nullptr),
			"Follow_Up TLV",
			"Send Follow_Up messages with TLV");
		toggle->getProperties().set(Identifiers::HelpText,
			"\nPTP Follow_Up messages may be sent with or without the TLV data at the end.\n\n"
			"TLV enabled: Follow_Up messages will be 90 bytes with messageLength set to 72.\n\n"
			"TLV disabled: Follow_Up messages will be 60 bytes with messageLength set to 44.");
		time_controls.add(toggle);
	
		announceBooleanPropertyComponent = new BooleanPropertyComponent(tree.getPropertyAsValue(Identifiers::PTPSendAnnounce, nullptr),
			"Announce",
			"Send Announce messages");
		announceBooleanPropertyComponent->getProperties().set(Identifiers::HelpText,
			"\nWorkbench can optionally send PTP Announce messages.");
		announceBooleanPropertyComponent->setEnabled((int)tree.getProperty(Identifiers::StaticPTPRole) != CONFIG_BMCA);
		time_controls.add(announceBooleanPropertyComponent);

		toggle = new BooleanPropertyComponent(tree.getPropertyAsValue(Identifiers::PTPSendSignalingFlag, nullptr),
			"Send Signaling",
			"Send Signaling message once PTP is locked");
		toggle->getProperties().set(Identifiers::HelpText,
			"\nIf Workbench is in PTP Follower mode, Workbench can send a Signaling message to the grandmaster once PTP is locked.");
		time_controls.add(toggle);

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
		choice = new ChoicePropertyComponent(tree.getPropertyAsValue(Identifiers::PTPDelayRequestIntervalMsec, nullptr),
			"Send PDelay_Req",
			choices,
			temp);
		choice->refresh();
		choice->getProperties().set(Identifiers::HelpText,
			"\nSet how often Workbench will generate PDelay_Req messages.");
		time_controls.add(choice);
	
		panel.addSection("PTP", time_controls);
	}

	{
		//
		// AVTP
		//
		SliderWithUnitsPropertyComponent *slider;
		Array<PropertyComponent *> avtp_controls;

		slider = new SliderWithUnitsPropertyComponent(tree.getPropertyAsValue(Identifiers::TalkerTimestampOffsetMsec, nullptr),
			"Talker transit time",
			0, 
			20, 
			0.1,
			1.0,
			msec);	
		slider->getProperties().set(Identifiers::HelpText,
			"\nStream packets are sent before the embedded presentation time.  This sets the offset between the launch time and the presentation time.\n\n"
			"This setting may only be changed if all the streams are stopped.");
		avtp_controls.add(slider);
		talkerTimestampOffsetPropertyComponent = slider;

		slider = new SliderWithUnitsPropertyComponent(tree.getPropertyAsValue(Identifiers::ListenerTimestampOffsetMsec, nullptr),
			"Listener timestamp adjust",
			-20.0, 
			20.0, 
			0.01,
			1.0,
			msec);	
		slider->getProperties().set(Identifiers::HelpText,
			"\nWorkbench adds this amount to incoming stream packet AVTP timestamps.");
		avtp_controls.add(slider);
		
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
		slider->getProperties().set(Identifiers::HelpText,
			"\nDifferences between AVTP timestamps beyond this tolerance will be logged as errors.");
		faultLoggingControls.add(slider);

		panel.addSection("AVTP fault logging", faultLoggingControls);
	}

	{
#if ANALYZERBR_USB

		ChoicePropertyComponent *choice;
		StringArray choices;
		Array<var> temp;
		Array<PropertyComponent *> analyzerControls;

		choices.add("Standard Ethernet");
		choices.add("BroadR-Reach master");
		choices.add("BroadR-Reach slave");
		temp.add(ANALYZERBR_USB_ETHERNET_MODE_STANDARD);
		temp.add(ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER);
		temp.add(ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE);

		USBDevice *device = controller->usb.getDevice();
		bool enabled = device != nullptr;

		ethernetModeValue = tree.getPropertyAsValue(Identifiers::EthernetMode, nullptr);
		choice = new ChoicePropertyComponent(ethernetModeValue,
			"Ethernet mode",
			choices,
			temp);
		choice->refresh();
		choice->getProperties().set(Identifiers::HelpText,
			"Set Analyzer BR Ethernet mode.");
		choice->setEnabled(enabled);
		analyzerControls.add(choice);

		spdifLockComp = new LabelPropertyComponent("S/PDIF input lock");
		spdifLockComp->setEnabled(enabled);
		analyzerControls.add(spdifLockComp);

		panel.addSection("Analyzer BR", analyzerControls);
		
		if (enabled)
		{
			startTimer(1000);
		}
#endif
	}

	//addAndMakeVisible(&tabs);
	//tabs.setCurrentTabIndex(1, false);
	addAndMakeVisible(&panel);

	lf.setColour(Slider::thumbColourId, /*JUCE_LIVE_CONSTANT*/(Colour(0xc01a53ff)));
	lf.setColour(TextEditor::backgroundColourId, Colour(0xfff2f2f2));
	lf.setColour(TextEditor::highlightColourId, Colour(0x55CC0000));
	lf.setColour(TextEditor::focusedOutlineColourId, findColour (TextButton::buttonColourId).withAlpha(0.5f));
	lf.setColour(TextEditor::focusedOutlineColourId, Colour(0xffE80000));
	setLookAndFeel(&lf);

	addAndMakeVisible(&helpTextDisplay.label);
	helpTextDisplay.label.setColour(Label::outlineColourId, Colours::lightgrey);
	helpTextDisplay.label.setJustificationType(Justification::centredLeft);
}

SettingsComponent::~SettingsComponent()
{
	setLookAndFeel(nullptr);
}

void SettingsComponent::resized()
{
	panel.setBounds(proportionOfWidth(0.05f), proportionOfHeight(0.05f), 350, proportionOfHeight(0.8f));
	helpTextDisplay.label.setBounds(proportionOfWidth(0.05f), panel.getBottom() - 75, 350, 135);
}

void SettingsComponent::changeListenerCallback( ChangeBroadcaster* source )
{
}

void SettingsComponent::paint( Graphics& g )
{
	g.fillAll(Colours::white);
}

void SettingsComponent::visibilityChanged()
{
	if (isVisible())
	{
		Desktop::getInstance().addGlobalMouseListener(&helpTextDisplay);
	}
	else
	{
		Desktop::getInstance().removeGlobalMouseListener(&helpTextDisplay);
	}
}

SettingsComponent::HelpTextDisplay::HelpTextDisplay() :
	currentHelpComponent(nullptr)
{

}

void SettingsComponent::HelpTextDisplay::mouseMove( const MouseEvent& event )
{
	Component *c = event.eventComponent;

	while (c != nullptr)
	{
		NamedValueSet& properties(c->getProperties());

		if (properties.contains(Identifiers::HelpText))
		{
			break;
		}

		c = c->getParentComponent();
	}

	if (nullptr == c)
	{
		currentHelpComponent = c;
		label.setText(String::empty, dontSendNotification);
		return;
	}

	if (c != currentHelpComponent)
	{
		NamedValueSet& properties(c->getProperties());
		Font f(14);
		label.setText(properties[Identifiers::HelpText], dontSendNotification);
		label.setFont(f);
		label.setJustificationType(Justification::topLeft);
		currentHelpComponent = c;
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
		const Colour shadowColour (box.findColour (TextEditor::shadowColourId).withMultipliedAlpha (0.75f));
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
