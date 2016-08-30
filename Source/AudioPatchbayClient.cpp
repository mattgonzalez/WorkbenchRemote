/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"
#include "Identifiers.h"
#include "Strings.h"
#include "OutputChannel.h"

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
	RemoteClient(settings_, 'HCNB'),
	audioDevicesTree (settings_->getAudioDevicesTree())
{
	//DBG("AudioPatchbayClient::AudioPatchbayClient()");

	audioDevicesTree.addListener(this);
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

	RemoteClient::connectionMade();
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

	RemoteClient::connectionMade();
}

//============================================================================
//
// Handle responses from AudioPatchbay
//
//============================================================================

void AudioPatchbayClient::handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage)
{
	var messagePropertyVar(messageObject->getProperty(expectedMessage));
	DynamicObject * propertyObject = messagePropertyVar.getDynamicObject();

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

		if (messageObject->hasProperty(Identifiers::GetResponse))
		{
			handleGetCurrentAudioDevicesResponse(propertyVar);
		}
		if (messageObject->hasProperty(Identifiers::PropertyChanged))
		{
			handleCurrentAudioDevicesChangeNotification(propertyVar);
		}
		return;
	}

	if (propertyObject->hasProperty(Identifiers::Input))
	{
		var propertyVar(propertyObject->getProperty(Identifiers::Input));

		if (false== propertyVar.isArray())
		{
			DBG("Could not parse get inputs response");
			return;
		}

		handleGetInputsResponse(propertyVar);
		return;
	}

	if (propertyObject->hasProperty(Identifiers::Output))
	{
		var propertyVar(propertyObject->getProperty(Identifiers::Output));

		if (false== propertyVar.isArray())
		{
			DBG("Could not parse get outputs response");
			return;
		}

		handleGetOutputsResponse(propertyVar);
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
	return getProperty(Identifiers::AvailableAudioDevices, new DynamicObject);
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

	arrayVar.resize(0); // force the var to become a zero-length array

	for (int i = 0; i < tree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject);
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
		var inputVar(d->getProperty(Identifiers::Input));
		var outputVar(d->getProperty(Identifiers::Output));
		int numInputs = inputVar.size();
		int numOutputs = outputVar.size();

		//int numInputs(d->getProperty(Identifiers::Input));
		//int numOutputs(d->getProperty(Identifiers::Output));

		if (d->hasProperty(Identifiers::DeviceName))
		{
			deviceTree.setProperty(Identifiers::DeviceName, d->getProperty(Identifiers::DeviceName), nullptr);
		}
 		if (d->hasProperty(Identifiers::DeviceName)
 			&& inputVar.isArray()
 			|| outputVar.isArray())
		{
			settings->initializeAudioDevice(deviceIndex,
				numInputs,
				numOutputs);
		}
		else
		{
			continue;
		}

		handleGetInputsResponse(inputVar);
		handleGetOutputsResponse(outputVar);

		if (d->hasProperty(Identifiers::SampleRates))
		{
			deviceTree.setProperty(Identifiers::SampleRates, d->getProperty(Identifiers::SampleRates), nullptr);
		}
		if (d->hasProperty(Identifiers::SampleRate))
		{
			deviceTree.setProperty(Identifiers::SampleRate, (double)d->getProperty(Identifiers::SampleRate), nullptr);
		}
	}
}

void AudioPatchbayClient::handleCurrentAudioDevicesChangeNotification( var currentPropertyVar )
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

		int numInputs(d->getProperty(Identifiers::Input));
		int numOutputs(d->getProperty(Identifiers::Output));

		if (d->hasProperty(Identifiers::DeviceName))
		{
			deviceTree.setProperty(Identifiers::DeviceName, d->getProperty(Identifiers::DeviceName), nullptr);
		}		
	
		settings->initializeAudioDevice(deviceIndex,
			numInputs,
			numOutputs);

		if (d->hasProperty(Identifiers::SampleRates))
		{
			deviceTree.setProperty(Identifiers::SampleRates, d->getProperty(Identifiers::SampleRates), nullptr);
		}
		if (d->hasProperty(Identifiers::SampleRate))
		{
			deviceTree.setProperty(Identifiers::SampleRate, (double)d->getProperty(Identifiers::SampleRate), nullptr);
		}
	}
}

