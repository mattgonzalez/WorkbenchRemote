/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "WorkbenchClient.h"
#include "Settings.h"
#include "Identifiers.h"
#include "Strings.h"
#include "SettingsComponent.h"
#include "PacketSync.h"
#include "PTP.h"
#include "FaultInjection.h"

/*

This class uses the JUCE InterprocessConnection class to send and receive data
over a socket.  The actual data sent over the socket consists of a four byte magic number,
followed by a four byte little-endian message length count in bytes, followed by the JSON string.

For example, to transmit this 45 byte JSON string:

{"GetCommand": {"System": {}}, "Sequence": 1}

the following bytes are sent over the socket:

574f524b			Magic number 'WORK'
2d000000			Byte count (32 bits, endian swapped)
7b22476574436f6d	{"GetCom
6d616e64223a207b	mand": {
2253797374656d22	"System"
3a207b7d7d2c2022	: {}}, "
53657175656e6365	Sequence
223a20317d			": 1}

Strings are sent without a zero terminator.

*/

const String WorkbenchClient::followerString("Follower");
const String WorkbenchClient::grandmasterString("Grandmaster");
const String WorkbenchClient::BMCAString("BMCA");
const String WorkbenchClient::ethernetString("Ethernet");
const String WorkbenchClient::masterString("Master");
const String WorkbenchClient::slaveString("Slave");
const String WorkbenchClient::lockedString("Locked");
const String WorkbenchClient::unlockedString("Unlocked");
const String WorkbenchClient::onceString("Once");
const String WorkbenchClient::repeatString("Repeat");
const String WorkbenchClient::repeatContinuouslyString("Repeat Continuously");

//============================================================================
//
// Constructor and destructor
//
//============================================================================

//
// The 'KROW' value is the magic number written to the start of the packet
// that has to match at both ends of the connection
//
WorkbenchClient::WorkbenchClient(Settings* settings_):
	RemoteClient(settings_,'KROW'),
	workbenchSettingsTree(settings_->getWorkbenchSettingsTree()),
	ptpTree(settings_->getPTPTree())
{
	//DBG("WorkbenchClient::WorkbenchClient()");
	workbenchSettingsTree.addListener(this);
	ptpTree.addListener(this);
}

WorkbenchClient::~WorkbenchClient()
{
	//DBG("WorkbenchClient::~WorkbenchClient()");
}

//============================================================================
//
// Callbacks
//
//============================================================================

//---------------------------------------------------------------------------
//
// Callback indicating that the socket successfully connected
//
void WorkbenchClient::connectionMade()
{
	DBG("WorkbenchClient::connectionMade");

	RemoteClient::connectionMade();
}

//---------------------------------------------------------------------------
//
// Callback indicating that the socket has disconnected
//
void WorkbenchClient::connectionLost()
{
	DBG("WorkbenchClient::connectionLost");

	// Remove all the stream information from the settings tree
	settings->removeStreams();

	RemoteClient::connectionLost();
}

//============================================================================
//
// Commands sent to Workbench
//
//============================================================================

Result WorkbenchClient::getTalkerStreams()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree talkersTree(settings->getWorkbenchTree().getChildWithName(Identifiers::Talkers));

	arrayVar.resize(0); // force the var to become a zero-length array

	for (int i = 0; i < talkersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject);
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Talkers, arrayVar);
}

Result WorkbenchClient::getListenerStreams()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree listenersTree(settings->getWorkbenchTree().getChildWithName(Identifiers::Listeners));

	arrayVar.resize(0); // force the var to become a zero-length array

	for (int i = 0; i < listenersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject);
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Listeners, arrayVar);
}

Result WorkbenchClient::getLinkState()
{
	return getProperty(Identifiers::LinkState, new DynamicObject);
}

Result WorkbenchClient::getSettings()
{
	return getProperty(Identifiers::WorkbenchSettings, new DynamicObject);
}

Result WorkbenchClient::getPTP()
{
	return getProperty(Identifiers::PTP, new DynamicObject);
}

Result WorkbenchClient::setStreamProperty( Identifier const type, int const streamIndex, Identifier const &ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);
	var arrayVar;
	DynamicObject::Ptr streamObject(new DynamicObject);

	arrayVar.resize(0); // force the var to become a zero-length array

	streamObject->setProperty(Identifiers::Index, streamIndex);
	streamObject->setProperty(ID, parameter);
	arrayVar.append(var(streamObject));
	commandObject->setProperty(type, arrayVar);
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

//============================================================================
//
// Handle responses from Workbench
//
//============================================================================

