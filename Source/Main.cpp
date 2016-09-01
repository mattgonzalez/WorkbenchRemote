/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "MainComponent.h"
#include "Settings.h"

//==============================================================================
class WorkbenchRemoteApplication  : public JUCEApplication
{
public:
    //==============================================================================
    WorkbenchRemoteApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& /*commandLine*/) override
    {
		settings = new Settings();
		client = new WorkbenchClient(settings);
		patchbayClient = new AudioPatchbayClient(settings);
		lookAndFeelV3 = new LookAndFeel_V3;
		lookAndFeelV3->setColour(TextEditor::outlineColourId, Colours::lightgrey);
		LookAndFeel::setDefaultLookAndFeel(lookAndFeelV3);
		
        mainWindow = new MainWindow(client, patchbayClient, settings);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
		lookAndFeelV3 = nullptr;
		client = nullptr;
		patchbayClient = nullptr;
		settings = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const String& /*commandLine*/) override
    {
    }

    //==============================================================================

    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow(WorkbenchClient* client, AudioPatchbayClient* patchbayClient, Settings *settings)  : DocumentWindow ("Workbench Remote",
                                        Colours::lightgrey,
                                        DocumentWindow::allButtons)
        {
			setUsingNativeTitleBar(true);

            setContentOwned (new MainContentComponent(client, patchbayClient, settings), true);

            centreWithSize (getWidth(), getHeight());
            setVisible (true);

			setResizable(true,false);
			setResizeLimits(850, 700, INT_MAX, INT_MAX);
        }

        void closeButtonPressed()
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
	ScopedPointer<Settings> settings;
	ScopedPointer<WorkbenchClient> client;
	ScopedPointer<AudioPatchbayClient> patchbayClient;
	ScopedPointer<LookAndFeel_V3> lookAndFeelV3;
    ScopedPointer<MainWindow> mainWindow;

};

//==============================================================================

START_JUCE_APPLICATION (WorkbenchRemoteApplication)
