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
	workbenchSettingsTree(settings_->getWorkbenchSettingsTree())
{
	//DBG("WorkbenchClient::WorkbenchClient()");
	workbenchSettingsTree.addListener(this);
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
	ValueTree talkersTree(settings->getStreamsTree().getChildWithName(Identifiers::Talkers));

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
	ValueTree listenersTree(settings->getStreamsTree().getChildWithName(Identifiers::Listeners));

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
		handleGetStreamsResponse(talkersPropertyVar, settings->getStreamsTree().getChildWithName(Identifiers::Talkers));
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
		handleGetStreamsResponse(listenersPropertyVar, settings->getStreamsTree().getChildWithName(Identifiers::Listeners));
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
}

void WorkbenchClient::handleGetSystemResponse( DynamicObject * systemPropertyObject )
{
	int numTalkers = systemPropertyObject->getProperty(Identifiers::Talkers);
	int numListeners = systemPropertyObject->getProperty(Identifiers::Listeners);
	ValueTree workbenchSettingsTree(settings->getWorkbenchSettingsTree());

	if (systemPropertyObject->hasProperty(Identifiers::BroadRReachSupported))
	{
        clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachSupported, systemPropertyObject->getProperty(Identifiers::BroadRReachSupported));
		//workbenchSettingsTree.setProperty(Identifiers::BroadRReachSupported, systemPropertyObject->getProperty(Identifiers::BroadRReachSupported), nullptr);
	}

	settings->initializeStreams(numTalkers, numListeners);
    remoteCacheTree= settings->getStreamsTree().createCopy();
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

		if (d->hasProperty(Identifiers::Name))
		{
			clonePropertyChange(streamsTree, Identifiers::Name, d->getProperty(Identifiers::Name), streamIndex);
			//streamTree.setProperty(Identifiers::Name, d->getProperty(Identifiers::Name), nullptr);
		}

		if (d->hasProperty(Identifiers::StreamID))
		{
			int64 streamID = d->getProperty(Identifiers::StreamID).toString().getHexValue64();
			clonePropertyChange(streamsTree, Identifiers::StreamID, streamID, streamIndex);
			//streamTree.setProperty(Identifiers::StreamID, streamID, nullptr);
		}

		if (d->hasProperty(Identifiers::DestinationAddress))
		{
			String addressString(d->getProperty(Identifiers::DestinationAddress).toString());
			addressString = addressString.toLowerCase();
			addressString.retainCharacters(Strings::hexChars);
			clonePropertyChange(streamsTree, Identifiers::DestinationAddress, addressString.getHexValue64(), streamIndex);
			//streamTree.setProperty(Identifiers::DestinationAddress, addressString.getHexValue64(), nullptr);
		}

		if (d->hasProperty(Identifiers::Subtype))
		{
			clonePropertyChange(streamsTree, Identifiers::Subtype, d->getProperty(Identifiers::Subtype), streamIndex);
			//streamTree.setProperty(Identifiers::Subtype, (int) d->getProperty(Identifiers::Subtype), nullptr);
		}

		if (d->hasProperty(Identifiers::ChannelCount))
		{
			clonePropertyChange(streamsTree, Identifiers::ChannelCount, d->getProperty(Identifiers::ChannelCount), streamIndex);
			//streamTree.setProperty(Identifiers::ChannelCount, (int) d->getProperty(Identifiers::ChannelCount), nullptr);
		}

		if (d->hasProperty(Identifiers::Active))
		{
			clonePropertyChange(streamsTree, Identifiers::Active, d->getProperty(Identifiers::Active), streamIndex);
			//streamTree.setProperty(Identifiers::Active, d->getProperty(Identifiers::Active), nullptr);
		}

		if (d->hasProperty(Identifiers::FaultInjection) && Identifiers::Talkers == streamsTree.getType())
		{
			ValueTree faultTree(streamsTree.getChildWithName(Identifiers::FaultInjection));
			var const& faultVar(d->getProperty(Identifiers::FaultInjection));
			DynamicObject::Ptr const faultObject(faultVar.getDynamicObject());

			if (faultObject->hasProperty(Identifiers::CorruptPackets))
			{
				ValueTree corruptTree(faultTree.getChildWithName(Identifiers::CorruptPackets));
				var const& corruptVar(faultObject->getProperty(Identifiers::CorruptPackets));
				DynamicObject::Ptr const corruptObject(corruptVar.getDynamicObject());
				if (corruptObject->hasProperty(Identifiers::Enabled))
				{
					clonePropertyChange(corruptTree, Identifiers::Enabled, corruptObject->getProperty(Identifiers::Enabled));
					//corruptTree.setProperty(Identifiers::Enabled, corruptObject->getProperty(Identifiers::Enabled), nullptr);
				}

				if (corruptObject->hasProperty(Identifiers::Percent))
				{
					clonePropertyChange(corruptTree, Identifiers::Percent, corruptObject->getProperty(Identifiers::Percent));
					//corruptTree.setProperty(Identifiers::Percent, corruptObject->getProperty(Identifiers::Percent), nullptr);
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
		clonePropertyChange(linkStateTree, Identifiers::ConnectState, linkStatePropertyObject->getProperty(Identifiers::ConnectState));
		//linkStateTree.setProperty(Identifiers::ConnectState, linkStatePropertyObject->getProperty(Identifiers::ConnectState), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::DuplexState))
	{
		clonePropertyChange(linkStateTree, Identifiers::DuplexState, linkStatePropertyObject->getProperty(Identifiers::DuplexState));
		//linkStateTree.setProperty(Identifiers::DuplexState, linkStatePropertyObject->getProperty(Identifiers::DuplexState), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::TransmitSpeed))
	{
		clonePropertyChange(linkStateTree, Identifiers::TransmitSpeed, linkStatePropertyObject->getProperty(Identifiers::TransmitSpeed));
		//linkStateTree.setProperty(Identifiers::TransmitSpeed, linkStatePropertyObject->getProperty(Identifiers::TransmitSpeed), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::ReceiveSpeed))
	{
		clonePropertyChange(linkStateTree, Identifiers::ReceiveSpeed, linkStatePropertyObject->getProperty(Identifiers::ReceiveSpeed));
		//linkStateTree.setProperty(Identifiers::ReceiveSpeed, linkStatePropertyObject->getProperty(Identifiers::ReceiveSpeed), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::AutoNegotiation))
	{
		clonePropertyChange(linkStateTree, Identifiers::AutoNegotiation, linkStatePropertyObject->getProperty(Identifiers::AutoNegotiation));
		//linkStateTree.setProperty(Identifiers::AutoNegotiation, linkStatePropertyObject->getProperty(Identifiers::AutoNegotiation), nullptr);
	}

	ValueTree workbenchSettingsTree(settings->getWorkbenchSettingsTree());

	if (linkStatePropertyObject->hasProperty(Identifiers::EthernetMode))
	{
		if (linkStatePropertyObject->getProperty(Identifiers::EthernetMode).toString() == "Ethernet")
		{
			clonePropertyChange(linkStateTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD);
			//linkStateTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
			clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD);
			//workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
		}
		clonePropertyChange(linkStateTree, Identifiers::EthernetMode, linkStatePropertyObject->getProperty(Identifiers::EthernetMode));
		//linkStateTree.setProperty(Identifiers::EthernetMode, linkStatePropertyObject->getProperty(Identifiers::EthernetMode), nullptr);
	}

	if (linkStatePropertyObject->hasProperty(Identifiers::BroadRReachMode))
	{
		var property(linkStatePropertyObject->getProperty(Identifiers::BroadRReachMode));
		if (property.toString() == "Master")
		{
			clonePropertyChange(linkStateTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER);
			//linkStateTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
			clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER);
			//workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
		}
		if (property.toString() == "Slave")
		{
			clonePropertyChange(linkStateTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE);
			//linkStateTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE, nullptr);
			clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE);
			//workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE, nullptr);
		}
	}
}

void WorkbenchClient::handleGetWorkbenchSettingsResponse( DynamicObject* workbenchSettingsPropertyObject )
{
	ScopedLock locker(settings->lock);

	ValueTree workbenchSettingsTree(settings->getWorkbenchSettingsTree());

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::StaticPTPRole))
	{
		var property(workbenchSettingsPropertyObject->getProperty(Identifiers::StaticPTPRole));
		if (property.toString() == "PTP Follower")
		{
			clonePropertyChange(workbenchSettingsTree, Identifiers::StaticPTPRole, SettingsComponent::CONFIG_FOLLOWER);
			//workbenchSettingsTree.setProperty(Identifiers::StaticPTPRole, SettingsComponent::CONFIG_FOLLOWER, nullptr);
		}
		if (property.toString() == "PTP Grandmaster")
		{
			clonePropertyChange(workbenchSettingsTree, Identifiers::StaticPTPRole, SettingsComponent::CONFIG_GRANDMASTER);
			//workbenchSettingsTree.setProperty(Identifiers::StaticPTPRole, SettingsComponent::CONFIG_GRANDMASTER, nullptr);
		}
		if (property.toString() == "Use best master clock algorithm (BMCA)")
		{
			clonePropertyChange(workbenchSettingsTree, Identifiers::StaticPTPRole, SettingsComponent::CONFIG_BMCA);
			//workbenchSettingsTree.setProperty(Identifiers::StaticPTPRole, SettingsComponent::CONFIG_BMCA, nullptr);
		}
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::PTPSendFollowupTLV))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::PTPSendFollowupTLV, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPSendFollowupTLV));
		//workbenchSettingsTree.setProperty(Identifiers::PTPSendFollowupTLV, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPSendFollowupTLV), nullptr);
	}
	
	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::PTPSendAnnounce))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::PTPSendAnnounce, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPSendAnnounce));
		//workbenchSettingsTree.setProperty(Identifiers::PTPSendAnnounce, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPSendAnnounce), nullptr);
	}
	
	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::PTPSendSignalingFlag))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::PTPSendSignalingFlag, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPSendSignalingFlag));
		//workbenchSettingsTree.setProperty(Identifiers::PTPSendSignalingFlag, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPSendSignalingFlag), nullptr);
	}
	
	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::PTPDelayRequestIntervalMsec))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::PTPDelayRequestIntervalMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPDelayRequestIntervalMsec));
		//workbenchSettingsTree.setProperty(Identifiers::PTPDelayRequestIntervalMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::PTPDelayRequestIntervalMsec), nullptr);
	}
	
	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::TalkerPresentationOffsetMsec))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::TalkerPresentationOffsetMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::TalkerPresentationOffsetMsec));
		//workbenchSettingsTree.setProperty(Identifiers::TalkerPresentationOffsetMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::TalkerPresentationOffsetMsec), nullptr);
	}
	
	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::ListenerPresentationOffsetMsec))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::ListenerPresentationOffsetMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::ListenerPresentationOffsetMsec));
		//workbenchSettingsTree.setProperty(Identifiers::ListenerPresentationOffsetMsec, workbenchSettingsPropertyObject->getProperty(Identifiers::ListenerPresentationOffsetMsec), nullptr);
	}
	
	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::TimestampTolerancePercent))
	{
		clonePropertyChange(workbenchSettingsTree, Identifiers::TimestampTolerancePercent, workbenchSettingsPropertyObject->getProperty(Identifiers::TimestampTolerancePercent));
		//workbenchSettingsTree.setProperty(Identifiers::TimestampTolerancePercent, workbenchSettingsPropertyObject->getProperty(Identifiers::TimestampTolerancePercent), nullptr);
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::EthernetMode))
	{
		if (workbenchSettingsPropertyObject->getProperty(Identifiers::EthernetMode).toString() == "Ethernet")
		{
			clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD);
			workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD, nullptr);
		}
	}

	if (workbenchSettingsPropertyObject->hasProperty(Identifiers::BroadRReachMode))
	{
		var property(workbenchSettingsPropertyObject->getProperty(Identifiers::BroadRReachMode));
		if (property.toString() == "Master")
		{
			clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER);
			//workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
		}
		if (property.toString() == "Slave")
		{
			clonePropertyChange(workbenchSettingsTree, Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE);
			//workbenchSettingsTree.setProperty(Identifiers::BroadRReachMode, SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER, nullptr);
		}
	}
}

