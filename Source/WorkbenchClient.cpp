/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "WorkbenchClient.h"
#include "Settings.h"
#include "Identifiers.h"
#include "Strings.h"
#include "SettingsComponent.h"
#include "PacketSync.h"
#include "PTP.h"
#include "FaultInjection.h"

/*

This class uses the JUCE InterprocessConnection class to send and receive data
over a socket.  The actual data sent over the socket consists of a four byte magic number,
followed by a four byte little-endian message length count in bytes, followed by the JSON string.

For example, to transmit this 45 byte JSON string:

{"GetCommand": {"System": {}}, "Sequence": 1}

the following bytes are sent over the socket:

574f524b			Magic number 'WORK'
2d000000			Byte count (32 bits, endian swapped)
7b22476574436f6d	{"GetCom
6d616e64223a207b	mand": {
2253797374656d22	"System"
3a207b7d7d2c2022	: {}}, "
53657175656e6365	Sequence
223a20317d			": 1}

Strings are sent without a zero terminator.

*/

const String WorkbenchClient::followerString("Follower");
const String WorkbenchClient::grandmasterString("Grandmaster");
const String WorkbenchClient::BMCAString("BMCA");
const String WorkbenchClient::ethernetString("Ethernet");
const String WorkbenchClient::masterString("Master");
const String WorkbenchClient::slaveString("Slave");
const String WorkbenchClient::lockedString("Locked");
const String WorkbenchClient::unlockedString("Unlocked");
const String WorkbenchClient::onceString("Once");
const String WorkbenchClient::repeatString("Repeat");
const String WorkbenchClient::repeatContinuouslyString("Repeat Continuously");

//============================================================================
//
// Constructor and destructor
//
//============================================================================

//
// The 'KROW' value is the magic number written to the start of the packet
// that has to match at both ends of the connection
//
WorkbenchClient::WorkbenchClient(Settings* settings_):
	RemoteClient(settings_,'KROW'),
	workbenchSettingsTree(settings_->getWorkbenchSettingsTree()),
	ptpTree(settings_->getPTPTree())
{
	//DBG("WorkbenchClient::WorkbenchClient()");
	workbenchSettingsTree.addListener(this);
	ptpTree.addListener(this);
}

WorkbenchClient::~WorkbenchClient()
{
	//DBG("WorkbenchClient::~WorkbenchClient()");
}

//============================================================================
//
// Callbacks
//
//============================================================================

//---------------------------------------------------------------------------
//
// Callback indicating that the socket successfully connected
//
void WorkbenchClient::connectionMade()
{
	DBG("WorkbenchClient::connectionMade");

	RemoteClient::connectionMade();
}

//---------------------------------------------------------------------------
//
// Callback indicating that the socket has disconnected
//
void WorkbenchClient::connectionLost()
{
	DBG("WorkbenchClient::connectionLost");

	// Remove all the stream information from the settings tree
	settings->removeStreams();

	RemoteClient::connectionLost();
}

//============================================================================
//
// Commands sent to Workbench
//
//============================================================================
Result WorkbenchClient::getTalkerStreams()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree talkersTree(settings->getWorkbenchTree().getChildWithName(Identifiers::Talkers));

	arrayVar.resize(0); // force the var to become a zero-length array

	for (int i = 0; i < talkersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject);
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Talkers, arrayVar);
}

Result WorkbenchClient::getListenerStreams()
{
	ScopedLock locker(settings->lock);
	var arrayVar;
	ValueTree listenersTree(settings->getWorkbenchTree().getChildWithName(Identifiers::Listeners));

	arrayVar.resize(0); // force the var to become a zero-length array

	for (int i = 0; i < listenersTree.getNumChildren(); ++i)
	{
		DynamicObject::Ptr indexObject(new DynamicObject);
		indexObject->setProperty(Identifiers::Index, i);
		arrayVar.append(var(indexObject));
	}

	return getProperty(Identifiers::Listeners, arrayVar);
}

Result WorkbenchClient::getLinkState()
{
	return getProperty(Identifiers::LinkState, new DynamicObject);
}

Result WorkbenchClient::getSettings()
{
	return getProperty(Identifiers::WorkbenchSettings, new DynamicObject);
}

