#include "base.h"
#include "PTPCorruptionComponent.h"
#include "Identifiers.h"
#include "Strings.h"


PTPCorruptionComponent::PTPCorruptionComponent(ValueTree corruptionTree_, int64 adapterGuid_) :
	corruptionTree(corruptionTree_),
	adapterGuid(adapterGuid_)
{
	titleLabel.setJustificationType(Justification::centred);
	titleLabel.setColour(Label::backgroundColourId, Colours::lightgrey.withAlpha(0.5f));
	addAndMakeVisible(titleLabel);
}

PTPCorruptionComponent::~PTPCorruptionComponent()
{
}

void PTPCorruptionComponent::resized()
{
	int h = 20;
	int y = h;

	titleLabel.setBounds(0, 0, getWidth(), h);

	for (int i = 0; i < fieldComponents.size(); ++i)
	{
		FieldComponent* fieldComponent = fieldComponents[i];

		fieldComponent->setBounds(0, y, getWidth(), h);
		y += h;
	}
}


void PTPCorruptionComponent::addFieldComponent(String const fieldName, PacketPTP::Field &field)
{
	ValueTree fieldTree(corruptionTree.getChildWithName(field.identifier));
	FieldComponent * fieldComponent = new FieldComponent(fieldTree, fieldName, field);
	addAndMakeVisible(fieldComponent);
	fieldComponents.add(fieldComponent);
}

PTPCorruptionComponent::FieldComponent::FieldComponent(ValueTree fieldTree_,String const fieldName, PacketPTP::Field &field_) :
	field(field_),
	fieldTree(fieldTree_),
	valueLabel(this),
	resetButton("Set default value for this field")
{
	nameLabel.setText(fieldName, dontSendNotification);
	nameLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(&nameLabel);

	if (field.canCorrupt)
	{
		bool enabled = fieldTree[Identifiers::Enabled];
		var const &value(fieldTree[Identifiers::Value]);
		String valueText(printValue(value, field.numBits));
		valueLabel.setText(valueText, dontSendNotification);
		
		valueLabel.setJustificationType(Justification::centred);
		valueLabel.setColour(Label::outlineColourId, Colours::darkgrey);
		valueLabel.setEnabled(enabled);
		valueLabel.setEditable(enabled);
		addAndMakeVisible(&valueLabel);
		valueLabel.addListener(this);

		addAndMakeVisible(&enabledButton);
		enabledButton.setName("PTP enable field corruption button " + fieldName);
		enabledButton.setTooltip("Enable editing the default value for " + fieldName);
		enabledButton.setToggleState( enabled, dontSendNotification);
		enabledButton.addListener(this);

		addChildComponent(&resetButton);
		resetButton.addListener(this);

		updateResetButton();

		fieldTree.addListener(this);
	}
}

PTPCorruptionComponent::FieldComponent::~FieldComponent()
{
}

void PTPCorruptionComponent::FieldComponent::resized()
{
	int h = getHeight() - 2;
	int w = h + 5;
	enabledButton.setBounds(0, 0, w, h);

	w = (getWidth() - h) / 2;
	nameLabel.setBounds(enabledButton.getRight(), 1, w, h);

	juce::Rectangle<int> r(nameLabel.getBounds().translated(w, 0));
	r.setWidth(getWidth() - r.getX());
	valueLabel.setBounds(r);

	resetButton.setBounds(getWidth() - h, 0, h, h);
}

void PTPCorruptionComponent::FieldComponent::buttonClicked( Button* button)
{
	if (&enabledButton == button)
	{
		bool enabled = enabledButton.getToggleState();
		valueLabel.setEnabled(enabled);
		valueLabel.setEditable(enabled);
		
		fieldTree.setProperty(Identifiers::Enabled, enabled, nullptr);

		updateResetButton();
		return;
	}

	if (&resetButton == button)
	{
		String valueText(printValue( var(field.correctValueBigEndian), field.numBits));
		valueLabel.setText(valueText, sendNotification);
		return;
	}
}