Result AudioPatchbayClient::getInputChannel( String const deviceName, int const channel)
{
	var arrayVar;
	DynamicObject::Ptr channelObject(new DynamicObject);

	arrayVar.resize(0); // force the var to become a zero-length array

	channelObject->setProperty(Identifiers::DeviceName, deviceName);
	channelObject->setProperty(Identifiers::Channel, channel);
	arrayVar.append(var(channelObject));

	return getProperty(Identifiers::Input, arrayVar);
}

Result AudioPatchbayClient::getOutputChannel( String const deviceName, int const channel )
{
	var arrayVar;
	DynamicObject::Ptr channelObject(new DynamicObject);

	arrayVar.resize(0); // force the var to become a zero-length array

	channelObject->setProperty(Identifiers::DeviceName, deviceName);
	channelObject->setProperty(Identifiers::Channel, channel);
	arrayVar.append(var(channelObject));

	return getProperty(Identifiers::Output, arrayVar);
}

void AudioPatchbayClient::handleGetInputsResponse( var inputsPropertyVar )
{
	handleGetChannelsResponse(inputsPropertyVar, Identifiers::Input);
}

void AudioPatchbayClient::handleGetOutputsResponse( var outputsPropertyVar )
{
	handleGetChannelsResponse(outputsPropertyVar, Identifiers::Output);
}

void AudioPatchbayClient::handleGetChannelsResponse( var channelsPropertyVar, Identifier const type )
{
	if (false == channelsPropertyVar.isArray())
		return;

	ScopedLock locker(lock);

	for (int responseIndex = 0; responseIndex < channelsPropertyVar.size(); responseIndex++)
	{
		var const &channelPropertyVar(channelsPropertyVar[responseIndex]);
		DynamicObject::Ptr const channelObject(channelPropertyVar.getDynamicObject());
		if (nullptr == channelObject || false == channelObject->hasProperty(Identifiers::Channel) || false == channelObject->hasProperty(Identifiers::DeviceName))
		{
			continue;
		}

		ValueTree tree(settings->getAudioDevicesTree());
		ValueTree deviceTree;
		for (int deviceIndex = 0; deviceIndex < tree.getNumChildren(); deviceIndex++)
		{
			if (tree.getChild(deviceIndex).getProperty(Identifiers::DeviceName) == channelObject->getProperty(Identifiers::DeviceName))
			{
				deviceTree = tree.getChild(deviceIndex);
				break;
			}
		}
		if (false == deviceTree.isValid())
		{
			continue;
		}

		ValueTree channelsTree(deviceTree.getChildWithName(type));
		ValueTree channelTree(channelsTree.getChild(channelObject->getProperty(Identifiers::Channel)));
		if (false == channelTree.isValid())
		{
			continue;
		}

		if (channelObject->hasProperty(Identifiers::Name))
		{
			channelTree.setProperty(Identifiers::Name, channelObject->getProperty(Identifiers::Name), nullptr);
		}

		if (channelObject->hasProperty(Identifiers::Mute))
		{
			channelTree.setProperty(Identifiers::Mute, (bool)channelObject->getProperty(Identifiers::Mute), nullptr);
		}

		if (channelObject->hasProperty(Identifiers::GainDecibels))
		{
			channelTree.setProperty(Identifiers::GainDecibels, (double)channelObject->getProperty(Identifiers::GainDecibels), nullptr);
		}

		if (channelObject->hasProperty(Identifiers::ToneFrequency))
		{
			channelTree.setProperty(Identifiers::ToneFrequency, (double)channelObject->getProperty(Identifiers::ToneFrequency), nullptr);
		}

		if (channelObject->hasProperty(Identifiers::SourceDeviceName))
		{
			channelTree.setProperty(Identifiers::SourceDeviceName, channelObject->getProperty(Identifiers::SourceDeviceName), nullptr);
		}

		if (channelObject->hasProperty(Identifiers::SourceChannel))
		{
			channelTree.setProperty(Identifiers::SourceChannel, (int)channelObject->getProperty(Identifiers::SourceChannel), nullptr);
		}

		if (channelObject->hasProperty(Identifiers::Mode))
		{
			String modeString(channelObject->getProperty(Identifiers::Mode).toString());
			int mode = OutputChannel::MODE_NONE;

			if (modeString == Strings::mute)
			{
				mode = OutputChannel::MODE_MUTE;
			}
			else if (modeString == Strings::tone)
			{
				mode = OutputChannel::MODE_TONE;
			}
			else if (modeString == Strings::input)
			{
				mode = OutputChannel::MODE_INPUT;
			}

			channelTree.setProperty(Identifiers::Mode, mode, nullptr);
		}
	}
}

