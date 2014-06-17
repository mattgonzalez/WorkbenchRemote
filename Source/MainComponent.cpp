/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "MainComponent.h"
#include "Identifiers.h"

MainContentComponent::MainContentComponent(WorkbenchClient* client_, AudioPatchbayClient* patchbayClient_, Settings *settings_):
	client(client_),
	patchbayClient(patchbayClient_),
	settings(settings_),
	workbenchTab(nullptr)
	
{
	addAndMakeVisible (addressEditor = new TextEditor ("addressEditor"));
	addressEditor->setMultiLine (false);
	addressEditor->setReturnKeyStartsNewLine (false);
	addressEditor->setReadOnly (false);
	addressEditor->setScrollbarsShown (true);
	addressEditor->setCaretVisible (true);
	addressEditor->setPopupMenuEnabled (true);
	addressEditor->setText ("127.0.0.1");
	addressEditor->setColour(TextEditor::highlightColourId,  Colours::orange);
	addressEditor->setColour(TextEditor::focusedOutlineColourId,  Colours::orange);
	addressEditor->setInputRestrictions(15, "0123456789.");
	addressEditor->addListener(this);
	addressEditor->setText(settings->getAddress().toString());

	addAndMakeVisible (addressLabel = new Label ("address label",
		TRANS("Address")));
	addressLabel->setFont (Font (15.00f, Font::plain));
	addressLabel->setJustificationType (Justification::centredRight);
	addressLabel->setEditable (false, false, false);
	addressLabel->setColour (TextEditor::textColourId, Colours::black);
	addressLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	setSize (600, 400);

	workbenchTab = new WorkbenchComponent(this, client, settings);
	tabs->addTab("Workbench", Colours::white, workbenchTab, true, WORKBENCH_TAB);

	audioPatchbayTab = new AudioPatchbayComponent(this, patchbayClient, settings);
	tabs->addTab("Patchbay", Colours::white, audioPatchbayTab, true, PATCHBAY_TAB);
}

MainContentComponent::~MainContentComponent()
{
	tabs->removeTab(WORKBENCH_TAB);
	tabs->removeTab(PATCHBAY_TAB);
	workbenchTab = nullptr;
	audioPatchbayTab = nullptr;
	tabs = nullptr;
	addressEditor = nullptr;
	addressLabel = nullptr;
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);

}

void MainContentComponent::resized()
{
	addressLabel->setBounds (10, 20, 60, 24);
	addressEditor->setBounds (addressLabel->getRight() + 5, 20, 110, 24);

	int y = addressLabel->getY() + 30;
	int w = getWidth() - 20;
	int h = getHeight() - y - 10;
	tabs->setBounds(10, y, w, h);
}

void MainContentComponent::textEditorTextChanged( TextEditor& )
{
}

void MainContentComponent::textEditorReturnKeyPressed( TextEditor& edit)
{
	if (&edit == addressEditor)
	{
		updateAddress();
		return;
	}
	
}

void MainContentComponent::textEditorEscapeKeyPressed( TextEditor& edit)
{
	if (&edit == addressEditor)
	{
		addressEditor->setText(settings->getAddress().toString());
		return;
	}
	
}

void MainContentComponent::textEditorFocusLost( TextEditor& edit)
{
	if (&edit == addressEditor)
	{
		updateAddress();
		return;
	}

}

void MainContentComponent::updateAddress()
{
	IPAddress address(addressEditor->getText());
	addressEditor->setText(address.toString());
	settings->storeAddress(address);
}

String MainContentComponent::getAddress()
{
	return addressEditor->getText();
}