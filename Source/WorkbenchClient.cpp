#include "WorkbenchClient.h"

namespace Identifiers
{
	Identifier const GetCommand("GetCommand");
	Identifier const SetCommand("SetCommand");
	Identifier const Property("Property");
	Identifier const System("System");
	Identifier const GetTalkersInfo("GetTalkersInfo");
	Identifier const SetTalkerInfo("SetTalkerInfo");
	Identifier const Listeners("Listeners");
	Identifier const Version("Version");
	Identifier const StreamID("StreamID");
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

	return getProperty(Identifiers::GetTalkersInfo);
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

void WorkbenchClient::setTalkerStream( String streamID )
{
	DBG("WorkbenchClient::setTalkerStream");

	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject());
	DynamicObject::Ptr propertyObject(new DynamicObject());
	propertyObject->setProperty(Identifiers::StreamID, streamID);
	commandObject->setProperty(Identifiers::Property, var(propertyObject));
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, ++sequence);

	MemoryOutputStream stream;
	messageObject.writeAsJSON(stream, 0, true);
	DBG(stream.toString());
	sendMessage(stream.getMemoryBlock());
}