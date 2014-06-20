/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

class Settings;

class AudioPatchbayClient : public InterprocessConnection
{
public:
	AudioPatchbayClient(Settings* settings_);
	~AudioPatchbayClient();

	Result getSystemInfo();
	Result getAvailableAudioDevices();
	Result getCurrentAudioDevices();

	ChangeBroadcaster changeBroadcaster;

	Value lastMessageSent;
	Value lastMessageReceived;

protected:
	CriticalSection lock;
	int commandSequence;

	Settings* settings;

	virtual void connectionMade();
	virtual void connectionLost();
	virtual void messageReceived( const MemoryBlock& message );

	void handleGetResponse( DynamicObject * messageObject );
	void handleGetSystemResponse( DynamicObject * systemPropertyObject );
	void handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage);
	void handleGetAvailableAudioDevicesResponse(var availablePropertyVar);
	void handleGetCurrentAudioDevicesResponse(var currentPropertyVar);

	Result getProperty (Identifier const ID, var const parameter);

	Result sendJSONToSocket( DynamicObject &messageObject );

	JUCE_LEAK_DETECTOR(AudioPatchbayClient)
};