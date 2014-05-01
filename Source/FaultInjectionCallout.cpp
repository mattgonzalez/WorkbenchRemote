#if 0

/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

//
// Not yet implemented
//
#include "base.h"
#include "FaultInjectionCallout.h"
#include "FaultInjection.h"
#include "Identifiers.h"

void FaultInjectionCallout::configPercentSlider(Slider &slider)
{
	slider.setTextValueSuffix("%");

	slider.setRange(0.0,10.0, 0.1);
	addAndMakeVisible(slider);
	slider.addListener(this);
}

FaultInjectionCallout::FaultInjectionCallout(  Controller * controller_, ReferenceCountedObjectPtr<Stream> stream_ ) :
	stream(stream_),
	controller(controller_)
{
	addAndMakeVisible(&faultInjectionLab);
	faultInjectionLab.setText("Fault Injection", dontSendNotification);
	faultInjectionLab.setColour(Label::textColourId, Colours::black);
	faultInjectionLab.setFont(Font(18.0f));
	faultInjectionLab.setJustificationType(Justification::centred);

	addAndMakeVisible(&resetButton);
	resetButton.setButtonText("Reset");
	resetButton.addListener(this);

	addAndMakeVisible(&helpTextDisplay.label);
	helpTextDisplay.label.setColour(Label::outlineColourId, Colours::lightgrey);
	helpTextDisplay.label.setJustificationType(Justification::centredLeft);

	ValueTree faultInjectionTree(getFaultInjectionTree());
	for (int i = 0; i < numElementsInArray(FaultInjection::types); i++)
	{
		Group * group = new Group();
		group->toggleBtn = new ToggleButton(FaultInjection::types[i].text);
		addAndMakeVisible(group->toggleBtn);
		group->toggleBtn->getProperties().set(Identifiers::HelpText, 
			FaultInjection::types[i].toggleHelpText);

		group->percentSlider = new Slider(Slider::LinearBar, Slider::TextBoxRight);
		configPercentSlider(*group->percentSlider);
		addAndMakeVisible(group->toggleBtn);
		group->toggleBtn->addListener(this);
		group->percentSlider->getProperties().set(Identifiers::HelpText, 
			FaultInjection::types[i].percentHelpText);

		if (FaultInjection::types[i].units != nullptr)
		{
			group->amountSlider = new Slider(Slider::LinearBar, Slider::TextBoxRight);
			group->amountSlider->setTextValueSuffix(FaultInjection::types[i].units);

			group->amountSlider->setRange(0.0,FaultInjection::types[i].max, FaultInjection::types[i].step);
			addAndMakeVisible(group->amountSlider);
			group->amountSlider->addListener(this);

			group->amountSlider->getProperties().set(Identifiers::HelpText, 
				FaultInjection::types[i].amountHelpText);
		}
		

		ValueTree leaf(faultInjectionTree.getChildWithName(*FaultInjection::types[i].id));
		bool enabled = leaf[Identifiers::Enabled];
		group->toggleBtn->setToggleState(enabled,dontSendNotification);
		group->percentSlider->setValue(leaf[Identifiers::Percent],dontSendNotification);
		group->percentSlider->setEnabled(enabled);

		if (group->amountSlider != nullptr)
		{
			group->amountSlider->setValue(leaf[Identifiers::Amount],dontSendNotification);
			group->amountSlider->setEnabled(enabled);
		}

		groups.add(group);
	}
	
	addMouseListener(&helpTextDisplay, true);
	setSize(500,320);
}

FaultInjectionCallout::~FaultInjectionCallout()
{
}

void FaultInjectionCallout::resized()
{
	int gap = 15;
	int sliderGap = 50;
	int buttonWidth = 130;
	int buttonH = 25;

	faultInjectionLab.setBounds(0, 5, getWidth(), 25);

	juce::Rectangle<int> rect(gap,50,buttonWidth,25);
	for (int i = 0; i < groups.size(); i++)
	{
		Group * group = groups[i];
		group->toggleBtn->setBounds(rect);
		group->percentSlider->setBounds(group->toggleBtn->getRight() + sliderGap, group->toggleBtn->getY(), 100, 25);
		if (group->amountSlider != nullptr)
		{
			group->amountSlider->setBounds(group->percentSlider->getBounds().translated(120, 0));
		}
		rect.translate(0,rect.getHeight() + gap);

	}

	int x = faultInjectionLab.getX() + 20;
	int labelH = 45;
	helpTextDisplay.label.setBounds(x, getHeight() - labelH - buttonH - 10, getWidth() - 40, labelH);
	resetButton.setBounds((getWidth() - buttonWidth)/2, helpTextDisplay.label.getBottom() + 5, buttonWidth, buttonH);
}

void FaultInjectionCallout::sliderValueChanged( Slider* slider )
{
	ValueTree faultInjectionTree(getFaultInjectionTree());
	for (int i = 0; i < groups.size(); i++)
	{
		if (slider == groups[i]->percentSlider)
		{
			ValueTree leaf(faultInjectionTree.getChildWithName(*FaultInjection::types[i].id));
			if (leaf.isValid())
			{
				leaf.setProperty(Identifiers::Percent, slider->getValue(), nullptr);
			}
			return;
		}
		if (slider == groups[i]->amountSlider && groups[i]->amountSlider != nullptr)
		{
			ValueTree leaf(faultInjectionTree.getChildWithName(*FaultInjection::types[i].id));
			if (leaf.isValid())
			{
				leaf.setProperty(Identifiers::Amount, slider->getValue(), nullptr);
			}
			return;
		}
	}	
}

void FaultInjectionCallout::buttonClicked( Button* button)
{
	ValueTree faultInjectionTree(getFaultInjectionTree());

	if (&resetButton == button)
	{
		resetSliders();
		return;
	}

	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i]->toggleBtn == button)
		{
			bool enabled = button->getToggleState();

			ValueTree leaf(faultInjectionTree.getChildWithName(*FaultInjection::types[i].id));
			if (leaf.isValid())
			{
				leaf.setProperty(Identifiers::Enabled, enabled, nullptr);
			}

			groups[i]->percentSlider->setEnabled(enabled);
			if (groups[i]->amountSlider != nullptr)
			{
				groups[i]->amountSlider->setEnabled(enabled);
			}
			return;
		}
	}
}

ValueTree FaultInjectionCallout::getFaultInjectionTree()
{
	ValueTree renderStreamsTree(controller->getTree(Identifiers::RenderStreams));
	ValueTree streamTree(renderStreamsTree.getChild(stream->listenerUniqueID));
	return streamTree.getChildWithName(Identifiers::FaultInjection);
}

void FaultInjectionCallout::resetSliders()
{
	for (int i = 0; i < groups.size(); i++)
	{
		Group * group = groups[i];
		group->percentSlider->setValue(0.0, sendNotification);
		if (group->amountSlider != nullptr)
		{
			group->amountSlider->setValue(group->amountSlider->getMinimum(),sendNotification);
		}
		group->toggleBtn->setToggleState(false, sendNotification);
	}

}

FaultInjectionCallout::HelpTextDisplay::HelpTextDisplay() :
	currentHelpComponent(nullptr)
{

}

void FaultInjectionCallout::HelpTextDisplay::mouseMove( const MouseEvent& event )
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

		label.setText(properties[Identifiers::HelpText], dontSendNotification);
		currentHelpComponent = c;
	}
}


#endif