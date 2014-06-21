#include "base.h"
#include "DeviceHeaderComponent.h"
#include "Identifiers.h"
#include "BinaryData.h"
#include "DeviceCallout.h"


DeviceHeaderComponent::DeviceHeaderComponent(ValueTree tree_, CriticalSection& lock_):
	deviceTree(tree_),
	audioDevicesTree(tree_.getParent()),
	lock(lock_)
{
	deviceCombo.addItem("None", -1);
	addAndMakeVisible(&deviceCombo);
	deviceCombo.addListener(this);

	addAndMakeVisible(&gearButton);
	gearButton.addListener(this);
	deviceTree.addListener(this);
	audioDevicesTree.addListener(this);
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
		ScopedLock locker(lock);
		String deviceName(comboBoxThatHasChanged->getText());
		deviceTree.setProperty(Identifiers::DeviceName, deviceName, nullptr);
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
	DeviceCallout* content = new DeviceCallout(deviceTree, lock);

	juce::Rectangle<int> b(gearButton.getBounds());
	Component* parent = getParentComponent()->getParentComponent();
	b = parent->getLocalArea(this,b);
	CallOutBox & box (CallOutBox::launchAsynchronously (content, b, parent));
	box.setAlpha(0.9f);
}

void DeviceHeaderComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (property == Identifiers::AvailableAudioDevices)
	{
		deviceCombo.clear();
		var deviceNames(treeWhosePropertyHasChanged.getProperty(Identifiers::AvailableAudioDevices));
		for (int i = 0; i < deviceNames.size(); i++)
		{
			deviceCombo.addItem(deviceNames[i], i + 1);
		}
		return;
	}

	if (treeWhosePropertyHasChanged != deviceTree)
	{
		return;
	}

	if (Identifiers::DeviceName == property)
	{
		//DBG("DeviceHeaderComponent::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());
		//DBG("   " << treeWhosePropertyHasChanged[property].toString());
		deviceCombo.setText(treeWhosePropertyHasChanged[property],  dontSendNotification);
		return;
	}
}

void DeviceHeaderComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void DeviceHeaderComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void DeviceHeaderComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void DeviceHeaderComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}