Result WorkbenchClient::setSettingsProperty( Identifier const & ID, var const parameter )
{
	DynamicObject messageObject;
	DynamicObject::Ptr commandObject(new DynamicObject);
	DynamicObject::Ptr settingsObject(new DynamicObject);

	if (ID == Identifiers::StaticPTPRole)
	{
		switch ((int)parameter)
		{
		case SettingsComponent::CONFIG_FOLLOWER:
			{
				settingsObject->setProperty(Identifiers::StaticPTPRole, "PTP Follower");
				break;
			}
		case SettingsComponent::CONFIG_GRANDMASTER:
			{
				settingsObject->setProperty(Identifiers::StaticPTPRole, "PTP Grandmaster");
				break;
			}
		case SettingsComponent::CONFIG_BMCA:
			{
				settingsObject->setProperty(Identifiers::StaticPTPRole, "Use best master clock algorithm (BMCA)");
				break;
			}
		}
	}

	if (ID == Identifiers::PTPSendFollowupTLV)
	{
		settingsObject->setProperty(Identifiers::PTPSendFollowupTLV, (bool)parameter);
	}

	if ( ID == Identifiers::PTPSendAnnounce)
	{
		settingsObject->setProperty(Identifiers::PTPSendAnnounce, (bool)parameter);
	}
	
	if (ID == Identifiers::PTPSendSignalingFlag)
	{
		settingsObject->setProperty(Identifiers::PTPSendSignalingFlag, (bool)parameter);
	}
	

	if (ID == Identifiers::BroadRReachMode)
	{
		if ((int)parameter == SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_STANDARD)
		{
			settingsObject->setProperty(Identifiers::EthernetMode, "Ethernet");
		}
		if ((int)parameter == SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER)
		{
			settingsObject->setProperty(Identifiers::BroadRReachMode, "Master");
		}
		if ((int)parameter == SettingsComponent::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE)
		{
			settingsObject->setProperty(Identifiers::BroadRReachMode, "Slave");
		}
	}

	settingsObject->setProperty(ID, parameter);
	commandObject->setProperty(Identifiers::WorkbenchSettings, var(settingsObject));
	messageObject.setProperty(Identifiers::SetCommand, var(commandObject));
	messageObject.setProperty(Identifiers::Sequence, commandSequence++);

	return sendJSONToSocket(messageObject);
}

