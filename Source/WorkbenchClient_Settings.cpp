#include "base.h"
#include "WorkbenchClient.h"
#include "Identifiers.h"
#include "Settings.h"

void WorkbenchClient::handleGetWorkbenchSettingsResponse(DynamicObject* workbenchSettingsPropertyObject)
{
	ScopedLock locker(settings->lock);

#if 0
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
#endif

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

void WorkbenchClient::handleGetLinkStateResponse(DynamicObject* linkStatePropertyObject)
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

DynamicObject* WorkbenchClient::createSettingsObject(Identifier const &property, var const &parameter)
{
	DynamicObject* object = new DynamicObject;

	if (property == Identifiers::BroadRReachMode ||
		property == Identifiers::EthernetMode)
	{
		switch ((int)parameter)
		{
		case Settings::ANALYZERBR_USB_ETHERNET_MODE_STANDARD:
			object->setProperty(Identifiers::EthernetMode, ethernetString);
			break;

		case Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_MASTER:
			object->setProperty(Identifiers::BroadRReachMode, masterString);
			break;

		case Settings::ANALYZERBR_USB_ETHERNET_MODE_BR_SLAVE:
			object->setProperty(Identifiers::BroadRReachMode, slaveString);
			break;
		}
	}
	else
	{
		object->setProperty(property, parameter);
	}

	return object;
}
