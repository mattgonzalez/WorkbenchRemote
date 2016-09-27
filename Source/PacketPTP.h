/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#pragma once

struct PacketPTP
{
	enum
	{
		ANNOUNCE = 0xB,
		SIGNALING = 0xC,

		SYNC = 0x0,
		PDELAY_REQ = 0x2,
		PDELAY_RESP = 0x3,
		FOLLOW_UP = 0x8,
		PDELAY_RESP_FOLLOW_UP = 0xA,

		TRANSPORT_SPECIFIC = 1,
		VERSION_PTP = 2,
		DOMAIN_NUMBER = 0,
		LOG_MESSAGE_INTERVAL_RESERVED = 0x7f,
		PORT = 1,

		FLAG_LEAP61 = 1 << 0,
		FLAG_LEAP59 = 1 << 1,
		FLAG_CURRENT_UTC_OFFSET_VALID = 1 << 2,
		FLAG_TIMESCALE = 1 << 3,
		FLAG_TIME_TRACEABLE = 1 << 4,
		FLAG_FREQUENCY_TRACEABLE = 1 << 5,
		FLAG_TWO_STEP = 1 << 9,

		DEFAULT_FLAGS = FLAG_TIMESCALE
	};

	enum
	{
		messageType,
		transportSpecific,
		versionPTP,
		reserved1,
		messageLength,
		domainNumber,
		reserved2,
		flags,
		correctionField,
		reserved3,
		sourcePortIdentity,
		sequenceID,
		control,
		logMessageInterval,

		NUM_HEADER_FIELDS
	};

	struct Identifiers
	{
		static const Identifier MessageType;
		static const Identifier TransportSpecific;
		static const Identifier VersionPTP;
		static const Identifier Reserved1;
		static const Identifier MessageLength;
		static const Identifier DomainNumber;
		static const Identifier Reserved2;
		static const Identifier Flags;
		static const Identifier CorrectionField;
		static const Identifier Reserved3;
		static const Identifier SourcePortIdentity;
		static const Identifier SequenceID;
		static const Identifier Control;
		static const Identifier LogMessageInterval;
	};

	struct Field
	{
		int index;
		Identifier identifier;
		uint16 numBits;
		uint16 offsetBits;
		MemoryBlock correctValueBigEndian;
		bool hasFixedValue;
		bool canCorrupt;

	protected:
		friend struct PacketPTP;
		friend struct PacketSync;
		friend struct PacketFollowUp;

		void setCorrectValueU8(uint8 value);
		void setCorrectValueU16(uint16 value);
		void setCorrectValueU24(uint8 v0, uint8 v1, uint8 v2);
	};

	static String getFieldName(int const fieldIndex);
	static void getField(Field &field, int64 adapterGuid);

	typedef struct 
	{
		uint8 numBits;
		uint16 offsetBits;
		Identifier identifier;
	} FieldSizeAndOffset;
};
