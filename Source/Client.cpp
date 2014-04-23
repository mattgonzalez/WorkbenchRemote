#include "Client.h"

Client::Client()
{
	DBG("Client::Client() " << (pointer_sized_int)this);
}

Client::~Client()
{
	DBG("Client::~Client() " << (pointer_sized_int)this);
}

void Client::connectionMade()
{
	DBG("Client::connectionMade");
}

void Client::connectionLost()
{
	DBG("Client::connectionLost");

}

void Client::messageReceived( const MemoryBlock& message )
{
	DBG("Client::messageReceived");
}
