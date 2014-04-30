#pragma once

#include "JuceHeader.h"

class Settings;

class WorkbenchClient : public InterprocessConnection
{
public:
	WorkbenchClient(Settings* settings_);
	~WorkbenchClient();

	Result getSystemInfo();
	Result getTalkerStreams();
	void setStreamProperty(Identifier const type, int const streamIndex, Identifier const &ID, var const parameter);

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


	JUCE_LEAK_DETECTOR(WorkbenchClient)
};

