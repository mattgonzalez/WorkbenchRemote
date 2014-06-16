#pragma once


class OutputChannelCallout : 
	public Component, 
	public Timer, 
	public Slider::Listener, 
	public ComboBox::Listener, 
	public Button::Listener
{
public:
	OutputChannelCallout(ValueTree channelTree_);

	virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );
	virtual void paint( Graphics& g );
	virtual void resized();
	virtual void timerCallback();
	virtual void sliderValueChanged( Slider* slider );	
	void valueChanged( Value& value );

protected:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputChannelCallout)

	void fillModeCombo();

	virtual void buttonClicked( Button* );

	ValueTree channelTree;
	Label topLabel;
	ComboBox modeCombo;
	Slider gainSlider;
	TextButton muteButton;
	Slider toneFrequencySlider;
	ToggleButton allOutputsToggle;
};
