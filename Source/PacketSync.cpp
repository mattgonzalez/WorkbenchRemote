#include "base.h"
#include "PacketSync.h"

String PacketSync::getFieldName(int const fieldIndex)
{
	if (fieldIndex < PacketSync::reserved)
		return PacketPTP::getFieldName(fieldIndex);

	if (PacketSync::reserved == fieldIndex)
		return "reserved";

	return String::empty;
}

void PacketSync::getField(Field &field, int64 adapterGuid)
{
	if (field.index > PacketSync::reserved)
	{
		return;
	}

	PacketPTP::getField(field, adapterGuid);

	switch (field.index)
	{
	case messageType:
		field.setCorrectValueU8(PacketPTP::SYNC);
		break;

	case messageLength:
		field.setCorrectValueU16(PacketSync::MESSAGE_LENGTH_VALUE);
		break;

	case control:
		field.setCorrectValueU16(PacketSync::CONTROL_VALUE);
		break;

	case flags:
		field.setCorrectValueU16(DEFAULT_FLAGS | FLAG_TWO_STEP);
		break;

	case PacketSync::reserved:
		field.numBits = 80;
		field.offsetBits = 34 * 8;
		field.identifier = PacketSync::Identifiers::Reserved;
		field.correctValueBigEndian.setSize(field.numBits / 8, true);
		break;
	}
}

const Identifier PacketSync::Identifiers::Reserved("Reserved");