Result WorkbenchClient::getPTP()
{
	return getProperty(Identifiers::PTP, new DynamicObject);
}

Result WorkbenchClient::setStreamProperty( Identifier const type, int const streamIndex, Identifier const &ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);
	var arrayVar;
	DynamicObject::Ptr streamObject(new DynamicObject);

	arrayVar.resize(0); // force the var to become a zero-length array

	streamObject->setProperty(Identifiers::Index, streamIndex);
	streamObject->setProperty(ID, parameter);
	arrayVar.append(var(streamObject));
	commandObject->setProperty(type, arrayVar);
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

//============================================================================
//
// Handle responses from Workbench
//
//============================================================================

void WorkbenchClient::handlePropertyChangedMessage(DynamicObject * messageObject, Identifier const expectedMessage)
{
	var propertyVar(messageObject->getProperty(expectedMessage));
	DynamicObject * propertyObject = propertyVar.getDynamicObject();

	if (nullptr == propertyObject)
	{
		DBG("propertyObject is null");
		return;
	}

	if (propertyObject->hasProperty(Identifiers::System))
	{
		var systemPropertyVar(propertyObject->getProperty(Identifiers::System));
		DynamicObject *systemPropertyObject = systemPropertyVar.getDynamicObject();

		if (nullptr == systemPropertyObject)
		{
			DBG("Could not parse get system response");
			return;
		}

		handleGetSystemResponse(systemPropertyObject);
		return;
	}

	if (propertyObject->hasProperty(Identifiers::Talkers))
	{
		var talkersPropertyVar(propertyObject->getProperty(Identifiers::Talkers));
		if (false == talkersPropertyVar.isArray())
		{
			DBG("Could not parse get talkers response");
			return;
		}
		handleGetStreamsResponse(talkersPropertyVar, settings->getWorkbenchTree().getChildWithName(Identifiers::Talkers));
		return;
	}

	if (propertyObject->hasProperty(Identifiers::Listeners))
	{
		var listenersPropertyVar(propertyObject->getProperty(Identifiers::Listeners));
		if (false == listenersPropertyVar.isArray())
		{
			DBG("Could not parse get listeners response");
			return;
		}
		handleGetStreamsResponse(listenersPropertyVar, settings->getWorkbenchTree().getChildWithName(Identifiers::Listeners));
		return;
	}

	if (propertyObject->hasProperty(Identifiers::LinkState))
	{
		var linkStatePropertyVar(propertyObject->getProperty(Identifiers::LinkState));
		DynamicObject* linkStatePropertyObject = linkStatePropertyVar.getDynamicObject();
		if (nullptr == linkStatePropertyObject)
		{
			DBG("Could not parse get linkstate response");
			return;
		}
		handleGetLinkStateResponse(linkStatePropertyObject);
	}

	if (propertyObject->hasProperty(Identifiers::WorkbenchSettings))
	{
		var workbenchSettingsPropertyVar(propertyObject->getProperty(Identifiers::WorkbenchSettings));
		DynamicObject* workbenchSettingsPropertyObject = workbenchSettingsPropertyVar.getDynamicObject();
		if (nullptr == workbenchSettingsPropertyObject)
		{
			DBG("Could not parse get settings response");
		}
		handleGetWorkbenchSettingsResponse(workbenchSettingsPropertyObject);
		return;
	}

	if (propertyObject->hasProperty(Identifiers::PTP))
	{
		var ptpPropertyVar(propertyObject->getProperty(Identifiers::PTP));
		DynamicObject* ptpPropertyObject = ptpPropertyVar.getDynamicObject();
		if (nullptr == ptpPropertyObject)
		{
			DBG("Could not parse get PTP response");
		}
		handleGetPTPResponse(ptpPropertyObject);
		return;
	}
}

void WorkbenchClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{
	int numTalkers = systemPropertyObject->getProperty(Identifiers::Talkers);
	int numListeners = systemPropertyObject->getProperty(Identifiers::Listeners);

	if (systemPropertyObject->hasProperty(Identifiers::BroadRReachSupported))
	{
		workbenchSettingsTree.setProperty(Identifiers::BroadRReachSupported, systemPropertyObject->getProperty(Identifiers::BroadRReachSupported), nullptr);
	}

	settings->initializeStreams(numTalkers, numListeners);
}

