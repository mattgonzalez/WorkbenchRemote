#pragma once

#include "PacketPTP.h"

struct PacketSync : public PacketPTP
{
	enum
	{
		reserved = logMessageInterval + 1,

		NUM_FIELDS
	};

	enum
	{
		CONTROL_VALUE = 0,
		MESSAGE_LENGTH_VALUE = 44
	};

	struct Identifiers
	{
		static const Identifier Reserved;
	};

	static String getFieldName(int const fieldIndex);
	static void getField(Field &field, int64 adapterGuid);
};
