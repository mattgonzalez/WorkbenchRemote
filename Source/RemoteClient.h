/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

class Settings;

class RemoteClient : public InterprocessConnection
{
public:
	RemoteClient(Settings* settings_, uint32 magicNumber);
	~RemoteClient();

	Result getSystemInfo();

	ChangeBroadcaster changeBroadcaster;

	Value lastMessageSent;
	Value lastMessageReceived;

protected:
	CriticalSection lock;
	int commandSequence;
	bool hostCurrentlyChangingProperty;

	Settings* settings;

	virtual void connectionMade();
	virtual void connectionLost();
	virtual void messageReceived( const MemoryBlock& message );

	virtual void handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage) = 0;

	bool inflightCommandFound(int responseSequence);

	Result getProperty (Identifier const ID, var const parameter);

	Result sendJSONToSocket( DynamicObject &messageObject );

	JUCE_LEAK_DETECTOR(RemoteClient)

	class InflightCommand
	{
	public:
		int sequence;
		Time time;
		LinkedListPointer<InflightCommand> nextListItem;

		JUCE_LEAK_DETECTOR(InflightCommand)
	};
	LinkedListPointer<InflightCommand> inflightCommands;
};