Result AudioPatchbayClient::setDeviceProperty( int const deviceIndex, Identifier const identifier, var const value )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);
	var deviceArrayVar;
	DynamicObject::Ptr deviceObject(new DynamicObject);

	deviceArrayVar.resize(0); // force the var to become a zero-length array

	deviceObject->setProperty(Identifiers::Index, deviceIndex);
	deviceObject->setProperty(identifier, value);
	deviceArrayVar.append(var(deviceObject));

	commandObject->setProperty(Identifiers::CurrentAudioDevices, deviceArrayVar);
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

void AudioPatchbayClient::handleSetResponse( DynamicObject * messageObject )
{
}

void AudioPatchbayClient::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	DBG("AudioPatchbayClient::valueTreePropertyChanged " << treeWhosePropertyHasChanged.getType().toString() << " " << property.toString());

	if (Identifiers::DeviceName == property || Identifiers::SampleRate == property)
	{
		int deviceIndex = treeWhosePropertyHasChanged.getProperty(Identifiers::Index,-1);
		if (deviceIndex < 0 || deviceIndex >= audioDevicesTree.getNumChildren())
			return;

		setDeviceProperty(deviceIndex, property, treeWhosePropertyHasChanged[property]);
		return;
	}

	if (Identifiers::GainDecibels == property ||
		Identifiers::Mute == property ||
		Identifiers::ToneFrequency == property ||
		Identifiers::Mode == property ||
		Identifiers::SourceDeviceName == property ||
		Identifiers::SourceChannel == property)
	{
		sendChannelProperty(treeWhosePropertyHasChanged);
		return;
	}
}

void AudioPatchbayClient::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void AudioPatchbayClient::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int )
{
}

void AudioPatchbayClient::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int )
{
}

void AudioPatchbayClient::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

Result AudioPatchbayClient::sendChannelProperty( ValueTree channelTree )
{
	ValueTree outputsTree(channelTree.getParent());
	ValueTree deviceTree(outputsTree.getParent());
	int deviceIndex = deviceTree.getProperty(Identifiers::Index,-1);
	if (deviceIndex < 0 || deviceIndex >= audioDevicesTree.getNumChildren())
		return Result::fail("Device not found");

	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);
	DynamicObject::Ptr channelObject(new DynamicObject);
	var channelArrayVar;

	channelArrayVar.resize(0); // force the var to become a zero-length array

	channelObject->setProperty(Identifiers::DeviceName, deviceTree[Identifiers::DeviceName]);
	channelObject->setProperty(Identifiers::Channel, channelTree[Identifiers::Channel]);

	int mode = channelTree[Identifiers::Mode];
	String modeString;
	switch (mode)
	{
	case OutputChannel::MODE_MUTE:
		modeString = Strings::mute;
		break;

	case OutputChannel::MODE_TONE:
		modeString = Strings::tone;
		break;

	case OutputChannel::MODE_INPUT:
		modeString = Strings::input;
		break;
	}

	if (modeString.isNotEmpty())
	{
		channelObject->setProperty(Identifiers::Mode, modeString);

		if (channelTree.hasProperty(Identifiers::SourceDeviceName))
		{
			channelObject->setProperty(Identifiers::SourceDeviceName, channelTree[Identifiers::SourceDeviceName]);
		}
		if (channelTree.hasProperty(Identifiers::SourceChannel))
		{
			channelObject->setProperty(Identifiers::SourceChannel, (int)channelTree[Identifiers::SourceChannel]);
		}
	}

	if (channelTree.hasProperty(Identifiers::GainDecibels))
	{
		channelObject->setProperty(Identifiers::GainDecibels, (double) channelTree[Identifiers::GainDecibels]);
	}

	if (channelTree.hasProperty(Identifiers::Mute))
	{
		channelObject->setProperty(Identifiers::Mute, (bool)channelTree[Identifiers::Mute]);
	}

	if (channelTree.hasProperty(Identifiers::ToneFrequency))
	{
		channelObject->setProperty(Identifiers::ToneFrequency, (double) channelTree[Identifiers::ToneFrequency]);
	}

	channelArrayVar.append(var(channelObject));

	commandObject->setProperty(Identifiers::Output, channelArrayVar);
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}