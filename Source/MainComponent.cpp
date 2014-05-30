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
	
	addAndMakeVisible (portEditor = new TextEditor ("portEditor"));
	portEditor->setMultiLine (false);
	portEditor->setReturnKeyStartsNewLine (false);
	portEditor->setReadOnly (false);
	portEditor->setScrollbarsShown (true);
	portEditor->setCaretVisible (true);
	portEditor->setPopupMenuEnabled (true);
	portEditor->setColour(TextEditor::highlightColourId,  Colours::orange);
	portEditor->setColour(TextEditor::focusedOutlineColourId,  Colours::orange);
	portEditor->setInputRestrictions(5, "0123456789");
	portEditor->addListener(this);

	addressEditor->setText(settings->getAddress().toString());
	portEditor->setText(String(settings->getPort()));

	addAndMakeVisible (addressLabel = new Label ("address label",
		TRANS("Address")));
	addressLabel->setFont (Font (15.00f, Font::plain));
	addressLabel->setJustificationType (Justification::centredRight);
	addressLabel->setEditable (false, false, false);
	addressLabel->setColour (TextEditor::textColourId, Colours::black);
	addressLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	addAndMakeVisible (portLabel = new Label ("port label",
		TRANS("Port")));
	portLabel->setFont (Font (15.00f, Font::plain));
	portLabel->setJustificationType (Justification::centredRight);
	portLabel->setEditable (false, false, false);
	portLabel->setColour (TextEditor::textColourId, Colours::black);
	portLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

	tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(tabs);

	setSize (600, 400);

	workbenchTab = new WorkbenchComponent(this, client, settings);
	tabs->addTab("Workbench", Colours::white, workbenchTab, true, WORKBENCH_TAB);

}

MainContentComponent::~MainContentComponent()
{
	tabs->removeTab(WORKBENCH_TAB);
	workbenchTab = nullptr;
	tabs = nullptr;
	addressEditor = nullptr;
	portEditor = nullptr;
	addressLabel = nullptr;
	portLabel = nullptr;
}

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);

}

void MainContentComponent::resized()
{
	addressLabel->setBounds (10, 20, 60, 24);
	addressEditor->setBounds (addressLabel->getRight() + 5, 20, 110, 24);
	portLabel->setBounds (addressEditor->getRight() + 5, 20, 40, 24);
	portEditor->setBounds (portLabel->getRight() + 5, 20, 50, 24);

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
	if (&edit == portEditor)
	{
		updatePort();
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
	if (&edit == portEditor)
	{
		portEditor->setText(String(settings->getPort()));
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

	if (&edit == portEditor)
	{
		updatePort();
		return;
	}
}

void MainContentComponent::updateAddress()
{
	IPAddress address(addressEditor->getText());
	addressEditor->setText(address.toString());
	settings->storeAddress(address);
}

void MainContentComponent::updatePort()
{
	int port = portEditor->getText().getIntValue();
	port = jlimit(0, 65535, port);
	portEditor->setText(String(port));
	settings->storePort(port);

}

String MainContentComponent::getAddress()
{
	return addressEditor->getText();
}