void WorkbenchClient::handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage)
{
	var propertyVar(messageObject->getProperty(expectedMessage));
	DynamicObject * propertyObject = propertyVar.getDynamicObject();

	if (nullptr == propertyObject)
	{
		DBG("propertyObject is null");
		return;
	}

	if (propertyObject->hasProperty(Identifiers::System))
	{
		var systemPropertyVar(propertyObject->getProperty(Identifiers::System));
		DynamicObject *systemPropertyObject = systemPropertyVar.getDynamicObject();

		if (nullptr == systemPropertyObject)
		{
			DBG("Could not parse get system response");
			return;
		}

		handleGetSystemResponse(systemPropertyObject);
		return;
	}

	if (propertyObject->hasProperty(Identifiers::Talkers))
	{
		var talkersPropertyVar(propertyObject->getProperty(Identifiers::Talkers));
		if (false == talkersPropertyVar.isArray())
		{
			DBG("Could not parse get talkers response");
			return;
		}
		handleGetStreamsResponse(talkersPropertyVar, settings->getWorkbenchTree().getChildWithName(Identifiers::Talkers));
		return;
	}

	if (propertyObject->hasProperty(Identifiers::Listeners))
	{
		var listenersPropertyVar(propertyObject->getProperty(Identifiers::Listeners));
		if (false == listenersPropertyVar.isArray())
		{
			DBG("Could not parse get listeners response");
			return;
		}
		handleGetStreamsResponse(listenersPropertyVar, settings->getWorkbenchTree().getChildWithName(Identifiers::Listeners));
		return;
	}

	if (propertyObject->hasProperty(Identifiers::LinkState))
	{
		var linkStatePropertyVar(propertyObject->getProperty(Identifiers::LinkState));
		DynamicObject* linkStatePropertyObject = linkStatePropertyVar.getDynamicObject();
		if (nullptr == linkStatePropertyObject)
		{
			DBG("Could not parse get linkstate response");
			return;
		}
		handleGetLinkStateResponse(linkStatePropertyObject);
	}

	if (propertyObject->hasProperty(Identifiers::WorkbenchSettings))
	{
		var workbenchSettingsPropertyVar(propertyObject->getProperty(Identifiers::WorkbenchSettings));
		DynamicObject* workbenchSettingsPropertyObject = workbenchSettingsPropertyVar.getDynamicObject();
		if (nullptr == workbenchSettingsPropertyObject)
		{
			DBG("Could not parse get settings response");
		}
		handleGetWorkbenchSettingsResponse(workbenchSettingsPropertyObject);
		return;
	}

	if (propertyObject->hasProperty(Identifiers::PTP))
	{
		var ptpPropertyVar(propertyObject->getProperty(Identifiers::PTP));
		DynamicObject* ptpPropertyObject = ptpPropertyVar.getDynamicObject();
		if (nullptr == ptpPropertyObject)
		{
			DBG("Could not parse get PTP response");
		}
		handlePTPObject(ptpPropertyObject);
		return;
	}
}

void WorkbenchClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{
	int numTalkers = systemPropertyObject->getProperty(Identifiers::Talkers);
	int numListeners = systemPropertyObject->getProperty(Identifiers::Listeners);

	if (systemPropertyObject->hasProperty(Identifiers::BroadRReachSupported))
	{
		workbenchSettingsTree.setProperty(Identifiers::BroadRReachSupported, systemPropertyObject->getProperty(Identifiers::BroadRReachSupported), nullptr);
	}

	settings->initializeStreams(numTalkers, numListeners);
}

void WorkbenchClient::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (treeWhosePropertyHasChanged.getType() == Identifiers::WorkbenchSettings)
	{
		setRemoteProperty(Identifiers::WorkbenchSettings, createSettingsObject(property, treeWhosePropertyHasChanged[property]));
		return;
	}

	if (treeWhosePropertyHasChanged.getType() == Identifiers::PTP)
	{
		setRemoteProperty(Identifiers::PTP, createPTPObject(property, treeWhosePropertyHasChanged[property]));
		return;
	}

	if (treeWhosePropertyHasChanged.getType() == Identifiers::FaultInjection &&
		treeWhosePropertyHasChanged.getParent().getType() == Identifiers::PTP)
	{
		setRemotePTPFaultInjectionProperty(property);
		return;
	}

	if (treeWhosePropertyHasChanged.getParent().getType() == Identifiers::CorruptPackets &&
		treeWhosePropertyHasChanged.getParent().getParent().getParent().getParent().getType() == Identifiers::PTP)
	{
		setRemotePTPCorruptPacketField(treeWhosePropertyHasChanged);
		return;
	}
}


Result WorkbenchClient::setRemoteProperty(Identifier const &commandProperty, DynamicObject* propertyObject)
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);

	commandObject->setProperty(commandProperty, var(propertyObject));
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

