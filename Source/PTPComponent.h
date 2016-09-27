/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#pragma once
#include "GrandmasterComponent.h"
#include "FollowerComponent.h"
#include "DelayMeasurementComponent.h"
#include "PTPFaultInjectionComponent.h"

class WorkbenchClient;

class PTPComponent : public Component, public ValueTree::Listener
{
public:
	PTPComponent(ValueTree tree, WorkbenchClient * client_);
	~PTPComponent();

	enum 
	{
		CONFIG_FOLLOWER = 100,
		CONFIG_GRANDMASTER,
		CONFIG_BMCA,
		CONFIG_DELAY_REQUESTS_DISABLED = -1,
		MIN_DELAY_REQUEST_INTERVAL_MILLISECONDS = 125,
		MAX_DELAY_REQUEST_INTERVAL_MILLISECONDS = 4000
	};

protected:
	virtual void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property);

	virtual void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded);

	virtual void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int);

	virtual void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int, int);

	virtual void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged);

	virtual void resized();

	ValueTree ptpTree;

	Label ptpRoleLabel;

	GrandmasterComponent grandmasterComponent;
	FollowerComponent followerComponent;
	DelayMeasurementComponent delayMeasurementComponent;
	PTPFaultInjectionComponent ptpFaultInjectionComponent;

	ComboBox ptpRoleComboBox;

	WorkbenchClient *client;
};