#include "base.h"
#include "DeviceCallout.h"
#include "Controller.h"
#include "Identifiers.h"
#include "CallbackMetrics.h"
#include "Processor.h"
#include "Device.h"

DeviceCallout::DeviceCallout(ValueTree deviceTree_ ):
	deviceTree(deviceTree_),
	topLabel(String::empty,"Audio Device"),
	minCallbackIntervalLabel(String::empty,"Minimum callback interval (msec)"),
	averageCallbackIntervalLabel(String::empty,"Average callback interval (msec)"),
	maxCallbackIntervalLabel(String::empty,"Maximum callback interval (msec)"),
	inputChannelsLabel(String::empty, "Input channels"),
	outputChannelsLabel(String::empty,"Output channels"),
	inputChannelCountSlider (Slider:: LinearHorizontal, Slider:: TextBoxRight),
	outputChannelCountSlider (Slider:: LinearHorizontal, Slider:: TextBoxRight)
{
	setSize(500, 280);

	topLabel.setJustificationType (Justification::centred);
	Font f(topLabel.getFont());
	f.setBold(true);
	topLabel.setFont(f);
	addAndMakeVisible(& topLabel);

	minCallbackIntervalLabel.setJustificationType (Justification::centredLeft);
	//addAndMakeVisible(&minCallbackIntervalLabel);
	minCallbackIntervalReadout.setJustificationType (Justification::centred);
	minCallbackIntervalReadout.setColour(Label:: outlineColourId, Colours::darkslategrey);
	//addAndMakeVisible(&minCallbackIntervalReadout);

	averageCallbackIntervalLabel.setJustificationType (Justification::centredLeft);
	//addAndMakeVisible(&averageCallbackIntervalLabel);
	averageCallbackIntervalReadout.setJustificationType (Justification::centred);
	averageCallbackIntervalReadout.setColour(Label:: outlineColourId, Colours::darkslategrey);
	//addAndMakeVisible(&averageCallbackIntervalReadout);

	maxCallbackIntervalLabel.setJustificationType (Justification::centredLeft);
	//addAndMakeVisible(&maxCallbackIntervalLabel);
	maxCallbackIntervalReadout.setJustificationType (Justification::centred);
	maxCallbackIntervalReadout.setColour(Label:: outlineColourId, Colours::darkslategrey);
	//addAndMakeVisible(&maxCallbackIntervalReadout);

	//StringArray const &device_names (controller_->deviceManager.getFriendlyDeviceNames());
	deviceCombo.clear(dontSendNotification);
	deviceCombo.addItem("None", -1);
	//deviceCombo.addItemList(device_names, 1);
	deviceCombo.setSelectedId -1,dontSendNotification);

	var deviceName(deviceTree.getProperty(Identifiers::DeviceName,String::empty));
	if (deviceName.toString().isNotEmpty())
		deviceCombo.setText(deviceName.toString(), dontSendNotification);

	addAndMakeVisible(&deviceCombo);
	deviceCombo.addListener(this);

	fillSampleRateCombo();
	addAndMakeVisible(&sampleRateCombo);
	sampleRateCombo.addListener(this);

	addAndMakeVisible(& inputChannelsLabel);
	addAndMakeVisible (& outputChannelsLabel);

	//inputChannelCountSlider.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(& inputChannelCountSlider);
	//outputChannelCountSlider.setChangeNotificationOnlyOnRelease(true);
	addAndMakeVisible(& outputChannelCountSlider);

	configureChannelCountSlider(inputChannelCountSlider, deviceTree_.getChildWithName(Identifiers::Input));
	configureChannelCountSlider(outputChannelCountSlider, deviceTree_.getChildWithName(Identifiers::Output));

	inputChannelCountSlider.addListener(this);
	outputChannelCountSlider.addListener(this);

	deviceTree.addListener(this);

	//startTimer(250);
}

void DeviceCallout::resized()
{
	int x,w;
	int h = 40;
	topLabel.setBounds(0, 0, getWidth(), h);

	w = proportionOfWidth(0.8f);
	h = 30;
	x = (getWidth() - w) >> 1;
	juce::Rectangle<int> r( x, topLabel.getBottom() + 5, w, h);
	deviceCombo.setBounds(r);
	r.translate(0, r.getHeight() + 2);
	sampleRateCombo.setBounds(r);

	w = proportionOfWidth(0.6f) - x;
	juce::Rectangle<int> left (x, sampleRateCombo.getBottom() + 5, w, h);
// 	minCallbackIntervalLabel.setBounds(left);
// 	juce::Rectangle<int> right(left);
// 	right.reduce(0, 5);
// 	right.setX(left.getRight() + 45);
// 	right.setWidth(100);
// 	minCallbackIntervalReadout.setBounds(right);
// 
// 	left.translate(0,h + 2);
// 	right.translate(0,h + 2);
// 	averageCallbackIntervalLabel.setBounds(left);
// 	averageCallbackIntervalReadout.setBounds(right);
// 	left.translate(0,h + 2);
// 	right.translate(0,h + 2);
// 	maxCallbackIntervalLabel.setBounds(left);
// 	maxCallbackIntervalReadout.setBounds(right);

	left.translate(0,10);
	inputChannelsLabel.setBounds(left);
	left.translate(0,left.getHeight() + 2);
	inputChannelCountSlider.setBounds(left.getX(), left.getY(), proportionOfWidth(0.75f), 30);

	left.translate(0,left.getHeight() + 5);
	outputChannelsLabel.setBounds(left);
	left.translate(0,left.getHeight() + 2);
	outputChannelCountSlider.setBounds(left.getX(), left.getY(), proportionOfWidth(0.75f), 30);
}

