#pragma once



class DelayMeasurementComponent : public GroupComponent
{
public:
	DelayMeasurementComponent(ValueTree tree);

	virtual void resized();

protected:
	Label pDelayRequestLabel;
	Label pDelayResponseLabel;
	Label pDelayFollowupLabel;
	Label sentLabel;
	Label receivedLabel;

	Label sentPDelayRequestReadout;
	Label sentPDelayResponseReadout;
	Label sentPDelayFollowupReadout;
	Label receivedPDelayRequestReadout;
	Label receivedPDelayResponseReadout;
	Label receivedPDelayFollowupReadout;

	Label pDelayRequestIntervalLabel;
	ComboBox pDelayRequestIntervalComboBox;

	ValueTree workbenchSettingsTree;
};