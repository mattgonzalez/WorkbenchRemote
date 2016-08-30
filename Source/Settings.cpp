/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "Settings.h"
#include "Identifiers.h"
#include "SettingsComponent.h"

const String addressKey ("Address");
const String portKey ("Port");

void dumpTree(ValueTree const &tree, const int depth)
{
	Identifier const treeId(tree.getType());
	int index;
	int const num_props = tree.getNumProperties();
	int const num_children = tree.getNumChildren();
	String indent(String::repeatedString("   ", depth));

	DBG(String::empty);
	if (1 == depth)
	{
		DBG("==========================================================================");
	}

	DBG(indent + treeId.toString());

	if (num_props)
	{
		DBG(indent + String::formatted("  properties:%d", num_props));
		for (index = 0; index < num_props; index++)
		{
			Identifier id(tree.getPropertyName(index));
			var prop(tree[id]);

			if (prop.isArray())
			{
				Array<var> *array = prop.getArray();

				DBG(indent + indent + id.toString() + " (array)");
				for (int array_index = 0; array_index < array->size(); array_index++)
				{
					DBG(indent + indent + id.toString() + "[" + String(array_index) + "]: " + (*array)[array_index].toString());
				}
			}
			else if (prop.isBinaryData())
			{
				MemoryBlock* block = prop.getBinaryData();
				DBG(indent + indent + id.toString() + String("  ") + String::toHexString(block->getData(), (int)block->getSize(), 1));
			}
			else
			{
				DBG(indent + indent + id.toString() + String("  ") + tree[id].toString());
			}
		}
	}

	if (num_children)
	{
		DBG(indent + String::formatted("  children:%d", num_children));
		for (index = 0; index < num_children; index++)
		{
			ValueTree const child(tree.getChild(index));
			dumpTree(child, depth + 1);
		}
	}
}