String PTPCorruptionComponent::FieldComponent::printValue( var const value, int const numBits )
{
	String valueText;
	if (value.isBinaryData())
	{
		MemoryBlock*block = value.getBinaryData();

		valueText = String::toHexString( block->getData(), (int)block->getSize(), 0);

		int numNibbles(numBits/ 4);
		if (numNibbles > valueText.length())
		{
			int numZerosToAdd(numNibbles - valueText.length());
			valueText = String::repeatedString("0", numZerosToAdd) + valueText;
		}

		if (4 == numBits)
		{
			valueText = valueText.getLastCharacters(1);
		}
		else if (numBits >= 16)
		{
			String oldText(valueText);
			int groupSize = (block->getSize() == 3) ? 2 : 4; // characters
			valueText = String::empty;

			for (int i = 0; i < oldText.length(); i += groupSize)
			{
				String temp(oldText.substring(i, i + groupSize));
				valueText += " " + temp;
			}
		}
	}

	return valueText;
}

void PTPCorruptionComponent::FieldComponent::labelTextChanged( Label* labelThatHasChanged )
{
	String text(labelThatHasChanged->getText().retainCharacters(Strings::hexChars));
	if (text.length() & 1)
		text = '0' + text;

	MemoryBlock tempBlock;
	tempBlock.loadFromHexString(text);
	tempBlock.setSize(field.correctValueBigEndian.getSize(), true);
	var tempVar(tempBlock);
	fieldTree.setProperty(Identifiers::Value, tempVar, nullptr);
	updateResetButton();
}

void PTPCorruptionComponent::FieldComponent::updateResetButton()
{
	bool enabled = fieldTree[Identifiers::Enabled];
	var tempVar(fieldTree[Identifiers::Value]);
	MemoryBlock *block = tempVar.getBinaryData();
	bool different = block && *block != field.correctValueBigEndian;
	resetButton.setVisible(different && enabled);
}

int PTPCorruptionComponent::FieldComponent::getFieldNumBits()
{
	jassert(field.numBits > 0 && field.numBits <= 96);
	jassert(field.identifier.isValid());
	return field.numBits;
}

void PTPCorruptionComponent::FieldComponent::valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property)
{
	if (Identifiers::Value == property)
	{
		var const &value(fieldTree[Identifiers::Value]);
		String valueText(printValue(value, field.numBits));
		valueLabel.setText(valueText, dontSendNotification);
		return;
	}

	if (Identifiers::Enabled == property)
	{
		bool enabled = fieldTree[Identifiers::Enabled];
		enabledButton.setToggleState(enabled, dontSendNotification);
		valueLabel.setEnabled(enabled);
		valueLabel.setEditable(enabled);
		return;
	}
}

void PTPCorruptionComponent::FieldComponent::valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded)
{
}

void PTPCorruptionComponent::FieldComponent::valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved)
{
}

void PTPCorruptionComponent::FieldComponent::valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex)
{
}

void PTPCorruptionComponent::FieldComponent::valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged)
{
}

PTPCorruptionComponent::FieldComponent::FieldLabel::FieldLabel(FieldComponent* fieldComponent_):
	fieldComponent(fieldComponent_)
{
}

TextEditor* PTPCorruptionComponent::FieldComponent::FieldLabel::createEditorComponent()
{
	TextEditor* const editor_ = Label::createEditorComponent();
	int numChars(fieldComponent->getFieldNumBits() / 4);
	if (numChars < 8)
	{
		numChars += numChars / 2;
	}
	else
	{
		numChars +=  numChars / 4;
	}
	editor_->setInputRestrictions(numChars, Strings::fieldChars);
	return editor_;
}

void PTPCorruptionComponent::FieldComponent::FieldLabel::textWasChanged()
{
	if (fieldComponent->getFieldNumBits() >= 8)
	{
		MemoryBlock block;
		String labelText(getText());
		labelText = labelText.removeCharacters(":- ");
		labelText = labelText.substring(0, fieldComponent->getFieldNumBits() / 4);

		if (labelText.length() & 1)
			labelText = '0' + labelText;	// even number of nibbles

		block.loadFromHexString(labelText);
		labelText = fieldComponent->printValue(block, fieldComponent->getFieldNumBits());
		setText(labelText, sendNotification);
	}
}
