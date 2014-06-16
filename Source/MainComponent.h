/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "base.h"
#include "WorkbenchClient.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"
#include "StaticStreamViewport.h"
#include "WorkbenchComponent.h"
#include "AudioPatchbayComponent.h"

class MainContentComponent   : public Component, 
	public TextEditor::Listener
{
public:
    //==============================================================================
    MainContentComponent(WorkbenchClient * client_, AudioPatchbayClient* patchbyClient_, Settings *settings_);
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();
	void buttonClicked (Button* buttonThatWasClicked);
	String getAddress();

private:
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)

	void updateAddress();

	virtual void textEditorTextChanged( TextEditor& );
	virtual void textEditorReturnKeyPressed( TextEditor& );
	virtual void textEditorEscapeKeyPressed( TextEditor& );
	virtual void textEditorFocusLost( TextEditor& );

	WorkbenchClient* client;
	AudioPatchbayClient* patchbayClient;
	Settings *settings;

	ScopedPointer<TextEditor> addressEditor;
	ScopedPointer<Label> addressLabel;

	ScopedPointer<TabbedComponent> tabs;
	WorkbenchComponent *workbenchTab;
	AudioPatchbayComponent *audioPatchbayTab;

	enum 
	{
		WORKBENCH_TAB,
		PATCHBAY_TAB
	};
};


#endif  // MAINCOMPONENT_H_INCLUDED
