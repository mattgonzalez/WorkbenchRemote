#include "base.h"
#include "DeviceHeaderComponent.h"
#include "Identifiers.h"
#include "BinaryData.h"
#include "DeviceCallout.h"

DeviceHeaderComponent::DeviceHeaderComponent(ValueTree tree_):
	deviceTree(tree_)
{
	//StringArray const device_names (controller_->deviceManager.getFriendlyDeviceNames());
	deviceCombo.clear(dontSendNotification);
	deviceCombo.addItem("None", -1);
	//deviceCombo.addItemList(device_names, 1);
	addAndMakeVisible(&deviceCombo);
	deviceCombo.addListener(this);

// 	deviceNameValue = deviceTree.getPropertyAsValue(Identifiers::DeviceName,nullptr);
// 	valueChanged(deviceNameValue);
// 	deviceNameValue.addListener(this);

	addAndMakeVisible(&gearButton);
	gearButton.addListener(this);
}

DeviceHeaderComponent::~DeviceHeaderComponent()
{
}

void DeviceHeaderComponent::resized()
{
	Image gear(ImageCache::getFromMemory(BinaryData::gearbutton_png, BinaryData::gearbutton_pngSize));

	deviceCombo.setBoundsRelative(0.1f,0.12f,0.7f,0.76f);

	gearButton.setBounds( deviceCombo.getRight() + 5,
		deviceCombo.getY (),
		gear.getWidth(),
		gear.getHeight());
}

void DeviceHeaderComponent::comboBoxChanged( ComboBox* comboBoxThatHasChanged )
{
	if (comboBoxThatHasChanged == &deviceCombo)
	{
		//String actualName(controller->deviceManager.getActualName(deviceCombo.getText()));
		//deviceTree.setProperty(Identifiers::DeviceName, actualName, nullptr);
		return;
	}
}


void DeviceHeaderComponent::valueChanged( Value& value )
{
	if (value.refersToSameSourceAs(deviceNameValue))
	{
		//String friendlyName(controller->deviceManager.getFriendlyName(value.toString()));

		//deviceCombo.setText(friendlyName, dontSendNotification);
		if (deviceCombo.getSelectedId() == 0)
			deviceCombo.setSelectedId(-1, dontSendNotification);
		return;
	}
}

void DeviceHeaderComponent::paint( Graphics& g )
{
	g.setColour(Colours::white);
	juce::Rectangle<float> r(getLocalBounds().toFloat());
	float h = r.getHeight();
	r.setHeight( r.getHeight() /** 2.0f*/);
	g.fillRoundedRectangle(r, h * 0.25f);
}


void DeviceHeaderComponent::buttonClicked( Button* )
{
	DeviceCallout* content = new DeviceCallout(deviceTree);

	juce::Rectangle<int> b(gearButton.getBounds());
	Component* parent = getParentComponent()->getParentComponent();
	b = parent->getLocalArea(this,b);
	CallOutBox & box (CallOutBox::launchAsynchronously (content, b, parent));
	box.setAlpha(0.9f);
}
