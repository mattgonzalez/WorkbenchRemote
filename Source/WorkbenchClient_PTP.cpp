#include "base.h"
#include "WorkbenchClient.h"
#include "Identifiers.h"
#include "PTP.h"
#include "FaultInjection.h"
#include "Settings.h"

void WorkbenchClient::handlePTPObject(DynamicObject* ptpPropertyObject)
{
	ScopedLock locker(settings->lock);

	if (ptpPropertyObject->hasProperty(Identifiers::StaticPTPRole))
	{
		var property(ptpPropertyObject->getProperty(Identifiers::StaticPTPRole));
		if (property.toString() == followerString)
		{
			ptpTree.setProperty(Identifiers::StaticPTPRole, PTP::CONFIG_FOLLOWER, nullptr);
		}
		if (property.toString() == grandmasterString)
		{
			ptpTree.setProperty(Identifiers::StaticPTPRole, PTP::CONFIG_GRANDMASTER, nullptr);
		}
		if (property.toString() == BMCAString)
		{
			ptpTree.setProperty(Identifiers::StaticPTPRole, PTP::CONFIG_BMCA, nullptr);
		}
	}

	if (ptpPropertyObject->hasProperty(Identifiers::PTPSendFollowupTLV))
	{
		ptpTree.setProperty(Identifiers::PTPSendFollowupTLV, ptpPropertyObject->getProperty(Identifiers::PTPSendFollowupTLV), nullptr);
	}

	if (ptpPropertyObject->hasProperty(Identifiers::PTPSendAnnounce))
	{
		ptpTree.setProperty(Identifiers::PTPSendAnnounce, ptpPropertyObject->getProperty(Identifiers::PTPSendAnnounce), nullptr);
	}

	if (ptpPropertyObject->hasProperty(Identifiers::PTPSendSignalingFlag))
	{
		ptpTree.setProperty(Identifiers::PTPSendSignalingFlag, ptpPropertyObject->getProperty(Identifiers::PTPSendSignalingFlag), nullptr);
	}

	if (ptpPropertyObject->hasProperty(Identifiers::PTPDelayRequest))
	{
		var delayRequestVar(ptpPropertyObject->getProperty(Identifiers::PTPDelayRequest));

		DynamicObject::Ptr delayRequestObject(delayRequestVar.getDynamicObject());
		if (delayRequestObject != nullptr)
		{
			if (delayRequestObject->hasProperty(Identifiers::Enabled))
			{
				bool enabled(delayRequestObject->getProperty(Identifiers::Enabled));
				if (enabled)
				{
					ptpTree.setProperty(Identifiers::PTPDelayRequestIntervalMsec, delayRequestObject->getProperty(Identifiers::Milliseconds), nullptr);
				}
				else
				{
					ptpTree.setProperty(Identifiers::PTPDelayRequestIntervalMsec, PTP::CONFIG_DELAY_REQUESTS_DISABLED, nullptr);
				}
			}
		}
	}

	if (ptpPropertyObject->hasProperty(Identifiers::PTPAutomaticGMTime))
	{
		ptpTree.setProperty(Identifiers::PTPAutomaticGMTime, (bool)ptpPropertyObject->getProperty(Identifiers::PTPAutomaticGMTime), nullptr);
	}

	if (ptpPropertyObject->hasProperty(Identifiers::PTPInitialTime))
	{
		ptpTree.setProperty(Identifiers::PTPInitialTime, (int64)ptpPropertyObject->getProperty(Identifiers::PTPInitialTime), nullptr);
	}

	if (ptpPropertyObject->hasProperty(Identifiers::FaultInjection))
	{
		DynamicObject::Ptr faultInjectionObject(ptpPropertyObject->getProperty(Identifiers::FaultInjection).getDynamicObject());
		if (faultInjectionObject != nullptr)
		{
			ValueTree ptpFaultInjectionTree(ptpTree.getChildWithName(Identifiers::FaultInjection));

			if (faultInjectionObject->hasProperty(Identifiers::Enabled))
			{
				ptpFaultInjectionTree.setProperty(Identifiers::Enabled, (bool)faultInjectionObject->getProperty(Identifiers::Enabled), nullptr);
			}

			if (faultInjectionObject->hasProperty(Identifiers::PTPFaultInjectionCycleMode))
			{
				var const &modeVar(faultInjectionObject->getProperty(Identifiers::PTPFaultInjectionCycleMode));
				int mode = -1;

				if (modeVar == onceString)
				{
					mode = FaultInjection::ONCE;
				}
				else if (modeVar == repeatString)
				{
					mode = FaultInjection::REPEAT;
				}
				else if (modeVar == repeatContinuouslyString)
				{
					mode = FaultInjection::REPEAT_CONTINUOUSLY;
				}

				if (mode >= FaultInjection::ONCE)
				{
					ptpFaultInjectionTree.setProperty(Identifiers::PTPFaultInjectionCycleMode, mode, nullptr);
				}
			}

			if (faultInjectionObject->hasProperty(Identifiers::PTPNumBadSyncFollowupPairsPerCycle))
			{
				int numBadPairs = faultInjectionObject->getProperty(Identifiers::PTPNumBadSyncFollowupPairsPerCycle);
				numBadPairs = jlimit(0, (int)FaultInjection::MAX_FAULT_INJECTION_CYCLE_LENGTH, numBadPairs);
				ptpFaultInjectionTree.setProperty(Identifiers::PTPNumBadSyncFollowupPairsPerCycle, numBadPairs, nullptr);
			}

			if (faultInjectionObject->hasProperty(Identifiers::PTPFaultInjectionCycleLengthPackets))
			{
				int length = faultInjectionObject->getProperty(Identifiers::PTPFaultInjectionCycleLengthPackets);
				length = jlimit(0, (int)FaultInjection::MAX_FAULT_INJECTION_CYCLE_LENGTH, length);
				ptpFaultInjectionTree.setProperty(Identifiers::PTPFaultInjectionCycleLengthPackets, length, nullptr);
			}

			if (faultInjectionObject->hasProperty(Identifiers::PTPNumFaultInjectionCycles))
			{
				int numCycles = faultInjectionObject->getProperty(Identifiers::PTPNumFaultInjectionCycles);
				numCycles = jlimit(0, (int)FaultInjection::MAX_FAULT_INJECTION_CYCLE_LENGTH, numCycles);
				ptpFaultInjectionTree.setProperty(Identifiers::PTPNumFaultInjectionCycles, numCycles, nullptr);
			}

			if (faultInjectionObject->hasProperty(Identifiers::Sync))
			{
				handlePTPPacketFieldCorruption(Identifiers::Sync, faultInjectionObject, ptpFaultInjectionTree);
			}

			if (faultInjectionObject->hasProperty(Identifiers::Followup))
			{
				handlePTPPacketFieldCorruption(Identifiers::Followup, faultInjectionObject, ptpFaultInjectionTree);
			}
		}
	}
}