void DeviceCallout::timerCallback()
{
#if 0
	CallbackMetrics metrics;
	controller->processor->getMetrics(metrics);

	minCallbackIntervalReadout.setText(String (metrics.minCallbackIntervalMsec, 1), dontSendNotification);
	averageCallbackIntervalReadout.setText(String (metrics.averageCallbackIntervalMsec, 1), dontSendNotification);
	maxCallbackIntervalReadout.setText(String (metrics.maxCallbackIntervalMsec, 1), dontSendNotification);
#endif

}

void DeviceCallout::comboBoxChanged( ComboBox* comboBoxThatHasChanged )
{
	if (comboBoxThatHasChanged == &deviceCombo)
	{
		//String actualName(controller->deviceManager.getActualName(deviceCombo.getText()));
		//deviceTree.setProperty(Identifiers::DeviceName, actualName, nullptr);
		return;
	}

	if (comboBoxThatHasChanged == &sampleRateCombo)
	{
		String text(sampleRateCombo.getText());
		double rate = text.getDoubleValue();
		deviceTree.setProperty(Identifiers::SampleRate, rate, nullptr);
		return;
	}
}


void DeviceCallout::sliderValueChanged( Slider* slider )
{

	if (slider == &inputChannelCountSlider)
	{
		deviceTree.getChildWithName(Identifiers::Input).setProperty(Identifiers::CurrentChannelCount, slider->getValue(), nullptr);
		return;
	}

	if (slider == &outputChannelCountSlider)
	{
		deviceTree.getChildWithName(Identifiers::Output).setProperty(Identifiers::CurrentChannelCount, slider->getValue(), nullptr);
		return;
	}
}

void DeviceCallout::paint( Graphics& g )
{
	g.fillAll(Colours::white);
}

void DeviceCallout::configureChannelCountSlider(Slider & channelCountSlider, ValueTree channelsTree)
{
	double max = channelsTree[Identifiers::MaxChannelCount];
	double min = (0.0 == max) ? 0.0 : 1.0;
	double current = channelsTree[Identifiers::CurrentChannelCount]; 

	channelCountSlider.setEnabled(max > 1.0);
	channelCountSlider.setRange(min, max, 1.0);
	channelCountSlider.setValue(current, sendNotification);
	channelCountSlider.setTextValueSuffix(" of " + String((int) max));
}

void DeviceCallout::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (Identifiers::CurrentChannelCount == property || Identifiers::MaxChannelCount == property)
	{
		Identifier type(treeWhosePropertyHasChanged.getType());

		if (Identifiers::Input == type)
		{
			configureChannelCountSlider(inputChannelCountSlider, treeWhosePropertyHasChanged);
			return;
		}

		if (Identifiers::Output == type)
		{
			configureChannelCountSlider(outputChannelCountSlider, treeWhosePropertyHasChanged);
			return;
		}
		return;
	}

	if (Identifiers::DeviceName == property)
	{
		fillSampleRateCombo();
		return;
	}
}

void DeviceCallout::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void DeviceCallout::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void DeviceCallout::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void DeviceCallout::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

void DeviceCallout::fillSampleRateCombo()
{
//Needs to be rewritten

// 	int deviceIndex = deviceTree[Identifiers::Index];
// 	Device* device = controller->deviceManager.getDevice(deviceIndex);
// 	if (nullptr == device->ioDevice)
// 		return;
// 
// 	sampleRateCombo.clear();
// 	Array<double> rates(device->ioDevice->getAvailableSampleRates());
// 	double currentRate = device->ioDevice->getCurrentSampleRate();
// 	for (int i = 0; i < rates.size(); ++i)
// 	{
// 		int id = i + 1;
// 		sampleRateCombo.addItem(String(rates[i]) + " Hz", id);
// 		if (rates[i] == currentRate)
// 		{
// 			sampleRateCombo.setSelectedId(id, dontSendNotification);
// 		}
// 	}
}
