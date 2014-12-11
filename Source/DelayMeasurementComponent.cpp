#include "base.h"
#include "DelayMeasurementComponent.h"
#include "Identifiers.h"
#include "SettingsComponent.h"


DelayMeasurementComponent::DelayMeasurementComponent(ValueTree tree):
GroupComponent("Delay Measurement", "Delay Measurement"),
pDelayRequestLabel(String::empty, "PDelay_Req"),
pDelayResponseLabel(String::empty, "PDelay_Resp"),
pDelayFollowupLabel(String::empty, "PDelay_Followup"),
pDelayRequestIntervalLabel(String::empty, "Send PDelay_Req"),
sentLabel(String::empty, "Sent"),
receivedLabel(String::empty, "Received"),
workbenchSettingsTree(tree)
{
	addAndMakeVisible(&pDelayRequestIntervalLabel);

	pDelayRequestIntervalComboBox.addItem("Disabled", SettingsComponent::CONFIG_DELAY_REQUESTS_DISABLED);
	for (int msec = SettingsComponent::MIN_DELAY_REQUEST_INTERVAL_MILLISECONDS;
		msec <= SettingsComponent::MAX_DELAY_REQUEST_INTERVAL_MILLISECONDS;
		msec <<= 1)
	{
		pDelayRequestIntervalComboBox.addItem("Every " + String(msec) + " milliseconds", msec);
	}
	pDelayRequestIntervalComboBox.getSelectedIdAsValue().referTo(workbenchSettingsTree.getPropertyAsValue(Identifiers::PTPDelayRequest, nullptr));
	addAndMakeVisible(&pDelayRequestIntervalComboBox);

	addAndMakeVisible(&pDelayRequestLabel);
	addAndMakeVisible(&pDelayResponseLabel);
	addAndMakeVisible(&pDelayFollowupLabel);
	addAndMakeVisible(&sentLabel);
	addAndMakeVisible(&receivedLabel);
	addAndMakeVisible(&sentPDelayRequestReadout);
	addAndMakeVisible(&sentPDelayResponseReadout);
	addAndMakeVisible(&sentPDelayFollowupReadout);
	addAndMakeVisible(&receivedPDelayRequestReadout);
	addAndMakeVisible(&receivedPDelayResponseReadout);
	addAndMakeVisible(&receivedPDelayFollowupReadout);

	Colour c(Colours::grey.withAlpha(0.2f));

	sentPDelayRequestReadout.setColour(Label::backgroundColourId, c);
	sentPDelayResponseReadout.setColour(Label::backgroundColourId, c);
	sentPDelayFollowupReadout.setColour(Label::backgroundColourId, c);
	receivedPDelayRequestReadout.setColour(Label::backgroundColourId, c);
	receivedPDelayResponseReadout.setColour(Label::backgroundColourId, c);
	receivedPDelayFollowupReadout.setColour(Label::backgroundColourId, c);

	sentPDelayRequestReadout.setJustificationType(Justification::centred);
	sentPDelayResponseReadout.setJustificationType(Justification::centred);
	sentPDelayFollowupReadout.setJustificationType(Justification::centred);
	receivedPDelayRequestReadout.setJustificationType(Justification::centred);
	receivedPDelayResponseReadout.setJustificationType(Justification::centred);
	receivedPDelayFollowupReadout.setJustificationType(Justification::centred);
}

void DelayMeasurementComponent::resized()
{
	int x = 75;
	int y = 10;
	int w = 90;
	int h = 20;

	pDelayRequestLabel.setBounds(x, y, w, h);
	
	x += w;
	pDelayResponseLabel.setBounds(x, y, w, h);
	
	x += w;
	pDelayFollowupLabel.setBounds(x, y, w, h);

	x = 0;
	y += 40;
	w = 75;
	sentLabel.setBounds(x, y, w, h);

	w = 75;

	x += w + 10;
	sentPDelayRequestReadout.setBounds(x, y, w, h);
	x += w + 10;

	sentPDelayResponseReadout.setBounds(x, y, w, h);
	x += w + 10;

	sentPDelayFollowupReadout.setBounds(x, y, w, h);
	
	x = 0;
	y += 40;
	receivedLabel.setBounds(x, y, w, h);

	x += w + 10;
	receivedPDelayRequestReadout.setBounds(x, y, w, h);
	x += w + 10;

	receivedPDelayResponseReadout.setBounds(x, y, w, h);
	x += w + 10;

	receivedPDelayFollowupReadout.setBounds(x, y, w, h);

	x = 0;
	y += 30;
	w = 100;
	pDelayRequestIntervalLabel.setBounds(x, y, w, h);

	x += w;
	w = 200;
	pDelayRequestIntervalComboBox.setBounds(x, y, w, h);
}
