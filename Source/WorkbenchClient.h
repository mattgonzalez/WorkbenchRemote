/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

class Settings;

class WorkbenchClient : public InterprocessConnection
{
public:
	WorkbenchClient(Settings* settings_);
	~WorkbenchClient();

	Result getSystemInfo();
	Result getTalkerStreams();
	Result setStreamProperty(Identifier const type, int const streamIndex, Identifier const &ID, var const parameter);

	ChangeBroadcaster changeBroadcaster;
	ActionBroadcaster stringBroadcaster;

protected:
	CriticalSection lock;
	int commandSequence;

	Settings* settings;

	virtual void connectionMade();
	virtual void connectionLost();
	virtual void messageReceived( const MemoryBlock& message );

	void handleGetResponse( DynamicObject * messageObject );
	void handleGetSystemResponse( DynamicObject * systemPropertyObject );
	void handleGetTalkersResponse( var talkersPropertyVar );

	Result getProperty (Identifier const ID, var const parameter);

	Result sendJSONToSocket( DynamicObject &messageObject );


	JUCE_LEAK_DETECTOR(WorkbenchClient)
};
