#include "base.h"
#include "WorkbenchClient.h"
#include "Identifiers.h"
#include "AVTP.h"
#include "FaultInjection.h"
#include "Settings.h"

void WorkbenchClient::handleAVTPObject(DynamicObject* avtpPropertyObject)
{
	ScopedLock locker(settings->lock);

	if (avtpPropertyObject->hasProperty(Identifiers::TalkerPresentationOffsetMsec))
	{
		double offset = avtpPropertyObject->getProperty(Identifiers::TalkerPresentationOffsetMsec);
		offset = jlimit(0.0, (double)AVTP::MAX_TALKER_PRESENTATION_OFFSET_MSEC, offset);
		avtpTree.setProperty(Identifiers::TalkerPresentationOffsetMsec, offset, nullptr);
	}

	if (avtpPropertyObject->hasProperty(Identifiers::ListenerPresentationOffsetMsec))
	{
		double offset = avtpPropertyObject->getProperty(Identifiers::ListenerPresentationOffsetMsec);
		offset = jlimit(0.0, (double)AVTP::MAX_LISTENER_PRESENTATION_OFFSET_MSEC, offset);
		avtpTree.setProperty(Identifiers::ListenerPresentationOffsetMsec, offset, nullptr);
	}

	if (avtpPropertyObject->hasProperty(Identifiers::FaultLogging))
	{
		DynamicObject* faultLoggingObject = avtpPropertyObject->getProperty(Identifiers::FaultLogging).getDynamicObject();
		if (faultLoggingObject)
		{
			ValueTree avtpFaultLoggingTree(avtpTree.getChildWithName(Identifiers::FaultLogging));
			if (faultLoggingObject->hasProperty(Identifiers::TimestampTolerancePercent))
			{
				double tolerance = faultLoggingObject->getProperty(Identifiers::TimestampTolerancePercent);
				tolerance = jlimit(0.0, 100.0, tolerance);
				avtpFaultLoggingTree.setProperty(Identifiers::TimestampTolerancePercent, tolerance, nullptr);
			}
		}
	}
}

DynamicObject* WorkbenchClient::createAVTPObject(ValueTree const tree, Identifier const &property)
{
	DynamicObject* object = new DynamicObject;

	if (tree.getType() == Identifiers::FaultLogging)
	{
		DynamicObject* faultLoggingObject = new DynamicObject;
		faultLoggingObject->setProperty(property, tree[property]);
		object->setProperty(Identifiers::FaultLogging, var(faultLoggingObject));
	}
	else
	{
		object->setProperty(property, tree[property]);
	}

	return object;
}
