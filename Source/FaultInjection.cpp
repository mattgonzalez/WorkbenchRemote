/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "FaultInjection.h"

//
// Not yet implemented
//

namespace Identifiers
{
	Identifier const CorruptPacket("CorruptPacket");
	Identifier const DupePackets("DupePackets");
	Identifier const TimestampJitter("TimestampJitter");
	Identifier const TimestampJump("TimestampJump");
};

namespace FaultInjection
{
	extern const Type types[NUM_TYPES] = 
	{
		{
			"Corrupt packets", &Identifiers::CorruptPacket, nullptr, 0.0, 0.0,
			"Enable intentionally sending corrupted packets for this stream", 
			"Set how often a corrupted packet will be sent", nullptr
		},

		{
			"Duplicate packets", &Identifiers::DupePackets, nullptr, 0.0, 0.0,
			"Enable intentionally duplicating packets for this stream", 
			"Set how often a stream packet will be duplicated", nullptr
		},
		{
			"Timestamp jitter", &Identifiers::TimestampJitter, nullptr, 0.0, 0.0,
			"Enable adding random jitter to AVTP timestamps", 
			"Set the maximum amount of jitter", nullptr
		},
		{
			"Timestamp jump", &Identifiers::TimestampJump, " msec", 20.0, 0.1,
			"Enable intentionally random AVTP timestamp jumps", 
			"Set how often a timestamp jump will occur", 
			"Set the maximum timestamp jump in milliseconds"
		}
	};
}
