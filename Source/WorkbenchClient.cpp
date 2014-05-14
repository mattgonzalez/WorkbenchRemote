/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "WorkbenchClient.h"
#include "Settings.h"
#include "Identifiers.h"

const String Receive("Receive");
const String Send("Send");
const String HexChars("0123456789abcdef");
const String OK("OK");

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
	InterprocessConnection(true, 'KROW'),
	settings(settings_),
	commandSequence(0)
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

	// Generate a change notification indicating that the state of the socket has changed
	changeBroadcaster.sendChangeMessage();
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

	// Generate a change notification indicating that the state of the socket has changed
	changeBroadcaster.sendChangeMessage();
}

//---------------------------------------------------------------------------
//
// Callback indicating that the socket has received a message
//
void WorkbenchClient::messageReceived( const MemoryBlock& message )
{
	ScopedLock locker(lock);

	DBG("WorkbenchClient::messageReceived  bytes:" << (int)message.getSize());
	
	// The MemoryBlock parameter is just the JSON string without the magic number or byte count
	// Parse the MemoryBlock as a JSON String
	var messageVar;
	Result parseResult(JSON::parse(message.toString(), messageVar));
	if (parseResult.failed())
	{
		DBG("parse failed " + parseResult.getErrorMessage());
		return;
	}

	// Send the JSON string to the user interface
	stringBroadcaster.sendActionMessage(Receive + message.toString());

	// Start reading properties from the JSON data
	DynamicObject * messageObject = messageVar.getDynamicObject();
	if (nullptr == messageObject)
	{
		DBG ("Could not convert JSON var to DynamicObject");
		return;
	}

	// Each message must have a sequence number
	if (false == messageObject->hasProperty(Identifiers::Sequence))
	{
		DBG ("Message has no sequence number");
		return;
	}

	// The received sequence number should match the transmitted sequence number
	int responseSequence = messageObject->getProperty(Identifiers::Sequence);
	int expectedSequence = commandSequence - 1;
	if (responseSequence != expectedSequence)
	{
		DBG ("Unexpected sequence number " << responseSequence << "; expected " << expectedSequence);
		return;
	}

	// The received message should have a Status string
	if (false == messageObject->hasProperty(Identifiers::Status))
	{
		DBG ("Missing status");
		return;
	}

	// The Status string should be "OK"
	String status(messageObject->getProperty(Identifiers::Status).toString());
	if (status != OK)
	{
		DBG ("Error response: " + status);
		return;
	}

	// See if this is a GetResponse message
	if (messageObject->hasProperty(Identifiers::GetResponse))
	{
		handleGetResponse(messageObject);
		return;
	}
}


//============================================================================
//
// Commands sent to Workbench
//
//============================================================================

Result WorkbenchClient::getSystemInfo()
{
	return getProperty(Identifiers::System, new DynamicObject());
}

Result WorkbenchClient::getTalkerStreams()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree talkersTree(settings->tree.getChildWithName(Identifiers::Talkers));
	for (int i = 0; i < talkersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject());
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Talkers, arrayVar);
}


Result WorkbenchClient::getListenerStreams()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree listenersTree(settings->tree.getChildWithName(Identifiers::Listeners));
	for (int i = 0; i < listenersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject());
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Listeners, arrayVar);
}

Result WorkbenchClient::setStreamProperty( Identifier const type, int const streamIndex, Identifier const &ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject());
	var arrayVar;
	DynamicObject::Ptr streamObject(new DynamicObject());

	streamObject->setProperty(Identifiers::Index, streamIndex);
	streamObject->setProperty(ID, parameter);
	arrayVar.append(var(streamObject));
	commandObject->setProperty(type, arrayVar);
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

Result WorkbenchClient::getProperty( Identifier const ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject());
	commandObject->setProperty(ID, parameter);
	messageObject.setProperty(Identifiers::GetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}


Result WorkbenchClient::sendJSONToSocket( DynamicObject &messageObject )
{
	// Convert the DynamicObject to a JSON string
	MemoryOutputStream stream;
	messageObject.writeAsJSON(stream, 0, true);

	// Update the user interface
	stringBroadcaster.sendActionMessage(Send + stream.toString());

	// Transmit the JSON string.  The sendMessage function
	// will prepend the data with the magic number and byte count.
	bool success = sendMessage(stream.getMemoryBlock());
	if (success)
		return Result::ok();

	return Result::fail("InterprocessConnection::sendMessage failed");
}

//============================================================================
//
// Handle responses from Workbench
//
//============================================================================

void WorkbenchClient::handleGetResponse( DynamicObject * messageObject )
{
	var propertyVar(messageObject->getProperty(Identifiers::GetResponse));
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
		handleGetTalkersResponse(talkersPropertyVar);
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
		handleGetListenersResponse(listenersPropertyVar);
		return;
	}
}

void WorkbenchClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{
	int numTalkers = systemPropertyObject->getProperty(Identifiers::Talkers);
	int numListeners = systemPropertyObject->getProperty(Identifiers::Listeners);

	settings->initializeStreams(numTalkers, numListeners);
}

void WorkbenchClient::handleGetTalkersResponse(var talkersPropertyVar )
{
	ScopedLock locker(settings->lock);
	ValueTree talkersTree(settings->tree.getChildWithName(Identifiers::Talkers));

	for (int responseIndex = 0; responseIndex < talkersPropertyVar.size(); ++responseIndex)
	{
		//
		// Get the stream index from the response...
		//
		var const &v(talkersPropertyVar[responseIndex]);
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
		ValueTree streamTree(talkersTree.getChild(streamIndex));
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
			addressString.retainCharacters(HexChars);
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
	}
}

void WorkbenchClient::handleGetListenersResponse( var listenersPropertyVar )
{
	ScopedLock locker(settings->lock);
	ValueTree listenersTree(settings->tree.getChildWithName(Identifiers::Listeners));

	for (int responseIndex = 0; responseIndex < listenersPropertyVar.size(); ++responseIndex)
	{
		//
		// Get the stream index from the response...
		//
		var const &v(listenersPropertyVar[responseIndex]);
		DynamicObject::Ptr const d(v.getDynamicObject());

		if (nullptr == d || false == d->hasProperty(Identifiers::Index))
		{
			DBG("Invalid response for get listeners");
			continue;
		}

		int streamIndex = d->getProperty(Identifiers::Index);
		//DBG(streamIndex);

		//
		// Get the values for the stream from the response and put them into the tree
		//
		ValueTree streamTree(listenersTree.getChild(streamIndex));
		if (false == streamTree.isValid())
		{
			DBG("Invalid stream index for get listeners");
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
			addressString.retainCharacters(HexChars);
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
	}
}
