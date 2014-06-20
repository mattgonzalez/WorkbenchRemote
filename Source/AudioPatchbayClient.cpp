/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"
#include "Identifiers.h"

// const String Receive("Receive");
// const String Send("Send");
const String HexChars("0123456789abcdef");
const String OK("OK");

/*

This class uses the JUCE InterprocessConnection class to send and receive data
over a socket.  The actual data sent over the socket consists of a four byte magic number, 
followed by a four byte little-endian message length count in bytes, followed by the JSON string.

For example, to transmit this 45 byte JSON string:

{"GetCommand": {"System": {}}, "Sequence": 1}

the following bytes are sent over the socket:

574f524b			Magic number 'BNCH'
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
// The 'HCNB' value is the magic number written to the start of the packet
// that has to match at both ends of the connection
//
AudioPatchbayClient::AudioPatchbayClient(Settings* settings_):
	InterprocessConnection( true, 'HCNB'),
	settings(settings_),
	commandSequence(0)
{
	//DBG("AudioPatchbayClient::AudioPatchbayClient()");
}

AudioPatchbayClient::~AudioPatchbayClient()
{
	//DBG("AudioPatchbayClient::~AudioPatchbayClient()");
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
void AudioPatchbayClient::connectionMade()
{
	DBG("AudioPatchbayClient::connectionMade");

	// Generate a change notification indicating that the state of the socket has changed
	changeBroadcaster.sendChangeMessage();
}

//---------------------------------------------------------------------------
//
// Callback indicating that the socket has disconnected
//
void AudioPatchbayClient::connectionLost()
{
	DBG("AudioPatchbayClient::connectionLost");

	// Remove all the stream information from the settings tree
	settings->removeAudioDevices();

	// Generate a change notification indicating that the state of the socket has changed
	changeBroadcaster.sendChangeMessage();
}

//---------------------------------------------------------------------------
//
// Callback indicating that the socket has received a message
//
void AudioPatchbayClient::messageReceived( const MemoryBlock& message )
{
	ScopedLock locker(lock);

	DBG("AudioPatchbayClient::messageReceived  bytes:" << (int)message.getSize());

	// The MemoryBlock parameter is just the JSON string without the magic number or byte count
	// Parse the MemoryBlock as a JSON String
	var messageVar;
	Result parseResult(JSON::parse(message.toString(), messageVar));
	if (parseResult.failed())
	{
		DBG("parse failed " + parseResult.getErrorMessage());
		return;
	}

	lastMessageReceived = message.toString();

	// Start reading properties from the JSON data
	DynamicObject * messageObject = messageVar.getDynamicObject();
	if (nullptr == messageObject)
	{
		DBG ("Could not convert JSON var to DynamicObject");
		return;
	}

	if (messageObject->hasProperty(Identifiers::PropertyChanged))
	{
		handlePropertyChangedMessage(messageObject, Identifiers::PropertyChanged);
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
		handlePropertyChangedMessage(messageObject, Identifiers::GetResponse);
		return;
	}
}


//============================================================================
//
// Commands sent to AudioPatchbay
//
//============================================================================

Result AudioPatchbayClient::getSystemInfo()
{
	return getProperty(Identifiers::System, new DynamicObject());
}

Result AudioPatchbayClient::getProperty( Identifier const ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject());
	commandObject->setProperty(ID, parameter);
	messageObject.setProperty(Identifiers::GetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

Result AudioPatchbayClient::sendJSONToSocket( DynamicObject &messageObject )
{
	// Convert the DynamicObject to a JSON string
	MemoryOutputStream stream;
	messageObject.writeAsJSON(stream, 0, true);

	// Update the user interface
	lastMessageSent = stream.toString();

	// Transmit the JSON string.  The sendMessage function
	// will prepend the data with the magic number and byte count.
	bool success = sendMessage(stream.getMemoryBlock());
	if (success)
		return Result::ok();

	return Result::fail("InterprocessConnection::sendMessage failed");
}

//============================================================================
//
// Handle responses from AudioPatchbay
//
//============================================================================

void AudioPatchbayClient::handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage)
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

	if (propertyObject->hasProperty(Identifiers::AvailableAudioDevices))
	{
		var availablePropertyVar(propertyObject->getProperty(Identifiers::AvailableAudioDevices));

		if (false== availablePropertyVar.isArray())
		{
			DBG("Could not parse get available audio devices response");
			return;
		}

		handleGetAvailableAudioDevicesResponse(availablePropertyVar);
		return;
	}

	if (propertyObject->hasProperty(Identifiers::CurrentAudioDevices))
	{
		var propertyVar(propertyObject->getProperty(Identifiers::CurrentAudioDevices));

		if (false== propertyVar.isArray())
		{
			DBG("Could not parse get current audio devices response");
			return;
		}

		handleGetCurrentAudioDevicesResponse(propertyVar);
		return;
	}
}

void AudioPatchbayClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{
	int numAudioDevices = systemPropertyObject->getProperty(Identifiers::AudioDevices);

	settings->initializeAudioDevices(numAudioDevices);
}

Result AudioPatchbayClient::getAvailableAudioDevices()
{
	return getProperty(Identifiers::AvailableAudioDevices, new DynamicObject());
}

void AudioPatchbayClient::handleGetAvailableAudioDevicesResponse( var availablePropertyVar )
{
	ScopedLock locker(lock);

	ValueTree tree(settings->getAudioDevicesTree());
	tree.setProperty(Identifiers::AvailableAudioDevices, availablePropertyVar, nullptr);
}

Result AudioPatchbayClient::getCurrentAudioDevices()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree tree(settings->getAudioDevicesTree());
	for (int i = 0; i < tree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject());
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::CurrentAudioDevices, arrayVar);
}

void AudioPatchbayClient::handleGetCurrentAudioDevicesResponse( var currentPropertyVar )
{
	if (false == currentPropertyVar.isArray())
		return;

	ScopedLock locker(lock);

	ValueTree tree(settings->getAudioDevicesTree());
	for (int responseIndex = 0; responseIndex < currentPropertyVar.size(); responseIndex++)
	{
		var const &v(currentPropertyVar[responseIndex]);
		DynamicObject::Ptr const d(v.getDynamicObject());
		if (nullptr == d || false == d->hasProperty(Identifiers::Index))
		{
			continue;
		}

		int deviceIndex = d->getProperty(Identifiers::Index);
		if (deviceIndex < 0 || deviceIndex >= tree.getNumChildren())
		{
			continue;
		}

		ValueTree deviceTree(tree.getChild(deviceIndex));

		if (d->hasProperty(Identifiers::DeviceName))
		{
			deviceTree.setProperty(Identifiers::DeviceName, d->getProperty(Identifiers::DeviceName), nullptr);
		}
		if (d->hasProperty(Identifiers::Inputs))
		{
			deviceTree.setProperty(Identifiers::Inputs, d->getProperty(Identifiers::Inputs), nullptr);
		}		
		if (d->hasProperty(Identifiers::Outputs))
		{
			deviceTree.setProperty(Identifiers::Outputs, d->getProperty(Identifiers::Outputs), nullptr);
		}
		if (d->hasProperty(Identifiers::SampleRates))
		{
			deviceTree.setProperty(Identifiers::SampleRates, d->getProperty(Identifiers::SampleRates), nullptr);
		}
		if (d->hasProperty(Identifiers::SampleRate))
		{
			deviceTree.setProperty(Identifiers::SampleRate, d->getProperty(Identifiers::SampleRate), nullptr);
		}

		if (d->hasProperty(Identifiers::DeviceName) && d->hasProperty(Identifiers::Inputs) && d->hasProperty(Identifiers::Outputs))
		{
			settings->initializeAudioDevice(deviceIndex,
				d->getProperty(Identifiers::Inputs),
				d->getProperty(Identifiers::Outputs));
		}
	}
}
