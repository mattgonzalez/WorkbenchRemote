#include "base.h"
#include "GrandmasterComponent.h"
#include "Strings.h"
#include "BinaryData.h"
#include "Identifiers.h"

GrandmasterComponent::GrandmasterComponent(ValueTree tree_) :
GroupComponent("PTP Grandmaster", "PTP Grandmaster"),
tree(tree_)
{
	addAndMakeVisible(&initialPTPTimeLabel);
	addAndMakeVisible(&initialPTPTimeEditor);
	addAndMakeVisible(&initialPTPTimeButton);
	addAndMakeVisible(&automaticPTPTimeToggle);

	initialPTPTimeEditor.setColour(TextEditor::outlineColourId, Colours::black);

	initialPTPTimeLabel.setText("PTP Start Time:", dontSendNotification);

	initialPTPTimeEditor.setInputRestrictions(19, Strings::decChars);
	initialPTPTimeEditor.setText("0");
	initialPTPTimeEditor.setEnabled(false);
	initialPTPTimeButton.setButtonText("Set Time");
	initialPTPTimeButton.setEnabled(false);

	initialPTPTimeButton.addListener(this);

	automaticPTPTimeToggle.setButtonText("Automatic Start Time");
	automaticPTPTimeToggle.addListener(this);

	tree.addListener(this);
}


void GrandmasterComponent::resized()
{
	int x = 0;
	int y = 28;
	int w = 150;
	int h = 23;
	initialPTPTimeLabel.setBounds(x, y, w, h * 2);
	int buttonW = 90;
	int toggleW = 170;
	int toggleX = x + w;
	automaticPTPTimeToggle.setBounds(toggleX, y, toggleW, h);
	y += h;
	initialPTPTimeEditor.setBounds(x + w, y + 2, w, h);
	initialPTPTimeButton.setBounds(initialPTPTimeEditor.getX(), y + initialPTPTimeEditor.getHeight() + 5, buttonW, h);
}

void GrandmasterComponent::buttonClicked(Button* button)
{
	if (button == &initialPTPTimeButton)
	{
// 		ValueTree timeTree(controller->getTree(Identifiers::PTP));
// 		int64 initialTime(initialPTPTimeEditor.getText().getLargeIntValue());
// 		int64 previousTime = timeTree[Identifiers::PTPInitialTime];
// 
// 		initialTime = jlimit(int64(0), 0x7fffffffffffffffLL, initialTime);
// 
// 		if (previousTime == initialTime)
// 		{
// 			timeTree.sendPropertyChangeMessage(Identifiers::PTPInitialTime);
// 		}
// 		else
// 		{
// 			timeTree.setProperty(Identifiers::PTPInitialTime, initialTime, nullptr);
// 		}
// 
// 		initialPTPTimeEditor.setText(String(initialTime));
// 
 		return;
	}

	if (button == &automaticPTPTimeToggle)
	{
		tree.setProperty(Identifiers::PTPAutomaticGMTime, button->getToggleState(), nullptr);
		return;
	}
}

void GrandmasterComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
	//DBG("GrandmasterComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << "  " << property.toString() << "  " << treeWhosePropertyHasChanged[property].toString());

	if (Identifiers::PTPAutomaticGMTime == property)
	{
		bool automaticTimeEnabled = tree[Identifiers::PTPAutomaticGMTime];
		automaticPTPTimeToggle.setToggleState(automaticTimeEnabled, dontSendNotification);
		initialPTPTimeEditor.setEnabled(!automaticTimeEnabled);
		initialPTPTimeButton.setEnabled(!automaticTimeEnabled);
		return;
	}

	if (Identifiers::PTPInitialTime == property)
	{
		int64 time = tree[Identifiers::PTPInitialTime];
		initialPTPTimeEditor.setText(String(time), false);
		return;
	}
}

void GrandmasterComponent::valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded)
{
}

void GrandmasterComponent::valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
}

void GrandmasterComponent::valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
}

void GrandmasterComponent::valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged)
{
}
