#pragma once

#include "JuceHeader.h"

class Client : public InterprocessConnection
{
public:
	Client();
	~Client();

	virtual void connectionMade();

	virtual void connectionLost();

	virtual void messageReceived( const MemoryBlock& message );

protected:
	JUCE_LEAK_DETECTOR(Client)
};

