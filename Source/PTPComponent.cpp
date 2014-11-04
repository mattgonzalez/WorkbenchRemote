#include "base.h"
#include "WorkbenchClient.h"
#include "Identifiers.h"
#include "PTPComponent.h"

PTPComponent::PTPComponent(ValueTree tree, WorkbenchClient * client_):
client(client_),
workbenchSettingsTree(tree.getChildWithName(Identifiers::WorkbenchSettings)),
ptpRoleLabel(String::empty, "PTP Role")
{
	addAndMakeVisible(&ptpRoleLabel);
	ptpRoleComboBox.addItem("PTP Follower", CONFIG_FOLLOWER);
	ptpRoleComboBox.addItem("PTP Grandmaster", CONFIG_GRANDMASTER);
	ptpRoleComboBox.addItem("Use best master clock algorithm (BMCA)", CONFIG_BMCA);
	ptpRoleComboBox.getSelectedIdAsValue().referTo(workbenchSettingsTree.getPropertyAsValue(Identifiers::StaticPTPRole, nullptr));
	addAndMakeVisible(&ptpRoleComboBox);
	ptpRoleLabel.attachToComponent(&ptpRoleComboBox, true);

	addAndMakeVisible(&grandmasterComponent);
	workbenchSettingsTree.addListener(this);
	}

PTPComponent::~PTPComponent()
{

}

void PTPComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
	if (property == Identifiers::StaticPTPRole)
	{
		if ((int)treeWhosePropertyHasChanged.getProperty(property) == CONFIG_GRANDMASTER)
		{
			grandmasterComponent.setVisible(true);
		}
		else
		{
			grandmasterComponent.setVisible(false);
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
}
