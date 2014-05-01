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

WorkbenchClient::WorkbenchClient(Settings* settings_):
	InterprocessConnection(true, 'KROW'),
	settings(settings_),
	commandSequence(0)
{
	DBG("WorkbenchClient::WorkbenchClient() ");
}

WorkbenchClient::~WorkbenchClient()
{
	DBG("WorkbenchClient::~WorkbenchClient() ");
}

void WorkbenchClient::connectionMade()
{
	DBG("WorkbenchClient::connectionMade");
	changeBroadcaster.sendChangeMessage();
}

void WorkbenchClient::connectionLost()
{
	DBG("WorkbenchClient::connectionLost");
	settings->removeStreams();
	changeBroadcaster.sendChangeMessage();
}

void WorkbenchClient::messageReceived( const MemoryBlock& message )
{
	ScopedLock locker(lock);
	DBG("WorkbenchClient::messageReceived  bytes:" << (int)message.getSize());

	var messageVar;
	Result parseResult(JSON::parse(message.toString(), messageVar));
	if (parseResult.failed())
	{
		DBG("parse failed " + parseResult.getErrorMessage());
		return;
	}

	stringBroadcaster.sendActionMessage(Receive + message.toString());

	DynamicObject * messageObject = messageVar.getDynamicObject();
	if (nullptr == messageObject)
	{
		DBG ("Could not convert JSON var to DynamicObject");
		return;
	}

	if (false == messageObject->hasProperty(Identifiers::Sequence))
	{
		DBG ("Message has no sequence number");
		return;
	}
	int responseSequence = messageObject->getProperty(Identifiers::Sequence);
	int expectedSequence = commandSequence - 1;
	if (responseSequence != expectedSequence)
	{
		DBG ("Unexpected sequence number " << responseSequence << "; expected " << expectedSequence);
		return;
	}

	if (false == messageObject->hasProperty(Identifiers::Status))
	{
		DBG ("Missing status");
		return;
	}

	String status(messageObject->getProperty(Identifiers::Status).toString());
	if (status != OK)
	{
		DBG ("Error response: " + status);
		return;
	}

	if (messageObject->hasProperty(Identifiers::GetResponse))
	{
		handleGetResponse(messageObject);
		return;
	}
}

Result WorkbenchClient::getSystemInfo()
{
	DBG("WorkbenchClient::getSystemInfo");

	return getProperty(Identifiers::System, new DynamicObject());
}

Result WorkbenchClient::getTalkerStreams()
{
	DBG("WorkbenchClient::getTalkerStreams");
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

Result WorkbenchClient::getProperty( Identifier const ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject());
	commandObject->setProperty(ID, parameter);
	messageObject.setProperty(Identifiers::GetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	MemoryOutputStream stream;
	messageObject.writeAsJSON(stream, 0, true);
	DBG(stream.toString());
	sendMessage(stream.getMemoryBlock());
	stringBroadcaster.sendActionMessage(Send + stream.toString());

	return Result::ok();
}


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
	}
}

void WorkbenchClient::setStreamProperty( Identifier const type, int const streamIndex, Identifier const &ID, var const parameter )
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

	MemoryOutputStream stream;
	messageObject.writeAsJSON(stream, 0, true);
	sendMessage(stream.getMemoryBlock());
	stringBroadcaster.sendActionMessage(Send + stream.toString());
}
