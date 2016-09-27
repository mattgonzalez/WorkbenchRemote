/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

class Settings
{
public:
	Settings();
	~Settings();

	void storeAddress(IPAddress address);
	IPAddress getAddress();
	ValueTree getWorkbenchTree();
	ValueTree getAudioDevicesTree();
	ValueTree getLinkStateTree();
	ValueTree getWorkbenchSettingsTree();
	ValueTree getPTPTree();
	ValueTree getAVTPTree();

	ScopedPointer<PropertiesFile> propfile;

	void initializeStreams(int numTalkers,int numListeners);
	void initializeAudioDevices(int numAudioDevices);
	void initializeAudioDevice(int deviceIndex, int numInputs,int numOutputs);
	void removeStreams();
	void removeAudioDevices();
	CriticalSection lock;

	ValueTree tree;

	//
	// Settings also used by Workbench
	//
	enum
	{
		ANALYZERBR_USB_ETHERNET_MODE_STANDARD = 1,
		ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER,
		ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE
	};
};

extern void dumpTree(ValueTree const &tree, const int depth = 0);