#include "base.h"
#include "GrandmasterComponent.h"
#include "Strings.h"
#include "BinaryData.h"

GrandmasterComponent::GrandmasterComponent():
GroupComponent("PTP Grandmaster", "PTP Grandmaster")
{
	addAndMakeVisible(&syncFollowupLabel);
	addAndMakeVisible(&syncFollowupReadout);
	addAndMakeVisible(&initialPTPTimeLabel);
	addAndMakeVisible(&initialPTPTimeEditor);
	addAndMakeVisible(&initialPTPTimeButton);
	addAndMakeVisible(&automaticPTPTimeToggle);

	syncFollowupReadout.setColour(Label::backgroundColourId, Colours::grey.withAlpha(0.2f));
	syncFollowupReadout.setJustificationType(Justification::centred);

	initialPTPTimeEditor.setColour(TextEditor::outlineColourId, Colours::black);

	syncFollowupLabel.setText("Sync / Follow_Up Sent:", dontSendNotification);
	initialPTPTimeLabel.setText("PTP Start Time:", dontSendNotification);

	initialPTPTimeEditor.setInputRestrictions(19, Strings::decChars);
	initialPTPTimeEditor.setText("0");
	initialPTPTimeButton.setButtonText("Set Time");

	initialPTPTimeButton.addListener(this);

	automaticPTPTimeToggle.setButtonText("Automatic Start Time");
}


void GrandmasterComponent::resized()
{
	int x = 0;
	int y = 28;
	int w = 150;
	int h = 23;
	syncFollowupLabel.setBounds(x, y, w, h);
	syncFollowupReadout.setBounds(x + w, y, w, h);
	y += h + 10;
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
// 		return;
	}
}
