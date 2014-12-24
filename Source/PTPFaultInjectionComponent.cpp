#include "base.h"
#include "PTPFaultInjectionComponent.h"
#include "Identifiers.h"
#include "Strings.h"
#include "PTPInfo.h"

PTPFaultInjectionComponent::PTPFaultInjectionComponent( ValueTree ptpFaultTree_) :
	groupCountLeftLabel(String::empty, "Send "),
	stopCountLeftLabel(String::empty,"Stop after "),
	enableButton("Enable fault injection"),
	faultTree(ptpFaultTree_),
	groupCountLabel(String::empty, ptpFaultTree_.getProperty(Identifiers::PTPNumBadSyncFollowupPairsPerCycle)),
	periodEditorLabel(String::empty, ptpFaultTree_.getProperty(Identifiers::PTPFaultInjectionCycleLengthPackets)),
	stopCountLabel(String::empty, ptpFaultTree_.getProperty(Identifiers::PTPNumFaultInjectionCycles)),
	packetsLabel(String::empty, "Packet"),
	packetPairDisplay(faultTree),
	GroupComponent("PTP Grandmaster Fault Injection", "PTP Grandmaster Fault Injection")
{
	enableButton.setToggleState(faultTree.getProperty(Identifiers::Enabled), dontSendNotification);
	enableButton.addListener(this);
	addAndMakeVisible(&enableButton);

	groupCountLabel.setEditable(true);
	groupCountLabel.setJustificationType(Justification::centredRight);
	groupCountLabel.setColour(Label::outlineColourId,Colours::lightgrey);
	groupCountLabel.addListener(this);

	addAndMakeVisible(&groupCountLeftLabel);
	addAndMakeVisible(&groupCountLabel);
	addAndMakeVisible(&groupCountRightLabel);

	repeatCombo.addItem("Once", PTPInfo::ONCE);
	repeatCombo.addItem("Repeat every ", PTPInfo::REPEAT);
	repeatCombo.addItem("Repeat continuously every ", PTPInfo::REPEAT_CONTINUOUSLY);
	addAndMakeVisible(&repeatCombo);
	repeatCombo.addListener(this);

	periodEditorLabel.setEditable(true);
	periodEditorLabel.setJustificationType(Justification::centredRight);
	periodEditorLabel.setColour(Label::outlineColourId,Colours::lightgrey);
	periodEditorLabel.addListener(this);
	addAndMakeVisible(&periodEditorLabel);

	addAndMakeVisible(&packetsLabel);

	stopCountLabel.setEditable(true);
	stopCountLabel.setJustificationType(Justification::centredRight);
	stopCountLabel.setColour(Label::outlineColourId,Colours::lightgrey);
	//labelTextChanged(&stopCountLabel);
	stopCountLabel.addListener(this);
	addAndMakeVisible(&stopCountLeftLabel);
	addAndMakeVisible(&stopCountLabel);
	addAndMakeVisible(&stopCountRightLabel);
	addAndMakeVisible(&periodLabel);
	repeatCombo.setSelectedId(faultTree.getProperty(Identifiers::PTPFaultInjectionCycleMode),sendNotification);

	addAndMakeVisible(&packetPairDisplay);

	faultTree.addListener(this);
	valueTreePropertyChanged(faultTree,Identifiers::PTPNumBadSyncFollowupPairsPerCycle);
	valueTreePropertyChanged(faultTree,Identifiers::PTPFaultInjectionCycleLengthPackets);
	valueTreePropertyChanged(faultTree,Identifiers::PTPNumFaultInjectionCycles);
}

PTPFaultInjectionComponent::~PTPFaultInjectionComponent()
{

}

void PTPFaultInjectionComponent::resized()
{
	int x = 10;
	int y = 20;
	int h = 28;

	int w = 139;
	enableButton.setBounds( (getWidth() - w) / 2, y, w, h);

	y += h + 20;
	groupCountLeftLabel.setBounds(x,y,50,h);
	groupCountLabel.setBounds(groupCountLeftLabel.getRight(),
		y,50,h);
	groupCountRightLabel.setBounds(groupCountLabel.getRight(),y,200,h);

	y += h + 2;
	periodLabel.setBounds(x, y, 250, h);
	y += h + 2;
	repeatCombo.setBounds(x,y,200,h);
	periodEditorLabel.setBounds(repeatCombo.getRight(),y,50,h);
	packetsLabel.setBounds(periodEditorLabel.getRight(), y, 50, h);

	y += h + 2;
	stopCountLeftLabel.setBounds(x,y,200,h);
	stopCountLabel.setBounds(stopCountLeftLabel.getRight(),
		y,50,h);
	stopCountRightLabel.setBounds(stopCountLabel.getRight(),y,200,h);


	y += h;

	packetPairDisplay.setBounds(x, y, 330, h * 3);
}

