#pragma once

#include "PacketPTP.h"
#include "ResetButton.h"

class PTPCorruptionComponent: public Component
{
public:
	PTPCorruptionComponent(ValueTree corruptionTree_, int64 adapterGuid_);
	~PTPCorruptionComponent();

	virtual void resized() override;

protected:
	void addFieldComponent(String const fieldName, PacketPTP::Field &field);

	class FieldComponent: public Component, public Button::Listener, Label::Listener, public ValueTree::Listener
	{
	public:
		FieldComponent(ValueTree fieldTree_, String const fieldName, PacketPTP::Field &field_);
		~FieldComponent();

		ToggleButton enabledButton;
		Label nameLabel;

		class FieldLabel : public Label
		{
		public:
			FieldLabel(FieldComponent* fieldComponent_);

		protected:
			virtual TextEditor* createEditorComponent();

			virtual void textWasChanged();

			FieldComponent* fieldComponent;
		} valueLabel;

		int getFieldNumBits();

		virtual void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
		virtual void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
		virtual void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
		virtual void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
		virtual void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override;

	protected:
		virtual void resized();
		virtual void buttonClicked( Button* );
		virtual void labelTextChanged( Label* labelThatHasChanged );

		String printValue(var const value, int const numBits);
		void updateResetButton();

		ResetButton resetButton;

		PacketPTP::Field field;
		ValueTree fieldTree;
	};

	OwnedArray<FieldComponent> fieldComponents;
	Label titleLabel;
	ValueTree corruptionTree;
	int64 adapterGuid;
};