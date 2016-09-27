#include "base.h"
#include "PacketFollowUp.h"

String PacketFollowUp::getFieldName(int const fieldIndex)
{
	static char const * const fieldNames[] =
	{
		"preciseOriginTimestamp",
		"tlvType",
		"lengthField",
		"organizationId",
		"organizationSubType",
		"cumulativeScaledRateOffset",
		"gmTimeBaseIndicator",
		"lastGmPhaseChange",
		"scaledLastGmFreqChange"
	};

	if (fieldIndex < preciseOriginTimestamp)
		return PacketPTP::getFieldName(fieldIndex);

	if (fieldIndex >= preciseOriginTimestamp && fieldIndex <= scaledLastGmFreqChange)
		return String(fieldNames[fieldIndex - preciseOriginTimestamp]);

	return String::empty;
}


void PacketFollowUp::getField(Field &field, int64 adapterGuid)
{
	static const FieldSizeAndOffset fields[] =
	{
		{ 80, (34 + 0) * 8,  Identifiers::PreciseOriginTimestamp }, // preciseOriginTimestamp
		{ 16, (44 + 0) * 8,  Identifiers::TlvType }, // tlvType
		{ 16, (44 + 2) * 8,  Identifiers::LengthField }, // lengthField
		{ 24, (44 + 4) * 8,  Identifiers::OrganizationId }, // organizationId
		{ 24, (44 + 7) * 8,  Identifiers::OrganizationSubType }, // organizationSubType
		{ 32, (44 + 10) * 8, Identifiers::CumulativeScaledRateOffset }, // cumulativeScaledRateOffset
		{ 16, (44 + 14) * 8, Identifiers::GmTimeBaseIndicator }, // gmTimeBaseIndicator
		{ 96, (44 + 16) * 8, Identifiers::LastGmPhaseChange }, // lastGmPhaseChange
		{ 32, (44 + 28) * 8, Identifiers::ScaledLastGmFreqChange }  // scaledLastGmFreqChange
	};

	if (field.index > scaledLastGmFreqChange)
	{
		return;
	}

	PacketPTP::getField(field, adapterGuid);

	if (field.index >= preciseOriginTimestamp && field.index <= scaledLastGmFreqChange)
	{
		int index = field.index - preciseOriginTimestamp;
		field.numBits = fields[index].numBits;
		field.offsetBits = fields[index].offsetBits;
		field.identifier = fields[index].identifier;
		field.correctValueBigEndian.setSize(field.numBits / 8, true);
	}

	switch (field.index)
	{
	case messageType:
		field.setCorrectValueU8(PacketPTP::FOLLOW_UP);
		break;

	case messageLength:
		field.setCorrectValueU16(PacketFollowUp::MESSAGE_LENGTH_VALUE);
		break;

	case control:
		field.setCorrectValueU8(PacketFollowUp::CONTROL_VALUE);
		break;

	case preciseOriginTimestamp:
		field.canCorrupt = false;
		field.hasFixedValue = false;
		break;

	case tlvType:
		field.setCorrectValueU16(ORGANIZATION_EXTENSION);
		break;

	case lengthField:
		field.setCorrectValueU16(PacketFollowUp::TLV_LENGTH_VALUE);
		break;

	case organizationId:
		field.setCorrectValueU24(ORGANIZATION_ID_HIGH, ORGANIZATION_ID_MID, ORGANIZATION_ID_LOW);
		break;

	case organizationSubType:
		field.setCorrectValueU24(ORGANIZATION_SUBTYPE_HIGH, ORGANIZATION_SUBTYPE_MID, ORGANIZATION_SUBTYPE_LOW);
		break;
	}
}

const Identifier PacketFollowUp::Identifiers::PreciseOriginTimestamp("PreciseOriginTimestamp");
const Identifier PacketFollowUp::Identifiers::TlvType("TlvType");
const Identifier PacketFollowUp::Identifiers::LengthField("LengthField");
const Identifier PacketFollowUp::Identifiers::OrganizationId("OrganizationId");
const Identifier PacketFollowUp::Identifiers::OrganizationSubType("OrganizationSubType");
const Identifier PacketFollowUp::Identifiers::CumulativeScaledRateOffset("CumulativeScaledRateOffset");
const Identifier PacketFollowUp::Identifiers::GmTimeBaseIndicator("GmTimeBaseIndicator");
const Identifier PacketFollowUp::Identifiers::LastGmPhaseChange("LastGmPhaseChange");
const Identifier PacketFollowUp::Identifiers::ScaledLastGmFreqChange("ScaledLastGmFreqChange");


