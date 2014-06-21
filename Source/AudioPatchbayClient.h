/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once
#include "RemoteClient.h"

class Settings;

class AudioPatchbayClient : public RemoteClient, public ValueTree::Listener
{
public:
	AudioPatchbayClient(Settings* settings_);
	~AudioPatchbayClient();

	Result getAvailableAudioDevices();
	Result getCurrentAudioDevices();
	Result getInputChannel(String const deviceName, int const channel);
	Result getOutputChannel(String const deviceName, int const channel);

protected:
	JUCE_LEAK_DETECTOR(AudioPatchbayClient)

	virtual void connectionMade();
	virtual void connectionLost();

	void handleSetResponse( DynamicObject * messageObject );
	void handleGetSystemResponse( DynamicObject * systemPropertyObject );
	virtual void handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage);
	void handleGetAvailableAudioDevicesResponse(var availablePropertyVar);
	void handleGetCurrentAudioDevicesResponse(var currentPropertyVar);
	void handleGetInputsResponse(var inputsPropertyVar);
	void handleGetOutputsResponse(var outputsPropertyVar);
	void handleGetChannelsResponse(var channelsPropertyVar, Identifier const type);
	Result setDeviceProperty(int const deviceIndex, Identifier const identifier, var const value);
	Result sendChannelProperty(ValueTree channelTree);

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

	ValueTree audioDevicesTree;
};