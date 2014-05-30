/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "Settings.h"
#include "Identifiers.h"

const String addressKey ("Address");
const String portKey ("Port");

Settings::Settings() :
	tree("Settings")
{
	PropertiesFile::Options options;
	options.applicationName     = "WorkbenchRemote";
	options.folderName          = "EchoAVB";
	options.filenameSuffix      = "xml";
	options.osxLibrarySubFolder = "Application Support";
	propfile = new PropertiesFile (options);
}

Settings::~Settings()
{

}

void Settings::storeAddress( IPAddress address )
{
	propfile->setValue(addressKey, address.toString());

}
void Settings::storePort( int port )
{
	propfile->setValue(portKey, port);

}

int Settings::getPort()
{
	return propfile->getIntValue(portKey, 0xecc0);
}

IPAddress Settings::getAddress()
{
	return IPAddress(propfile->getValue(addressKey, "127.0.0.1"));
}

void Settings::initializeStreams( int numTalkers,int numListeners )
{
	ScopedLock locker(lock);

	ValueTree talkersTree(tree.getOrCreateChildWithName(Identifiers::Talkers, nullptr));
	talkersTree.removeAllChildren(nullptr);
	for (int i = 0; i < numTalkers; ++i)
	{
		ValueTree child("Talker" + String(i));
		child.setProperty(Identifiers::Index, i, nullptr);
		ValueTree fault(child.getOrCreateChildWithName(Identifiers::FaultInjection, nullptr));
		ValueTree corrupt(fault.getOrCreateChildWithName(Identifiers::CorruptPackets, nullptr));
		corrupt.setProperty(Identifiers::Enabled, false, nullptr);
		corrupt.setProperty(Identifiers::Percent, 0.0f, nullptr);
		talkersTree.addChild(child,-1,nullptr);
	}

	ValueTree listenersTree(tree.getOrCreateChildWithName(Identifiers::Listeners, nullptr));
	listenersTree.removeAllChildren(nullptr);
	for (int i = 0; i < numListeners; ++i)
	{
		ValueTree child("Listener" + String(i));
		child.setProperty(Identifiers::Index, i, nullptr);
		listenersTree.addChild(child,-1,nullptr);
	}
}

void Settings::removeStreams()
{
	ScopedLock locker(lock);

	ValueTree talkersTree(tree.getChildWithName(Identifiers::Talkers));
	tree.removeChild(talkersTree,nullptr);
	ValueTree listenersTree(tree.getChildWithName(Identifiers::Listeners));
	tree.removeChild(listenersTree,nullptr);
}