void PTPFaultInjectionComponent::comboBoxChanged( ComboBox* comboBoxThatHasChanged )
{
	if (&repeatCombo == comboBoxThatHasChanged)
	{
		bool periodVisible = true;
		bool stopVisible = false;

		switch (comboBoxThatHasChanged->getSelectedId())
		{
		case PTPInfo::ONCE:
			break;

		case PTPInfo::REPEAT:
			periodVisible = true;
			stopVisible = true;
			break;

		case PTPInfo::REPEAT_CONTINUOUSLY:
			periodVisible = true;
			break;
		}

		periodEditorLabel.setVisible(periodVisible);	
		periodLabel.setVisible(periodVisible);
		stopCountLeftLabel.setVisible(stopVisible);
		stopCountLabel.setVisible(stopVisible);
		stopCountRightLabel.setVisible(stopVisible);

		faultTree.setProperty(Identifiers::PTPFaultInjectionCycleMode, comboBoxThatHasChanged->getSelectedId(), nullptr);

		packetPairDisplay.repaint();

		return;
	}
}

void PTPFaultInjectionComponent::labelTextChanged( Label* labelThatHasChanged )
{
	String temp(labelThatHasChanged->getText().retainCharacters(Strings::decChars));

	int count = temp.getIntValue();

	if (labelThatHasChanged == &groupCountLabel)
	{
		count = jlimit(0, (int)PTPInfo::MAX_FAULT_INJECTION_CYCLE_LENGTH, count);
		labelThatHasChanged->setText(String(count), dontSendNotification);
		faultTree.setProperty(Identifiers::PTPNumBadSyncFollowupPairsPerCycle, count, nullptr);
		return;
	}

	if (labelThatHasChanged == &periodEditorLabel)
	{
		count = jlimit(0, (int)PTPInfo::MAX_FAULT_INJECTION_CYCLE_LENGTH, count);
		labelThatHasChanged->setText(String(count), dontSendNotification);
		faultTree.setProperty(Identifiers::PTPFaultInjectionCycleLengthPackets, count, nullptr);
		return;
	}

	if (labelThatHasChanged == &stopCountLabel)
	{
		count = jlimit(0, (int)PTPInfo::MAX_FAULT_INJECTION_CYCLES, count);
		labelThatHasChanged->setText(String(count), dontSendNotification);
		faultTree.setProperty(Identifiers::PTPNumFaultInjectionCycles, count, nullptr);
		return;
	}
}

void PTPFaultInjectionComponent::buttonClicked( Button* button )
{
	if (button == &enableButton)
	{
		faultTree.setProperty(Identifiers::Enabled, enableButton.getToggleState(), nullptr);
		return;
	}
}

void PTPFaultInjectionComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (Identifiers::PTPNumBadSyncFollowupPairsPerCycle == property)
	{
		String text("Faulty Sync/Follow_Up Pair");
		int count = faultTree[property];
		if (count != 1)
			text += 's';
		groupCountRightLabel.setText(text, dontSendNotification);
		groupCountLabel.setText( String(count), dontSendNotification);

		packetPairDisplay.repaint();

		return;
	}

	if (Identifiers::PTPFaultInjectionCycleLengthPackets == property || Identifiers::PTPFaultInjectionCycleMode == property)
	{
		int count = faultTree[Identifiers::PTPFaultInjectionCycleLengthPackets];
		String text = String::empty;
		String packetText = String::empty;

		if ((int)treeWhosePropertyHasChanged[Identifiers::PTPFaultInjectionCycleMode] == PTPInfo::ONCE)
		{
			text = "Sync/Follow_Up Pair will be sent";
			if (count != 1)
			{
				text = "Sync/Follow_Up Pairs will be sent";
			}	

			packetsLabel.setVisible(false);
		}
		else
		{
			text = "Sync/Follow_Up Pair will";
			packetText = "Packet";
			if (count != 1)
			{
				text = "Sync/Follow_Up Pairs will";
				packetText += "s";
			}
		}
		
		periodEditorLabel.setText(String(count), dontSendNotification);
		periodLabel.setText(text, dontSendNotification);
		packetsLabel.setText(packetText, dontSendNotification);
		packetsLabel.setVisible(true);

		packetPairDisplay.repaint();

		return;
	}

	if (Identifiers::PTPNumFaultInjectionCycles == property)
	{
		String text("Cycle");
		int count = faultTree[property];
		if (count != 1)
			text += 's';
		stopCountLabel.setText(String(count), dontSendNotification);
		stopCountRightLabel.setText(text, dontSendNotification);

		packetPairDisplay.repaint();

		return;
	}

	if (Identifiers::Enabled == property && treeWhosePropertyHasChanged == faultTree)
	{
		enableButton.setToggleState(treeWhosePropertyHasChanged.getProperty(Identifiers::Enabled), dontSendNotification);
		return;
	}
}

void PTPFaultInjectionComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void PTPFaultInjectionComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void PTPFaultInjectionComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void PTPFaultInjectionComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

void PTPFaultInjectionComponent::paint( Graphics& g )
{
	juce::Rectangle<float> r(enableButton.getBounds().toFloat());

	r.expand( 10.0f, 5.0f);
	g.setColour( Colours::lightslategrey.withAlpha(0.25f));
	g.fillRoundedRectangle(r, 5.0f);
	GroupComponent::paint(g);
}

