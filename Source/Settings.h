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

	ScopedPointer<PropertiesFile> propfile;

	void initializeStreams(int numTalkers,int numListeners);
	void initializeAudioDevices(int numAudioDevices);
	void initializeAudioDevice(int deviceIndex, int numInputs,int numOutputs);
	void removeStreams();
	void removeAudioDevices();
	CriticalSection lock;

protected:
	ValueTree tree;
};

extern void dumpTree(ValueTree const &tree, const int depth = 0);