#include "WorkbenchClient.h"

WorkbenchClient::WorkbenchClient()
{
	DBG("WorkbenchClient::WorkbenchClient() " << (pointer_sized_int)this);
}

WorkbenchClient::~WorkbenchClient()
{
	DBG("WorkbenchClient::~WorkbenchClient() " << (pointer_sized_int)this);
}

void WorkbenchClient::connectionMade()
{
	DBG("WorkbenchClient::connectionMade");
}

void WorkbenchClient::connectionLost()
{
	DBG("WorkbenchClient::connectionLost");

}

void WorkbenchClient::messageReceived( const MemoryBlock& message )
{
	DBG("WorkbenchClient::messageReceived");
}
