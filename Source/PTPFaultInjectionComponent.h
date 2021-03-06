#pragma once

#include "FaultInjection.h"
#include "PTPSyncCorruptionComponent.h"
#include "PTPFollowupCorruptionComponent.h"

class PTPFaultInjectionComponent : 
	public Component, 
	public ComboBox::Listener, 
	public Label::Listener, 
	public Button::Listener,
	public ValueTree::Listener
{
public:
	PTPFaultInjectionComponent(ValueTree ptpFaultTree_, int64 adapterGuid_);
	~PTPFaultInjectionComponent();

	virtual void resized();

	virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );

	virtual void labelTextChanged( Label* labelThatHasChanged );

	virtual void buttonClicked( Button* );

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved,  int indexFromWhichChildWasRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

	virtual void paint( Graphics& g );

protected:
	ToggleButton enableButton;

	Label groupCountLeftLabel;
	Label groupCountLabel;
	Label groupCountRightLabel;

	ComboBox repeatCombo;
	Label periodEditorLabel;
	Label periodRightLabel;

	Label stopCountLeftLabel;
	Label stopCountLabel;
	Label stopCountRightLabel;

	ValueTree faultTree;

	PTPSyncCorruptionComponent syncCorruptionComponent;
	PTPFollowupCorruptionComponent followupCorruptionComponent;

	class PacketPairDisplay : public Component
	{
	public:
		PacketPairDisplay(ValueTree &tree_);
		void paint (Graphics&g);

	protected:
		void drawBlocks(Graphics &g, juce::Rectangle<int> &area, int const count, Colour const color);
		ValueTree &tree;
	} packetPairDisplay;
};