Settings::Settings() :
	tree("Settings")
{
	PropertiesFile::Options options;
	options.applicationName     = "WorkbenchRemote";
	options.folderName          = "EchoAVB";
	options.filenameSuffix      = "xml";
	options.osxLibrarySubFolder = "Application Support";

	propfile = new PropertiesFile (options);

	//
	// Workbench default Valuetree 
	//
	{
		ValueTree workbenchTree (Identifiers::Workbench);
		tree.addChild(workbenchTree,-1,nullptr);
		String key (workbenchTree.getType().toString() + portKey);
		workbenchTree.setProperty(Identifiers::Port, propfile->getIntValue(key,0xecc0), nullptr);
	}

	//
	// Audio Patchbay default Valuetree 
	//
	{
		ValueTree audioDevicesTree (Identifiers::AudioDevices);
		tree.addChild(audioDevicesTree,-1,nullptr);
		String key (audioDevicesTree.getType().toString() + portKey);
		audioDevicesTree.setProperty(Identifiers::Port, propfile->getIntValue(key,0xecc1), nullptr);
	}

	//
	// Linkstate default Valuetree 
	//
	{
		ValueTree linkStateTree(Identifiers::LinkState);
		getWorkbenchTree().addChild(linkStateTree, -1, nullptr);
	}

	//
	// Workbench settings default Valuetree 
	//
	{
		ValueTree workbenchSettingsTree(Identifiers::WorkbenchSettings);
		workbenchSettingsTree.setProperty(Identifiers::StaticPTPRole, SettingsComponent::CONFIG_FOLLOWER, nullptr);
		workbenchSettingsTree.setProperty(Identifiers::PTPDelayRequest, SettingsComponent::MIN_DELAY_REQUEST_INTERVAL_MILLISECONDS, nullptr);
		workbenchSettingsTree.setProperty(Identifiers::EthernetMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
		workbenchSettingsTree.setProperty(Identifiers::BroadRReachSupported, false, nullptr);
		getWorkbenchTree().addChild(workbenchSettingsTree, -1, nullptr);
	}

	//
	// PTP info default Valuetree 
	//
	{
		//fixme Commented out so it still compiles; needs to be modified from Workbench version.
		
		ValueTree ptpInfoTree(Identifiers::PTPInfo);
		ptpInfoTree.setProperty(Identifiers::StaticPTPRole, SettingsComponent::CONFIG_FOLLOWER, nullptr);
		
		ValueTree PTPFaultTree(ptpInfoTree.getOrCreateChildWithName(Identifiers::FaultInjection, nullptr));
		PTPFaultTree.setProperty(Identifiers::Enabled, false, nullptr);
		PTPFaultTree.setProperty(Identifiers::PTPFaultInjectionCycleMode, ONCE, nullptr);
		PTPFaultTree.setProperty(Identifiers::PTPNumBadSyncFollowupPairsPerCycle, 0, nullptr);
		PTPFaultTree.setProperty(Identifiers::PTPFaultInjectionCycleLengthPackets, 1, nullptr);
		PTPFaultTree.setProperty(Identifiers::PTPNumFaultInjectionCycles, 1, nullptr);

		getWorkbenchTree().addChild(ptpInfoTree, -1, nullptr);
		//
		// Sync packet corruption
		//
		/*
		{
			ValueTree syncFaultTree(PTPFaultTree.getOrCreateChildWithName(Identifiers::Sync, nullptr));
			ValueTree syncCorruptionTree(syncFaultTree.getOrCreateChildWithName(Identifiers::CorruptPackets, nullptr));
			for (int fieldIndex = 0; fieldIndex < PacketSync::NUM_FIELDS; ++fieldIndex)
			{
				PacketPTP::Field field;
				field.index = fieldIndex;
				PacketSync::getField(field, Guid64(int64(0)));

				ValueTree fieldTree(syncCorruptionTree.getOrCreateChildWithName(field.identifier, nullptr));
				fieldTree.setProperty(Identifiers::Enabled, false, nullptr);

				if (PacketPTP::sourcePortIdentity == fieldIndex)
				{
					// do this one later once the adapter MAC address is loaded
					continue;
				}

				var v(field.correctValueBigEndian);
				fieldTree.setProperty(Identifiers::Value, v, nullptr);
			}
		}

		//
		// Followup packet corruption
		//
	{
		ValueTree followupFaultTree(PTPFaultTree.getOrCreateChildWithName(Identifiers::Followup, nullptr));
		ValueTree followupCorruptionTree(followupFaultTree.getOrCreateChildWithName(Identifiers::CorruptPackets, nullptr));
		for (int fieldIndex = 0; fieldIndex < PacketFollowUp::NUM_FIELDS; ++fieldIndex)
		{
			PacketPTP::Field field;
			field.index = fieldIndex;
			PacketFollowUp::getField(field, Guid64(int64(0)));

			ValueTree fieldTree(followupCorruptionTree.getOrCreateChildWithName(field.identifier, nullptr));
			fieldTree.setProperty(Identifiers::Enabled, false, nullptr);

			if (PacketPTP::sourcePortIdentity == fieldIndex)
			{
				// do this one later once the adapter MAC address is loaded
				continue;
			}

			var v(field.correctValueBigEndian);
			fieldTree.setProperty(Identifiers::Value, v, nullptr);
			
		}
	}
	*/
	}
	
	dumpTree(tree);
}

Settings::~Settings()
{
	{
		ValueTree workbenchTree (getWorkbenchTree());
		String key (workbenchTree.getType().toString() + portKey);
		propfile->setValue(key, (int)workbenchTree[Identifiers::Port]);
	}

	{
		ValueTree audioDevicesTree (getAudioDevicesTree());
		String key (audioDevicesTree.getType().toString() + portKey);
		propfile->setValue(key, (int)audioDevicesTree[Identifiers::Port]);
	}
}

void Settings::storeAddress( IPAddress address )
{
	propfile->setValue(addressKey, address.toString());
}

IPAddress Settings::getAddress()
{
	return IPAddress(propfile->getValue(addressKey, "127.0.0.1"));
}

void Settings::initializeStreams( int numTalkers,int numListeners )
{
	ScopedLock locker(lock);
	ValueTree workbenchTree(tree.getChildWithName(Identifiers::Workbench));

	ValueTree talkersTree(workbenchTree.getOrCreateChildWithName(Identifiers::Talkers, nullptr));
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

	ValueTree listenersTree(workbenchTree.getOrCreateChildWithName(Identifiers::Listeners, nullptr));
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

	ValueTree workbenchTree(tree.getChildWithName(Identifiers::Workbench));
	ValueTree talkersTree(workbenchTree.getChildWithName(Identifiers::Talkers));
	workbenchTree.removeChild(talkersTree,nullptr);
	ValueTree listenersTree(workbenchTree.getChildWithName(Identifiers::Listeners));
	workbenchTree.removeChild(listenersTree,nullptr);
	ValueTree linkStateTree(workbenchTree.getChildWithName(Identifiers::LinkState));
	linkStateTree.removeAllProperties(nullptr);
}

void Settings::initializeAudioDevices( int numAudioDevices )
{
	ScopedLock locker(lock);

	ValueTree audioDevicesTree(tree.getOrCreateChildWithName(Identifiers::AudioDevices, nullptr));
	audioDevicesTree.removeAllChildren(nullptr);
	for (int i = 0; i < numAudioDevices; ++i)
	{
		ValueTree child("AudioDevice" + String(i));
		child.setProperty(Identifiers::Index, i, nullptr);
		ValueTree inputTree(child.getOrCreateChildWithName(Identifiers::Input, nullptr));
		ValueTree outputTree(child.getOrCreateChildWithName(Identifiers::Output, nullptr));
		child.setProperty(Identifiers::Index, i, nullptr);
		audioDevicesTree.addChild(child,-1,nullptr);
	}
}

void Settings::removeAudioDevices()
{
	ScopedLock locker(lock);

	ValueTree audioDevicesTree(tree.getChildWithName(Identifiers::AudioDevices));
	audioDevicesTree.removeAllChildren(nullptr);
	audioDevicesTree.removeAllProperties(nullptr);
}

ValueTree Settings::getAudioDevicesTree()
{
	return tree.getChildWithName(Identifiers::AudioDevices);
}

ValueTree Settings::getLinkStateTree()
{
	return getWorkbenchTree().getChildWithName(Identifiers::LinkState);
}
ValueTree Settings::getWorkbenchTree()
{
	return tree.getChildWithName(Identifiers::Workbench);
}

ValueTree Settings::getWorkbenchSettingsTree()
{
	return getWorkbenchTree().getChildWithName(Identifiers::WorkbenchSettings);
}

static void createChannels (ValueTree &  parent,int const numChannels)
{
	int channel = 0;

	parent.removeAllChildren(nullptr);
	while (channel < numChannels)
	{
		String channelString(channel);
		ValueTree channelTree("Channel_" + channelString);
		channelTree.setProperty(Identifiers::Name, "Channel " + channelString, nullptr);
		channelTree.setProperty(Identifiers::Channel, channel, nullptr);
		parent.addChild(channelTree, -1, nullptr);
		++channel;
	}

	parent.setProperty(Identifiers::MaxChannelCount, numChannels, nullptr);
	parent.setProperty(Identifiers::CurrentChannelCount, numChannels, nullptr);
}

void Settings::initializeAudioDevice( int deviceIndex, int numInputs, int numOutputs )
{
	ScopedLock locker(lock);

	ValueTree audioDevicesTree(tree.getChildWithName(Identifiers::AudioDevices));
	ValueTree deviceTree (audioDevicesTree.getChild(deviceIndex));

	ValueTree inputTree(deviceTree.getChildWithName(Identifiers::Input));
	createChannels(inputTree,numInputs);
	ValueTree outputTree(deviceTree.getChildWithName(Identifiers::Output));
	createChannels(outputTree,numOutputs);
}