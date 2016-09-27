/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#include "base.h"
#include "PacketPTP.h"

String PacketPTP::getFieldName( int const fieldIndex )
{
	static char const * const fieldNames[NUM_HEADER_FIELDS] =
	{
		"messageType",
		"transportSpecific",
		"versionPTP",
		"reserved",
		"messageLength",
		"domainNumber",
		"reserved",
		"flags",
		"correctionField",
		"reserved",
		"sourcePortIdentity",
		"sequenceID",
		"control",
		"logMessageInterval"
	};

	if (fieldIndex >= 0 && fieldIndex <= logMessageInterval)
		return String(fieldNames[fieldIndex]);

	return String::empty;
}

void PacketPTP::getField( Field &field, int64 adapterGuid )
{
	static const FieldSizeAndOffset fields[NUM_HEADER_FIELDS] =
	{
		{ 4,  0,   Identifiers::MessageType        }, // messageType
		{ 4,  4,   Identifiers::TransportSpecific  }, // transportSpecific
		{ 4,  8,   Identifiers::VersionPTP         }, // versionPTP
		{ 4,  12,  Identifiers::Reserved1          }, // reserved1
		{ 16, 16,  Identifiers::MessageLength      }, // messageLength
		{ 8,  32,  Identifiers::DomainNumber       }, // domainNumber
		{ 8,  40,  Identifiers::Reserved2          }, // reserved2 
		{ 16, 48,  Identifiers::Flags              }, // flags
		{ 64, 64,  Identifiers::CorrectionField    }, // correctionField
		{ 32, 128, Identifiers::Reserved3          }, // reserved3
		{ 80, 160, Identifiers::SourcePortIdentity }, // sourcePortIdentity
		{ 16, 240, Identifiers::SequenceID         }, // sequenceId,
		{ 8,  256, Identifiers::Control            }, // control
		{ 8,  264, Identifiers::LogMessageInterval }  // logMessageInterval
	};

	field.canCorrupt = true;
	field.hasFixedValue = true;

	if (field.index < 0 || field.index > logMessageInterval)
	{
		return;
	}

	field.numBits = fields[field.index].numBits;
	field.offsetBits = fields[field.index].offsetBits;
	field.identifier = fields[field.index].identifier;
	field.correctValueBigEndian.setSize((field.numBits + 7) / 8, true);

	switch (field.index)
	{
	case transportSpecific:
		field.setCorrectValueU8(TRANSPORT_SPECIFIC);
		break;

	case versionPTP:
		field.setCorrectValueU8(VERSION_PTP);
		break;

	case domainNumber:
		field.setCorrectValueU8(DOMAIN_NUMBER);
		break;

	case flags:
		field.setCorrectValueU16(DEFAULT_FLAGS);
		break;

	case sourcePortIdentity:
		{
			field.correctValueBigEndian.copyFrom( &adapterGuid, 0, sizeof(adapterGuid));
			
			uint16 temp16 = juce::ByteOrder::swapIfLittleEndian((uint16)PORT);
			field.correctValueBigEndian.copyFrom( &temp16, sizeof(adapterGuid), sizeof(uint16));
		}
		break;

	case sequenceID:
	case logMessageInterval:
		field.hasFixedValue = false;
		field.canCorrupt = false;
		break;
	}
}

void PacketPTP::Field::setCorrectValueU8( uint8 value )
{
	correctValueBigEndian.setSize(1);
	correctValueBigEndian[0] = value;
}

void PacketPTP::Field::setCorrectValueU16( uint16 value )
{
	correctValueBigEndian.setSize(2);
	value = juce::ByteOrder::swapIfLittleEndian(value);
	correctValueBigEndian.copyFrom(&value, 0, sizeof(value));
}

void PacketPTP::Field::setCorrectValueU24(uint8 v0, uint8 v1, uint8 v2)
{
	correctValueBigEndian.setSize(3);

	correctValueBigEndian[0] = v0;
	correctValueBigEndian[1] = v1;
	correctValueBigEndian[2] = v2;
}

const Identifier PacketPTP::Identifiers::MessageType("MessageType");
const Identifier PacketPTP::Identifiers::TransportSpecific("TransportSpecific");
const Identifier PacketPTP::Identifiers::VersionPTP("VersionPTP");
const Identifier PacketPTP::Identifiers::Reserved1("Reserved1");
const Identifier PacketPTP::Identifiers::MessageLength("MessageLength");
const Identifier PacketPTP::Identifiers::DomainNumber("DomainNumber");
const Identifier PacketPTP::Identifiers::Reserved2("Reserved2");
const Identifier PacketPTP::Identifiers::Flags("Flags");
const Identifier PacketPTP::Identifiers::CorrectionField("CorrectionField");
const Identifier PacketPTP::Identifiers::Reserved3("Reserved3");
const Identifier PacketPTP::Identifiers::SourcePortIdentity("SourcePortIdentity");
const Identifier PacketPTP::Identifiers::SequenceID("SequenceID");
const Identifier PacketPTP::Identifiers::Control("Control");
const Identifier PacketPTP::Identifiers::LogMessageInterval("LogMessageInterval");
