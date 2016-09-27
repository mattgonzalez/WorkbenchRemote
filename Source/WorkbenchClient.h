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
	Result getPTP();

	Result setStreamProperty(Identifier const type, int const streamIndex, Identifier const &ID, var const parameter);
	Result setSettingsProperty(Identifier const &commandProperty, Identifier const &property, var const &parameter);
	Result setPTPFaultInjectionProperty(Identifier const &property);
	Result setPTPCorruptPacketField(ValueTree &corruptFieldTree);

protected:
	ValueTree workbenchSettingsTree;
	ValueTree ptpTree;

	virtual void connectionMade();
	virtual void connectionLost();
	//virtual void messageReceived( const MemoryBlock& message );

	void handleGetSystemResponse( DynamicObject * systemPropertyObject );
	void handleGetStreamsResponse( var streamsPropertyVar, ValueTree streamsTree );
	void handleGetWorkbenchSettingsResponse( DynamicObject* workbenchSettingsPropertyObject );
	void handleGetPTPResponse(DynamicObject* ptpPropertyObject);

	void handlePTPPacketFieldCorruption(Identifier const &packetTypeIdentifier, DynamicObject::Ptr faultInjectionObject, ValueTree &ptpFaultInjectionTree);

	virtual void handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage);
	void handleFaultNotificationMessage(DynamicObject * messageObject);

	JUCE_LEAK_DETECTOR(WorkbenchClient)
		void handleGetLinkStateResponse( DynamicObject* linkStatePropertyObject );

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

	//
	// Some JSON properties are reported as plain-text strings; use these strings for comparison
	//
	static const String followerString;
	static const String grandmasterString;
	static const String BMCAString;
	static const String ethernetString;
	static const String masterString;
	static const String slaveString;
	static const String lockedString;
	static const String unlockedString;
	static const String onceString;
	static const String repeatString;
	static const String repeatContinuouslyString;
};