void WorkbenchClient::handlePTPPacketFieldCorruption(Identifier const &packetTypeIdentifier, DynamicObject::Ptr faultInjectionObject, ValueTree &ptpFaultInjectionTree)
{
	DynamicObject* packetTypeObject = faultInjectionObject->getProperty(packetTypeIdentifier).getDynamicObject();

	if (packetTypeObject)
	{
		if (packetTypeObject->hasProperty(Identifiers::CorruptPackets))
		{
			DynamicObject* corruptPacketsObject = packetTypeObject->getProperty(Identifiers::CorruptPackets).getDynamicObject();
			if (corruptPacketsObject)
			{
				NamedValueSet& corruptProperties(corruptPacketsObject->getProperties());
				ValueTree corruptionTree(ptpFaultInjectionTree.getChildWithName(packetTypeIdentifier).getChildWithName(Identifiers::CorruptPackets));

				for (int propertyIndex = 0; propertyIndex < corruptProperties.size(); ++propertyIndex)
				{
					Identifier fieldProperty(corruptProperties.getName(propertyIndex));
					ValueTree fieldTree(corruptionTree.getChildWithName(fieldProperty));
					if (fieldTree.isValid())
					{
						DynamicObject* fieldObject = corruptProperties[fieldProperty].getDynamicObject();
						if (fieldObject)
						{
							var const &newValueVar(fieldObject->getProperty(Identifiers::Value));
							var const &currentValueVar(fieldTree[Identifiers::Value]);

							if (newValueVar.isArray() && size_t(newValueVar.size()) == currentValueVar.getBinaryData()->getSize())
							{
								MemoryBlock block(newValueVar.size());

								for (int i = 0; i < newValueVar.size(); ++i)
								{
									block[i] = (uint8)(int)newValueVar[i];
								}

								fieldTree.setProperty(Identifiers::Value, var(block), nullptr);
								fieldTree.setProperty(Identifiers::Enabled, (bool)fieldObject->getProperty(Identifiers::Enabled), nullptr);
								fieldTree.sendPropertyChangeMessage(Identifiers::Value);
							}
						}
					}
				}
			}
		}
	}
}

