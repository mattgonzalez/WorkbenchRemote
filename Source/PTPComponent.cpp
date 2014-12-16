#include "base.h"
#include "WorkbenchClient.h"
#include "Identifiers.h"
#include "PTPComponent.h"

PTPComponent::PTPComponent(ValueTree tree, WorkbenchClient * client_):
client(client_),
workbenchSettingsTree(tree.getChildWithName(Identifiers::WorkbenchSettings)),
ptpInfoTree(tree.getChildWithName(Identifiers::PTPInfo)),
ptpRoleLabel(String::empty, "PTP Role"),
delayMeasurementComponent(tree.getChildWithName(Identifiers::WorkbenchSettings)),
ptpFaultInjectionComponent(tree.getChildWithName(Identifiers::PTPInfo))
{
	addAndMakeVisible(&ptpRoleLabel);
	ptpRoleComboBox.addItem("PTP Follower", CONFIG_FOLLOWER);
	ptpRoleComboBox.addItem("PTP Grandmaster", CONFIG_GRANDMASTER);
	ptpRoleComboBox.addItem("Use best master clock algorithm (BMCA)", CONFIG_BMCA);
	ptpRoleComboBox.getSelectedIdAsValue().referTo(workbenchSettingsTree.getPropertyAsValue(Identifiers::StaticPTPRole, nullptr));
	addAndMakeVisible(&ptpRoleComboBox);
	ptpRoleLabel.attachToComponent(&ptpRoleComboBox, true);

	addChildComponent(&grandmasterComponent);
	addAndMakeVisible(&followerComponent);
	addAndMakeVisible(&delayMeasurementComponent);
	addAndMakeVisible(&ptpFaultInjectionComponent);

	workbenchSettingsTree.addListener(this);
	}

PTPComponent::~PTPComponent()
{

}

void PTPComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
	if (property == Identifiers::StaticPTPRole)
	{
		switch ((int)treeWhosePropertyHasChanged.getProperty(property))
		{
		case CONFIG_GRANDMASTER:
		{
			grandmasterComponent.setVisible(true);
			followerComponent.setVisible(false);
			return;
		}
		case CONFIG_FOLLOWER:
		{
			grandmasterComponent.setVisible(false);
			followerComponent.setVisible(true);
			return;
		}
		case CONFIG_BMCA:
		{
			grandmasterComponent.setVisible(false);
			followerComponent.setVisible(false);
			return;
		}
		default:
		{
			grandmasterComponent.setVisible(false);
			followerComponent.setVisible(false);
			return;
		}
		}
	}
}

void PTPComponent::valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded)
{
}

void PTPComponent::valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved)
{
}

void PTPComponent::valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved)
{
}

void PTPComponent::valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged)
{
}

void PTPComponent::resized()
{
	juce::Rectangle<int>r(70, 20, 200, 20);
	ptpRoleComboBox.setBounds(r);
	grandmasterComponent.setBounds(10, 50, 350, 150);
	followerComponent.setBounds(10, 50, 350, 150);
	delayMeasurementComponent.setBounds(10, 200, 350, 150);
	ptpFaultInjectionComponent.setBounds(10, 350, 350, 400);
}
