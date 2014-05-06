#include "base.h"
#include "AudioPatchbayClient.h"
#include "Settings.h"
#include "Identifiers.h"

/*

This class uses the JUCE InterprocessConnection class to send and receive data
over a socket.  The actual data sent over the socket consists of a four byte magic number, 
followed by a four byte little-endian message length count in bytes, followed by the JSON string.

For example, to transmit this 45 byte JSON string:

{"GetCommand": {"System": {}}, "Sequence": 1}

the following bytes are sent over the socket:

574f524b			Magic number 'BNCH'
2d000000			Byte count (32 bits, endian swapped)
7b22476574436f6d	{"GetCom
6d616e64223a207b	mand": {
2253797374656d22	"System"
3a207b7d7d2c2022	: {}}, "
53657175656e6365	Sequence
223a20317d			": 1}

Strings are sent without a zero terminator.

*/

//============================================================================
//
// Constructor and destructor
//
//============================================================================

//
// The 'KROW' value is the magic number written to the start of the packet
// that has to match at both ends of the connection
//
AudioPatchbayClient::AudioPatchbayClient(Settings* settings_):
	InterprocessConnection(true, 'HCNB'),
	settings(settings_),
	commandSequence(0)
{
	//DBG("AudioPatchbayClient::AudioPatchbayClient()");
}

AudioPatchbayClient::~AudioPatchbayClient()
{
	//DBG("AudioPatchbayClient::~AudioPatchbayClient()");
}

Result AudioPatchbayClient::getSystemInfo()
{
	return Result::ok();
}

Result AudioPatchbayClient::getTalkerStreams()
{
	return Result::ok();
}

Result AudioPatchbayClient::setStreamProperty( Identifier const type, int const streamIndex, Identifier const &ID, var const parameter )
{
	return Result::ok();
}

void AudioPatchbayClient::connectionMade()
{

}

void AudioPatchbayClient::connectionLost()
{

}

void AudioPatchbayClient::messageReceived( const MemoryBlock& message )
{

}

void AudioPatchbayClient::handleGetResponse( DynamicObject * messageObject )
{

}

void AudioPatchbayClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{

}

void AudioPatchbayClient::handleGetTalkersResponse( var talkersPropertyVar )
{

}

Result AudioPatchbayClient::getProperty( Identifier const ID, var const parameter )
{
	return Result::ok();
}

Result AudioPatchbayClient::sendJSONToSocket( DynamicObject &messageObject )
{
	return Result::ok();
}

