#pragma once

#include "FaultInjection.h"

class PTPFaultInjectionComponent : 
	public GroupComponent, 
	public ComboBox::Listener, 
	public Label::Listener, 
	public Button::Listener,
	public ValueTree::Listener
{
public:
	PTPFaultInjectionComponent(ValueTree ptpFaultTree_);
	~PTPFaultInjectionComponent();

	virtual void resized();

	virtual void comboBoxChanged( ComboBox* comboBoxThatHasChanged );

	virtual void labelTextChanged( Label* labelThatHasChanged );

	virtual void buttonClicked( Button* );

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

	virtual void paint( Graphics& g );

protected:
	ToggleButton enableButton;

	Label groupCountLeftLabel;
	Label groupCountLabel;
	Label groupCountRightLabel;

	ComboBox repeatCombo;
	Label periodEditorLabel;
	Label periodLabel;
	Label packetsLabel;

	Label stopCountLeftLabel;
	Label stopCountLabel;
	Label stopCountRightLabel;

	ValueTree faultTree;

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