void WorkbenchClient::handleGetStreamsResponse( var streamsPropertyVar, ValueTree streamsTree )
{
	ScopedLock locker(settings->lock);

	for (int responseIndex = 0; responseIndex < streamsPropertyVar.size(); ++responseIndex)
	{
		//
		// Get the stream index from the response...
		//
		var const &v(streamsPropertyVar[responseIndex]);
		DynamicObject::Ptr const d(v.getDynamicObject());

		if (nullptr == d || false == d->hasProperty(Identifiers::Index))
		{
			DBG("Invalid response for get talkers/listeners");
			continue;
		}

		int streamIndex = d->getProperty(Identifiers::Index);
		//DBG(streamIndex);

		//
		// Get the values for the stream from the response and put them into the tree
		//
		ValueTree streamTree(streamsTree.getChild(streamIndex));
		if (false == streamTree.isValid())
		{
			DBG("Invalid stream index for get talkers/listeners");
			continue;
		}

		if (d->hasProperty(Identifiers::Name))
		{
			streamTree.setProperty(Identifiers::Name, d->getProperty(Identifiers::Name), nullptr);
		}

		if (d->hasProperty(Identifiers::StreamID))
		{
			int64 streamID = d->getProperty(Identifiers::StreamID).toString().getHexValue64();
			streamTree.setProperty(Identifiers::StreamID, streamID, nullptr);
		}

		if (d->hasProperty(Identifiers::DestinationAddress))
		{
			String addressString(d->getProperty(Identifiers::DestinationAddress).toString());
			addressString = addressString.toLowerCase();
			addressString.retainCharacters(Strings::hexChars);
			streamTree.setProperty(Identifiers::DestinationAddress, addressString.getHexValue64(), nullptr);
		}

		if (d->hasProperty(Identifiers::Subtype))
		{
			streamTree.setProperty(Identifiers::Subtype, (int) d->getProperty(Identifiers::Subtype), nullptr);
		}

		if (d->hasProperty(Identifiers::ChannelCount))
		{
			streamTree.setProperty(Identifiers::ChannelCount, (int) d->getProperty(Identifiers::ChannelCount), nullptr);
		}

		if (d->hasProperty(Identifiers::Active))
		{
			streamTree.setProperty(Identifiers::Active, d->getProperty(Identifiers::Active), nullptr);
		}

		if (d->hasProperty(Identifiers::AutoStart))
		{
			streamTree.setProperty(Identifiers::AutoStart, d->getProperty(Identifiers::AutoStart), nullptr);
		}

		if (d->hasProperty(Identifiers::FaultInjection) && Identifiers::Talkers == streamsTree.getType())
		{
			ValueTree faultTree(streamTree.getChildWithName(Identifiers::FaultInjection));
			var const& faultVar(d->getProperty(Identifiers::FaultInjection));
			DynamicObject::Ptr const faultObject(faultVar.getDynamicObject());

			if (faultObject->hasProperty(Identifiers::CorruptPackets))
			{
				ValueTree corruptTree(faultTree.getChildWithName(Identifiers::CorruptPackets));
				var const& corruptVar(faultObject->getProperty(Identifiers::CorruptPackets));
				DynamicObject::Ptr const corruptObject(corruptVar.getDynamicObject());
				if (corruptObject->hasProperty(Identifiers::Enabled))
				{
					corruptTree.setProperty(Identifiers::Enabled, corruptObject->getProperty(Identifiers::Enabled), nullptr);
				}

				if (corruptObject->hasProperty(Identifiers::Percent))
				{
					corruptTree.setProperty(Identifiers::Percent, corruptObject->getProperty(Identifiers::Percent), nullptr);
				}
			}
		}
	}
}

void WorkbenchClient::handleFaultNotificationMessage( DynamicObject * messageObject )
{
}