void WorkbenchClient::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	if (treeWhosePropertyHasChanged.getType() == Identifiers::WorkbenchSettings)
	{
        ValueTree cacheTree(remoteCacheTree.getChildWithName(treeWhosePropertyHasChanged.getType()));
        if (cacheTree.getProperty(property) == treeWhosePropertyHasChanged.getProperty(property))
        {
            return;
        }

		if (property == Identifiers::StaticPTPRole)
		{
            var parameter(treeWhosePropertyHasChanged.getProperty(Identifiers::StaticPTPRole));
			if ((int)parameter == SettingsComponent::CONFIG_FOLLOWER)
			{
				cacheTree.setProperty(property, "PTP Follower", nullptr);
				setSettingsProperty(property, "PTP Follower");
			}
			if ((int)parameter == SettingsComponent::CONFIG_GRANDMASTER)
			{
				cacheTree.setProperty(property, "PTP Grandmaster", nullptr);
				setSettingsProperty(property, "PTP Grandmaster");
			}
			if ((int)parameter == SettingsComponent::CONFIG_BMCA)
			{
				cacheTree.setProperty(property, "Use best master clock algorithm (BMCA)", nullptr);
				setSettingsProperty(property, "Use best master clock algorithm (BMCA)");
			}
		}
        cacheTree.setProperty(property, treeWhosePropertyHasChanged.getProperty(property), nullptr);
		setSettingsProperty(property, treeWhosePropertyHasChanged.getProperty(property));
		return;
	}
}

