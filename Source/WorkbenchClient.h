/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

#include "RemoteClient.h"

class Settings;

class WorkbenchClient : public RemoteClient
{
public:
	WorkbenchClient(Settings* settings_);
	~WorkbenchClient();

	Result getTalkerStreams();
	Result getListenerStreams();
	Result setStreamProperty(Identifier const type, int const streamIndex, Identifier const &ID, var const parameter);

protected:
	virtual void connectionMade();
	virtual void connectionLost();
	//virtual void messageReceived( const MemoryBlock& message );

	void handleGetResponse( DynamicObject * messageObject );
	void handleGetSystemResponse( DynamicObject * systemPropertyObject );
	void handleGetStreamsResponse( var streamsPropertyVar, ValueTree streamsTree );
	virtual void handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage);
	void handleFaultNotificationMessage(DynamicObject * messageObject);

	JUCE_LEAK_DETECTOR(WorkbenchClient)
};