void WorkbenchClient::handleGetLinkStateResponse( DynamicObject* linkStatePropertyObject )
{
	ScopedLock locker(settings->lock);

	ValueTree linkStateTree(settings->getLinkStateTree());

	if (linkStatePropertyObject->hasProperty(Identifiers::ConnectState))
	{
		linkStateTree.setProperty(Identifiers::ConnectState, linkStatePropertyObject->getProperty(Identifiers::ConnectState), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::DuplexState))
	{
		linkStateTree.setProperty(Identifiers::DuplexState, linkStatePropertyObject->getProperty(Identifiers::DuplexState), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::TransmitSpeed))
	{
		linkStateTree.setProperty(Identifiers::TransmitSpeed, linkStatePropertyObject->getProperty(Identifiers::TransmitSpeed), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::ReceiveSpeed))
	{
		linkStateTree.setProperty(Identifiers::ReceiveSpeed, linkStatePropertyObject->getProperty(Identifiers::ReceiveSpeed), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::AutoNegotiation))
	{
		linkStateTree.setProperty(Identifiers::AutoNegotiation, linkStatePropertyObject->getProperty(Identifiers::AutoNegotiation), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::EthernetMode))
	{
		if (linkStatePropertyObject->getProperty(Identifiers::EthernetMode).toString() == ethernetString)
		{
			linkStateTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
		}
		linkStateTree.setProperty(Identifiers::EthernetMode, linkStatePropertyObject->getProperty(Identifiers::EthernetMode), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::BroadRReachMode))
	{
		var property(linkStatePropertyObject->getProperty(Identifiers::BroadRReachMode));
		if (property.toString() == masterString)
		{
			linkStateTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
		}
		if (property.toString() == slaveString)
		{
			linkStateTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE, nullptr);
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE, nullptr);
		}
	}
}

void WorkbenchClient::handleGetWorkbenchSettingsResponse( DynamicObject* workbenchSettingsPropertyObject )
{
	ScopedLock locker(settings->lock);

	handleGetPTPResponse(workbenchSettingsPropertyObject);

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::TalkerPresentationOffsetMsec))
	{
		workbenchSettingsTree.setProperty(Identifiers::TalkerPresentationOffsetMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::TalkerPresentationOffsetMsec), nullptr);
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::ListenerPresentationOffsetMsec))
	{
		workbenchSettingsTree.setProperty(Identifiers::ListenerPresentationOffsetMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::ListenerPresentationOffsetMsec), nullptr);
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::TimestampTolerancePercent))
	{
		workbenchSettingsTree.setProperty(Identifiers::TimestampTolerancePercent, workbenchSettingsPropertyObject->getProperty(Identifiers::TimestampTolerancePercent), nullptr);
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::EthernetMode))
	{
		if (workbenchSettingsPropertyObject->getProperty(Identifiers::EthernetMode).toString() == ethernetString)
		{
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
		}
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::BroadRReachMode))
	{
		var property(workbenchSettingsPropertyObject->getProperty(Identifiers::BroadRReachMode));
		if (property.toString() == masterString)
		{
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
		}
		if (property.toString() == slaveString)
		{
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE, nullptr);
		}
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::SpdifLocked))
	{
		bool spdifLocked(workbenchSettingsPropertyObject->getProperty(Identifiers::SpdifLocked));

		if (spdifLocked)
		{
			workbenchSettingsTree.setProperty(Identifiers::SpdifLocked, lockedString, nullptr);
		}
		else
		{
			workbenchSettingsTree.setProperty(Identifiers::SpdifLocked, unlockedString, nullptr);
		}
	}
}