PTPFaultInjectionComponent::PacketPairDisplay::PacketPairDisplay( ValueTree &tree_ ) :
	tree(tree_)
{

}

void PTPFaultInjectionComponent::PacketPairDisplay::paint( Graphics&g )
{
	int errorCount = tree[Identifiers::PTPNumBadSyncFollowupPairsPerCycle];
	int periodPackets = tree[Identifiers::PTPFaultInjectionCycleLengthPackets];
	if (0 == errorCount && 0 == periodPackets)
		return;

	int margin = 10;
	int w = roundFloatToInt( float(errorCount * (getWidth() - margin*2)) / float(periodPackets));
	w = jlimit(proportionOfWidth(0.15f), proportionOfWidth(0.85f), w);
	juce::Rectangle<int> errorArea(margin, proportionOfHeight(0.4f), w, 10);
	if (errorCount)
	{
		drawBlocks(g, errorArea, errorCount, Colours::red);

		if (errorCount > 2)
		{
			juce::Rectangle<int> textArea(errorArea.translated(0,errorArea.getHeight() + 2));
			g.setColour(Colours::black);
			g.drawText(String(errorCount), textArea, Justification::centred, false);
		}
	}
	else
	{
		errorArea.setWidth(0);
	}

	int goodPackets = periodPackets - errorCount;
	int mode = (int)tree[Identifiers::PTPFaultInjectionCycleMode];
	w = getWidth() - errorArea.getRight() - margin;
	juce::Rectangle<int> goodArea(errorArea.getRight(), errorArea.getY(), w, errorArea.getHeight());
	if (periodPackets > 0 && mode > PTPInfo::ONCE)
	{
		drawBlocks(g, goodArea, goodPackets, Colours::limegreen);
	}

	if (errorCount > 0 && periodPackets > 0 && mode > PTPInfo::ONCE)
	{
		Path p;
		Point<float> start(goodArea.toFloat().getTopRight());
		start = start.translated(/*JUCE_LIVE_CONSTANT*/(-5.0f), -5.0f);
		p.startNewSubPath(start);
		p.lineTo(start.translated(0.0f, -8.0f));
		Point<float> end(errorArea.toFloat().getTopLeft());
		end = end.translated(4.0f, -3.0f);
		Point<float> corner(end.translated(0.0f, -10.0f));
		p.lineTo(corner);
		Line<float> l(corner, end);
		p.addArrow(l, 0.5f, 5.0f, 5.0f); 

		g.setColour(Colours::black);
		g.strokePath(p, PathStrokeType(2.0f, PathStrokeType::curved, PathStrokeType::rounded));

		juce::Rectangle<int> textArea(errorArea);
		textArea = textArea.getUnion(goodArea);
		Justification j(Justification::centred);
		int minW = proportionOfWidth(0.25f);
		if (textArea.getWidth() < minW)
		{
			textArea.setWidth(minW);
			textArea.translate(10, 0);
			j = Justification::centredLeft;
		}
		textArea.translate(0, - 28);
		String text(periodPackets);
		text += " pair";
		if (periodPackets != 1)
		{
			text += "s";
		}
		if (PTPInfo::REPEAT == mode)
		{
			int repeatCount = (int)tree[Identifiers::PTPNumFaultInjectionCycles];
			text += " / " + String(repeatCount) + "x";
		}
		g.drawText(text, textArea,j, false);
	}
}

void PTPFaultInjectionComponent::PacketPairDisplay::drawBlocks( Graphics &g, juce::Rectangle<int> &area, int const count, Colour const color )
{
	int blockW = 10;
	int blockSpace = 2;
	float blockStep = float(blockW + blockSpace);
	int totalBlockW = (blockW + blockSpace) * count;
	juce::Rectangle<float> r(area.toFloat());

	r.setWidth((float)blockW);
	if (totalBlockW <= area.getWidth())
	{
		area.setWidth(totalBlockW);

		g.setColour(color);
		for (int i = 0; i < count; ++i)
		{
			g.fillEllipse(r);
			r.translate(blockStep, 0);
		}
	}
	else
	{
		int smallBlockW = 4;
		int bigCount = (area.getWidth() - (smallBlockW + blockSpace) * 3) / (blockW + blockSpace);

		g.setColour(color);
		for (int i = 0; i < bigCount/2; ++i)
		{
			g.fillEllipse(r);
			r.translate(blockStep, 0);
		}

		g.setColour(Colours::grey);
		juce::Rectangle<float> r2(r.getX(), r.getY() + (r.getHeight() - smallBlockW) * 0.5f, float(smallBlockW), float(smallBlockW));
		float smallBlockStep = float(smallBlockW + blockSpace);
		for (int i = 0; i < 3; ++i)
		{
			g.fillEllipse(r2);
			r2.translate(smallBlockStep, 0);
		}

		g.setColour(color);
		r.setX(r2.getX());
		for (int i = 0; i < bigCount/2; ++i)
		{
			g.fillEllipse(r);
			r.translate(blockStep, 0);
		}

		area.setWidth( bigCount * blockW + blockSpace * (bigCount - 1) + 3 * (smallBlockW + blockSpace));
	}
}