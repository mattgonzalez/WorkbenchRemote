#include "WorkbenchClient.h"

namespace Identifiers
{
	Identifier const GetCommand("GetCommand");
	Identifier const Property("Property");
	Identifier const System("System");
	Identifier const Talkers("Talkers");
	Identifier const Listeners("Listeners");
	Identifier const Version("Version");
	Identifier const Sequence("Sequence");
};

WorkbenchClient::WorkbenchClient()
{
	DBG("WorkbenchClient::WorkbenchClient() ");
}

WorkbenchClient::~WorkbenchClient()
{
	DBG("WorkbenchClient::~WorkbenchClient() ");
}

void WorkbenchClient::connectionMade()
{
	DBG("WorkbenchClient::connectionMade");
	broadcaster.sendChangeMessage();
}

void WorkbenchClient::connectionLost()
{
	DBG("WorkbenchClient::connectionLost");
	broadcaster.sendChangeMessage();
}

void WorkbenchClient::messageReceived( const MemoryBlock& message )
{
	DBG("WorkbenchClient::messageReceived  bytes:" << (int)message.getSize());
#if JUCE_DEBUG
	var json(JSON::parse(message.toString()));
	DBG(JSON::toString(json));
#endif
}

Result WorkbenchClient::getSystemInfo()
{
	DBG("WorkbenchClient::getSystemInfo");

	return getProperty(Identifiers::System);
}

Result WorkbenchClient::getTalkerStreams()
{
	DBG("WorkbenchClient::getTalkerStreams");

	return getProperty(Identifiers::Talkers);
}

Result WorkbenchClient::getProperty( Identifier const ID )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject());
	commandObject->setProperty(Identifiers::Property, ID.toString());
	messageObject.setProperty(Identifiers::GetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, ++sequence);

	MemoryOutputStream stream;
	messageObject.writeAsJSON(stream, 0, true);
	DBG(stream.toString());
	sendMessage(stream.getMemoryBlock());

	return Result::ok();
}
