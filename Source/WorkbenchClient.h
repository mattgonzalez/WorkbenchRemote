#pragma once

#include "JuceHeader.h"

class WorkbenchClient : public InterprocessConnection
{
public:
	WorkbenchClient();
	~WorkbenchClient();

	Result getSystemInfo();
	Result getTalkerStreams();
	void setTalkerStream(String streamID);

	ChangeBroadcaster broadcaster;

protected:
	Atomic<int> sequence;

	virtual void connectionMade();
	virtual void connectionLost();
	virtual void messageReceived( const MemoryBlock& message );
	Result getProperty (Identifier const ID);
	JUCE_LEAK_DETECTOR(WorkbenchClient)
};