Result WorkbenchClient::setRemotePTPFaultInjectionProperty(Identifier const &property)
{
	DynamicObject::Ptr faultInjectionObject(new DynamicObject);
	ValueTree faultInjectionTree(ptpTree.getChildWithName(Identifiers::FaultInjection));

	if (Identifiers::PTPFaultInjectionCycleMode == property)
	{
		switch ((int)faultInjectionTree[property])
		{
		case FaultInjection::ONCE:
			faultInjectionObject->setProperty(property, onceString);
			break;

		case FaultInjection::REPEAT:
			faultInjectionObject->setProperty(property, repeatString);
			break;

		case FaultInjection::REPEAT_CONTINUOUSLY:
			faultInjectionObject->setProperty(property, repeatContinuouslyString);
			break;
		}
	}
	else
	{
		faultInjectionObject->setProperty(property, faultInjectionTree[property]);
	}

	DynamicObject::Ptr ptpObject(new DynamicObject);
	ptpObject->setProperty(Identifiers::FaultInjection, var(faultInjectionObject));

	DynamicObject::Ptr commandObject(new DynamicObject);
	commandObject->setProperty(Identifiers::PTP, var(ptpObject));
	DynamicObject messageObject;
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

Result WorkbenchClient::setRemotePTPCorruptPacketField(ValueTree &corruptFieldTree)
{
	var const &blockVar(corruptFieldTree[Identifiers::Value]);
	if (false == blockVar.isBinaryData())
	{
		return Result::fail("No binary data");
	}

	MemoryBlock* block = blockVar.getBinaryData();
	uint8* blockData = static_cast<uint8 *>(block->getData());
	var arrayVar;
	for (size_t i = 0; i < block->getSize(); ++i)
	{
		arrayVar.append(blockData[i]);
	}

	DynamicObject::Ptr fieldObject(new DynamicObject);
	fieldObject->setProperty(Identifiers::Value, arrayVar);
	fieldObject->setProperty(Identifiers::Enabled, corruptFieldTree[Identifiers::Enabled]);

	DynamicObject::Ptr corruptPacketsObject(new DynamicObject);
	corruptPacketsObject->setProperty(corruptFieldTree.getType(), var(fieldObject));

	DynamicObject::Ptr packetTypeObject(new DynamicObject);
	packetTypeObject->setProperty(Identifiers::CorruptPackets, var(corruptPacketsObject));

	DynamicObject::Ptr faultInjectionObject(new DynamicObject);
	faultInjectionObject->setProperty(corruptFieldTree.getParent().getParent().getType(), var(packetTypeObject));

	DynamicObject::Ptr ptpObject(new DynamicObject);
	ptpObject->setProperty(Identifiers::FaultInjection, var(faultInjectionObject));

	DynamicObject::Ptr commandObject(new DynamicObject);
	commandObject->setProperty(Identifiers::PTP, var(ptpObject));
	DynamicObject messageObject;
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

void WorkbenchClient::setPTPRoleProperty(DynamicObject* object, var const &value)
{
	switch ((int)value)
	{
		case PTP::CONFIG_FOLLOWER:
		{
			object->setProperty(Identifiers::StaticPTPRole, followerString);
			break;
		}
		case PTP::CONFIG_GRANDMASTER:
		{
			object->setProperty(Identifiers::StaticPTPRole, grandmasterString);
			break;
		}
		case PTP::CONFIG_BMCA:
		{
			object->setProperty(Identifiers::StaticPTPRole, BMCAString);
			break;
		}
	}
}

void WorkbenchClient::setPTPDelayRequestProperties(DynamicObject* object)
{
	DynamicObject::Ptr delayRequestObject(new DynamicObject);
	int delayRequestProperty((int)ptpTree.getProperty(Identifiers::PTPDelayRequestIntervalMsec));
	if (delayRequestProperty == PTP::CONFIG_DELAY_REQUESTS_DISABLED)
	{
		delayRequestObject->setProperty(Identifiers::Enabled, false);
	}
	else
	{
		delayRequestObject->setProperty(Identifiers::Enabled, true);
		delayRequestObject->setProperty(Identifiers::Milliseconds, delayRequestProperty);
	}
	object->setProperty(Identifiers::PTPDelayRequest, (var)delayRequestObject);
}

DynamicObject* WorkbenchClient::createPTPObject(Identifier const &property, var const &parameter)
{
	DynamicObject* object = new DynamicObject;

	if (property == Identifiers::PTPInitialTime)
	{
		object->setProperty(Identifiers::PTPInitialTime, (int64)parameter);
	}
	else if (property == Identifiers::StaticPTPRole)
	{
		setPTPRoleProperty(object, parameter);
	}
	else if (property == Identifiers::PTPDelayRequestIntervalMsec)
	{
		setPTPDelayRequestProperties(object);
	}
	else
	{
		object->setProperty(property, parameter);
	}

	return object;
}
