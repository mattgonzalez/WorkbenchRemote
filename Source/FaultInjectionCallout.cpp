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

FaultInjectionCallout::FaultInjectionCallout(ValueTree tree_, CriticalSection& lock_, WorkbenchClient* client_):
	tree(tree_),
	lock(lock_),
	client(client_)
{
	addAndMakeVisible(&faultInjectionLabel);
	faultInjectionLabel.setText("Fault Injection", dontSendNotification);
	faultInjectionLabel.setColour(Label::textColourId, Colours::black);
	faultInjectionLabel.setFont(Font(18.0f));
	faultInjectionLabel.setJustificationType(Justification::centred);

	addAndMakeVisible(&resetButton);
	resetButton.setButtonText("Reset");
	resetButton.addListener(this);

	addAndMakeVisible(&helpTextDisplay.label);
	helpTextDisplay.label.setColour(Label::outlineColourId, Colours::lightgrey);
	helpTextDisplay.label.setJustificationType(Justification::centredLeft);

	for (int i = 0; i < numElementsInArray(FaultInjection::types); i++)
	{
		Group * group = new Group();
		group->toggleButton = new ToggleButton(FaultInjection::types[i].text);
		addAndMakeVisible(group->toggleButton);
		group->toggleButton->getProperties().set(Identifiers::HelpText, 
			FaultInjection::types[i].toggleHelpText);

		group->percentSlider = new Slider(Slider::LinearBar, Slider::TextBoxRight);
		configPercentSlider(*group->percentSlider);
		addAndMakeVisible(group->toggleButton);
		group->toggleButton->addListener(this);
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
		

		ValueTree leaf(tree.getChildWithName(*FaultInjection::types[i].id));
		bool enabled = leaf[Identifiers::Enabled];
		group->toggleButton->setToggleState(enabled,dontSendNotification);
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
	tree.addListener(this);
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

	faultInjectionLabel.setBounds(0, 5, getWidth(), 25);

	juce::Rectangle<int> rect(gap,50,buttonWidth,25);
	for (int i = 0; i < groups.size(); i++)
	{
		Group * group = groups[i];
		group->toggleButton->setBounds(rect);
		group->percentSlider->setBounds(group->toggleButton->getRight() + sliderGap, group->toggleButton->getY(), 100, 25);
		if (group->amountSlider != nullptr)
		{
			group->amountSlider->setBounds(group->percentSlider->getBounds().translated(120, 0));
		}
		rect.translate(0,rect.getHeight() + gap);

	}

	int x = faultInjectionLabel.getX() + 20;
	int labelH = 45;
	helpTextDisplay.label.setBounds(x, getHeight() - labelH - buttonH - 10, getWidth() - 40, labelH);
	resetButton.setBounds((getWidth() - buttonWidth)/2, helpTextDisplay.label.getBottom() + 5, buttonWidth, buttonH);
}

void FaultInjectionCallout::sliderValueChanged( Slider* slider )
{
	for (int i = 0; i < groups.size(); i++)
	{
		if (slider == groups[i]->percentSlider)
		{
			ValueTree leaf(tree.getChildWithName(*FaultInjection::types[i].id));
			if (leaf.isValid())
			{
				leaf.setProperty(Identifiers::Percent, slider->getValue(), nullptr);
			}
			return;
		}
		if (slider == groups[i]->amountSlider && groups[i]->amountSlider != nullptr)
		{
			ValueTree leaf(tree.getChildWithName(*FaultInjection::types[i].id));
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

	if (&resetButton == button)
	{
		resetSliders();
		return;
	}

	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i]->toggleButton == button)
		{
			bool enabled = button->getToggleState();

			ValueTree leaf(tree.getChildWithName(*FaultInjection::types[i].id));
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
		group->toggleButton->setToggleState(false, sendNotification);
	}

}

void FaultInjectionCallout::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
#if 0
	if (property == Identifiers::Enabled)
	{
		groups[0]->toggleButton->setToggleState(treeWhosePropertyHasChanged.getProperty(property), dontSendNotification);
	}
#endif
}

void FaultInjectionCallout::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void FaultInjectionCallout::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void FaultInjectionCallout::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void FaultInjectionCallout::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
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