void WorkbenchClient::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void WorkbenchClient::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void WorkbenchClient::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void WorkbenchClient::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

void WorkbenchClient::clonePropertyChange( ValueTree& mainTree, Identifier id, var property, int index )
{
    ValueTree cacheTree;

    //
    // For fault injection tree changes
    //
    if (mainTree.getType() == Identifiers::CorruptPackets)
    {
        cacheTree = remoteCacheTree.getChildWithName(Identifiers::Talkers);
        cacheTree = cacheTree.getChildWithName(Identifiers::FaultInjection).getChildWithName(Identifiers::CorruptPackets);

        cacheTree.setProperty(id, property, nullptr);
        mainTree.setProperty(id, property, nullptr);
        return;
    }

	cacheTree = remoteCacheTree.getChildWithName(mainTree.getType());
        
    //
    // For talker/listener tree changes
    //
    if (mainTree.getType() == Identifiers::Talkers || cacheTree.getType() == Identifiers::Listeners)
    {
		if (index < 0)
		{
			DBG("Invalid index: " + index);
			return;
		}

		ValueTree streamTree(mainTree.getChild(index));
		if (false == mainTree.isValid())
		{
			DBG("Invalid stream index for get talkers/listeners");
			return;
		}
		
        cacheTree = cacheTree.getChild(index);
        
		cacheTree.setProperty(id, property, nullptr);
        mainTree.setProperty(id, property, nullptr);
        return;
    }

    //
    // For Linkstate/Workbench settings tree changes
    //
    cacheTree.setProperty(id, property, nullptr);
    mainTree.setProperty(id, property, nullptr);
}
