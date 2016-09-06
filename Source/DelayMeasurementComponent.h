#pragma once



class DelayMeasurementComponent : public GroupComponent
{
public:
	DelayMeasurementComponent(ValueTree tree);

	virtual void resized();

protected:
	Label pDelayRequestIntervalLabel;
	ComboBox pDelayRequestIntervalComboBox;

	ValueTree workbenchSettingsTree;
};