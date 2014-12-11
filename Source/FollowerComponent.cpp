#include "base.h"
#include "FollowerComponent.h"
#include "Strings.h"
#include "BinaryData.h"


FollowerComponent::FollowerComponent() :
GroupComponent("PTP Follower", "PTP Follower")
{
	addAndMakeVisible(&syncFollowupLabel);
	addAndMakeVisible(&syncFollowupReadout);

	Colour c(Colours::grey.withAlpha(0.2f));

	syncFollowupReadout.setColour(Label::backgroundColourId, c);


	syncFollowupReadout.setJustificationType(Justification::centred);

	syncFollowupLabel.setText("Sync / Follow_Up Received:", dontSendNotification);
}

void FollowerComponent::resized()
{
	int x = 0;
	int y = 28;
	int w = 170;
	int h = 23;
	syncFollowupLabel.setBounds(x, y, w, h);
	syncFollowupReadout.setBounds(x + w, y, w, h);
}