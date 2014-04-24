#pragma once

#include "JuceHeader.h"

class WorkbenchClient : public InterprocessConnection
{
public:
	WorkbenchClient();
	~WorkbenchClient();

	virtual void connectionMade();

	virtual void connectionLost();

	virtual void messageReceived( const MemoryBlock& message );

protected:
	JUCE_LEAK_DETECTOR(WorkbenchClient)
};

