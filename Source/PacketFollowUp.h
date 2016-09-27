#pragma once

#include "PacketPTP.h"

struct PacketFollowUp : public PacketPTP
{
	enum
	{
		preciseOriginTimestamp = logMessageInterval + 1,
		tlvType,
		lengthField,
		organizationId,
		organizationSubType,
		cumulativeScaledRateOffset,
		gmTimeBaseIndicator,
		lastGmPhaseChange,
		scaledLastGmFreqChange,

		NUM_FIELDS
	};

	enum
	{
		CONTROL_VALUE = 2,
		MESSAGE_LENGTH_VALUE = 76,
		TLV_LENGTH_VALUE = 28
	};

	enum
	{
		ORGANIZATION_EXTENSION = 3,
		ORGANIZATION_ID_LOW = 0xC2,
		ORGANIZATION_ID_MID = 0x80,
		ORGANIZATION_ID_HIGH = 0,
		ORGANIZATION_SUBTYPE_LOW = 1,
		ORGANIZATION_SUBTYPE_MID = 0,
		ORGANIZATION_SUBTYPE_HIGH = 0
	};

	struct Identifiers
	{
		static const Identifier PreciseOriginTimestamp;
		static const Identifier TlvType;
		static const Identifier LengthField;
		static const Identifier OrganizationId;
		static const Identifier OrganizationSubType;
		static const Identifier CumulativeScaledRateOffset;
		static const Identifier GmTimeBaseIndicator;
		static const Identifier LastGmPhaseChange;
		static const Identifier ScaledLastGmFreqChange;
	};

	static String getFieldName(int const fieldIndex);
	static void getField(Field &field, int64 adapterGuid);
};
