#pragma once
class GrandmasterComponent : public GroupComponent, public Button::Listener, public ValueTree::Listener
{
public:
	GrandmasterComponent(ValueTree tree_);

	virtual void resized();

	virtual void buttonClicked(Button*);
	virtual void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
	virtual void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
	virtual void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
	virtual void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
	virtual void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override;

protected:
	ValueTree tree;
	Label initialPTPTimeLabel;

	TextEditor initialPTPTimeEditor;
	TextButton initialPTPTimeButton;

	ToggleButton automaticPTPTimeToggle;
};