#include "base.h"
#include "DelayMeasurementComponent.h"
#include "Identifiers.h"
#include "SettingsComponent.h"


DelayMeasurementComponent::DelayMeasurementComponent(ValueTree tree):
GroupComponent("Delay Measurement", "Delay Measurement"),
pDelayRequestIntervalLabel(String::empty, "Send PDelay_Req"),
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
}

void DelayMeasurementComponent::resized()
{
	int h = 20;
	int x = 10;
	int w = 140;
	int y = 32;
	pDelayRequestIntervalLabel.setBounds(x, y, w, h);

	x += w;
	w = 180;
	pDelayRequestIntervalComboBox.setBounds(x, y, w, h);
}
