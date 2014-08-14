/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

#include "RemoteClient.h"

class Settings;

class WorkbenchClient : public RemoteClient, public ValueTree::Listener
{
public:
	WorkbenchClient(Settings* settings_);
	~WorkbenchClient();

	Result getTalkerStreams();
	Result getListenerStreams();
	Result getLinkState();
	Result getSettings();

	Result setStreamProperty(Identifier const type, int const streamIndex, Identifier const &ID, var const parameter);
	Result setSettingsProperty(Identifier const & ID, var const parameter);

protected:
	ValueTree workbenchSettingsTree;

	virtual void connectionMade();
	virtual void connectionLost();
	//virtual void messageReceived( const MemoryBlock& message );

	void handleGetResponse( DynamicObject * messageObject );
	void handleGetSystemResponse( DynamicObject * systemPropertyObject );
	void handleGetStreamsResponse( var streamsPropertyVar, ValueTree streamsTree );
	void handleGetWorkbenchSettingsResponse( DynamicObject* workbenchSettingsPropertyObject );
	virtual void handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage);
	void handleFaultNotificationMessage(DynamicObject * messageObject);

	JUCE_LEAK_DETECTOR(WorkbenchClient)
		void handleGetLinkStateResponse( DynamicObject* linkStatePropertyObject );

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );

	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );

	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );

	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );

	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
};

