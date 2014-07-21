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
	RemoteClient(settings_,'KROW')
{
	//DBG("WorkbenchClient::WorkbenchClient()");
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
	ValueTree talkersTree(settings->getStreamsTree().getChildWithName(Identifiers::Talkers));

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
	ValueTree listenersTree(settings->getStreamsTree().getChildWithName(Identifiers::Listeners));

	arrayVar.resize(0); // force the var to become a zero-length array

	for (int i = 0; i < listenersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject);
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Listeners, arrayVar);
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
		DBG("Could not parse get system response");
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
		handleGetStreamsResponse(talkersPropertyVar, settings->getStreamsTree().getChildWithName(Identifiers::Talkers));
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
		handleGetStreamsResponse(listenersPropertyVar, settings->getStreamsTree().getChildWithName(Identifiers::Listeners));
		return;
	}
}

void WorkbenchClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{
	int numTalkers = systemPropertyObject->getProperty(Identifiers::Talkers);
	int numListeners = systemPropertyObject->getProperty(Identifiers::Listeners);

	settings->initializeStreams(numTalkers, numListeners);
}

void WorkbenchClient::handleGetStreamsResponse( var streamsPropertyVar, ValueTree streamsTree )
{
	ScopedLock locker(settings->lock);

	for (int responseIndex = 0; responseIndex < streamsPropertyVar.size(); ++responseIndex)
	{
		//
		// Get the stream index from the response...
		//
		var const &v(streamsPropertyVar[responseIndex]);
		DynamicObject::Ptr const d(v.getDynamicObject());

		if (nullptr == d || false == d->hasProperty(Identifiers::Index))
		{
			DBG("Invalid response for get talkers");
			continue;
		}

		int streamIndex = d->getProperty(Identifiers::Index);
		//DBG(streamIndex);

		//
		// Get the values for the stream from the response and put them into the tree
		//
		ValueTree streamTree(streamsTree.getChild(streamIndex));
		if (false == streamTree.isValid())
		{
			DBG("Invalid stream index for get talkers");
			continue;
		}

		if (d->hasProperty(Identifiers::Name))
		{
			streamTree.setProperty(Identifiers::Name, d->getProperty(Identifiers::Name), nullptr);
		}

		if (d->hasProperty(Identifiers::StreamID))
		{
			int64 streamID = d->getProperty(Identifiers::StreamID).toString().getHexValue64();
			streamTree.setProperty(Identifiers::StreamID, streamID, nullptr);
		}

		if (d->hasProperty(Identifiers::DestinationAddress))
		{
			String addressString(d->getProperty(Identifiers::DestinationAddress).toString());
			addressString = addressString.toLowerCase();
			addressString.retainCharacters(Strings::hexChars);
			streamTree.setProperty(Identifiers::DestinationAddress, addressString.getHexValue64(), nullptr);
		}

		if (d->hasProperty(Identifiers::Subtype))
		{
			streamTree.setProperty(Identifiers::Subtype, (int) d->getProperty(Identifiers::Subtype), nullptr);
		}

		if (d->hasProperty(Identifiers::ChannelCount))
		{
			streamTree.setProperty(Identifiers::ChannelCount, (int) d->getProperty(Identifiers::ChannelCount), nullptr);
		}

		if (d->hasProperty(Identifiers::Active))
		{
			streamTree.setProperty(Identifiers::Active, d->getProperty(Identifiers::Active), nullptr);
		}

		if (d->hasProperty(Identifiers::FaultInjection) && Identifiers::Talkers == streamsTree.getType())
		{
			ValueTree faultTree(streamTree.getChildWithName(Identifiers::FaultInjection));
			var const& faultVar(d->getProperty(Identifiers::FaultInjection));
			DynamicObject::Ptr const faultObject(faultVar.getDynamicObject());

			if (faultObject->hasProperty(Identifiers::CorruptPackets))
			{
				ValueTree corruptTree(faultTree.getChildWithName(Identifiers::CorruptPackets));
				var const& corruptVar(faultObject->getProperty(Identifiers::CorruptPackets));
				DynamicObject::Ptr const corruptObject(corruptVar.getDynamicObject());
				if (corruptObject->hasProperty(Identifiers::Enabled))
				{
					corruptTree.setProperty(Identifiers::Enabled, corruptObject->getProperty(Identifiers::Enabled), nullptr);
				}

				if (corruptObject->hasProperty(Identifiers::Percent))
				{
					corruptTree.setProperty(Identifiers::Percent, corruptObject->getProperty(Identifiers::Percent), nullptr);
				}
			}
		}
	}
}

void WorkbenchClient::handleFaultNotificationMessage( DynamicObject * messageObject )
{
}