void WorkbenchClient::handleGetPTPResponse(DynamicObject* ptpPropertyObject)
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
					ptpTree.setProperty(Identifiers::PTPDelayRequest, delayRequestObject->getProperty(Identifiers::Milliseconds), nullptr);
				}
				else
				{
					ptpTree.setProperty(Identifiers::PTPDelayRequest, PTP::CONFIG_DELAY_REQUESTS_DISABLED, nullptr);
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

Result WorkbenchClient::setSettingsProperty( Identifier const &commandProperty, Identifier const &property, var const &parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);
	DynamicObject::Ptr settingsObject(new DynamicObject);

	if (property != Identifiers::BroadRReachMode && property != Identifiers::StaticPTPRole)
	{
		settingsObject->setProperty(property, parameter);
	}

	if (property == Identifiers::StaticPTPRole)
	{
		switch ((int)parameter)
		{
		case PTP::CONFIG_FOLLOWER:
			{
				settingsObject->setProperty(Identifiers::StaticPTPRole, followerString);
				break;
			}
		case PTP::CONFIG_GRANDMASTER:
			{
				settingsObject->setProperty(Identifiers::StaticPTPRole, grandmasterString);
				break;
			}
		case PTP::CONFIG_BMCA:
			{
				settingsObject->setProperty(Identifiers::StaticPTPRole, BMCAString);
				break;
			}
		}
	}

	if (property == Identifiers::PTPSendFollowupTLV ||
		property == Identifiers::PTPSendAnnounce ||
		property == Identifiers::PTPSendSignalingFlag ||
		property == Identifiers::PTPAutomaticGMTime)
	{
		settingsObject->setProperty(property, (bool)parameter);
	}

	if (property == Identifiers::PTPDelayRequest)
	{
		DynamicObject::Ptr delayRequestObject(new DynamicObject);
		if ((int)parameter == PTP::CONFIG_DELAY_REQUESTS_DISABLED)
		{
			delayRequestObject->setProperty(Identifiers::Enabled, false);
		}
		else
		{
			delayRequestObject->setProperty(Identifiers::Enabled, true);
			delayRequestObject->setProperty(Identifiers::Milliseconds, (int)parameter);
		}
		settingsObject->setProperty(Identifiers::PTPDelayRequest, var(delayRequestObject));
	}

	if (property == Identifiers::BroadRReachMode)
	{
		if ((int)parameter == Settings::ANALYZERBR_USB_ETHERNET_MODE_STANDARD)
		{
			settingsObject->setProperty(Identifiers::EthernetMode, ethernetString);
		}
		if ((int)parameter == Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER)
		{
			settingsObject->setProperty(Identifiers::BroadRReachMode, masterString);
		}
		if ((int)parameter == Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE)
		{
			settingsObject->setProperty(Identifiers::BroadRReachMode, slaveString);
		}
	}

	if (property == Identifiers::PTPInitialTime)
	{
		settingsObject->setProperty(Identifiers::PTPInitialTime, (int64)parameter);
	}

	commandObject->setProperty(commandProperty, var(settingsObject));
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

Result WorkbenchClient::setPTPFaultInjectionProperty(Identifier const &property)
{
	DynamicObject::Ptr faultInjectionObject(new DynamicObject);
	ValueTree faultInjectionTree(ptpTree.getChildWithName(Identifiers::FaultInjection));

	faultInjectionObject->setProperty(property, faultInjectionTree[property]);

	DynamicObject::Ptr ptpObject(new DynamicObject);
	ptpObject->setProperty(Identifiers::FaultInjection, var(faultInjectionObject));

	DynamicObject::Ptr commandObject(new DynamicObject); 
	commandObject->setProperty(Identifiers::PTP, var(ptpObject));
	DynamicObject messageObject;
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

Result WorkbenchClient::setPTPCorruptPacketField(ValueTree &corruptFieldTree)
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

void WorkbenchClient::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (treeWhosePropertyHasChanged.getType() == Identifiers::WorkbenchSettings)
	{
		setSettingsProperty(Identifiers::WorkbenchSettings, property, treeWhosePropertyHasChanged.getProperty(property));
		return;
	}

	if (treeWhosePropertyHasChanged.getType() == Identifiers::PTP)
	{
		setSettingsProperty(Identifiers::PTP, property, treeWhosePropertyHasChanged.getProperty(property));
		return;
	}

	if (treeWhosePropertyHasChanged.getType() == Identifiers::FaultInjection)
	{
		setPTPFaultInjectionProperty(property);
		return;
	}

	if (treeWhosePropertyHasChanged.getParent().getType() == Identifiers::CorruptPackets)
	{
		setPTPCorruptPacketField(treeWhosePropertyHasChanged);
		return;
	}
}

void WorkbenchClient::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void WorkbenchClient::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int )
{
}

void WorkbenchClient::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved, int, int )
{
}

void WorkbenchClient::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}