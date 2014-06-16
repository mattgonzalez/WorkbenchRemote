#pragma once

class Controller;

class DeviceCallout : public Component, 
	public Timer, 
	public Slider::Listener, 
	public ComboBox::Listener,
	public ValueTree::Listener
{
public:
	DeviceCallout(ValueTree deviceTree_);

	virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );
	virtual void paint( Graphics& g );
	virtual void resized();
	virtual void timerCallback();
	virtual void sliderValueChanged( Slider* slider );	
	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

protected:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeviceCallout)
	void configureChannelCountSlider(Slider & channelCountSlider, ValueTree channelsTree);
	void fillSampleRateCombo();

	ValueTree deviceTree;

	Label topLabel;
	ComboBox deviceCombo;
	ComboBox sampleRateCombo;
	Label minCallbackIntervalLabel;
	Label minCallbackIntervalReadout;
	Label averageCallbackIntervalLabel;
	Label averageCallbackIntervalReadout;
	Label maxCallbackIntervalLabel;
	Label maxCallbackIntervalReadout;
	Label inputChannelsLabel;
	Label outputChannelsLabel;
	Slider inputChannelCountSlider;
	Slider outputChannelCountSlider;
};