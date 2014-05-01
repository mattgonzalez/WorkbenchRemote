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
	void storePort( int port );
	IPAddress getAddress();

	int getPort();
	ScopedPointer<PropertiesFile> propfile;

	void initializeStreams(int numTalkers,int numListeners);
	void removeStreams();

	CriticalSection lock;
	ValueTree tree;
};
