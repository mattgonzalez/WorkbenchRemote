#pragma once
#include "base.h"

//
// Contains enums for everything PTP related
//
namespace PTP
{
	enum
	{
		CONFIG_FOLLOWER = 100,
		CONFIG_GRANDMASTER,
		CONFIG_BMCA,
		CONFIG_DELAY_REQUESTS_DISABLED = -1,
		MIN_DELAY_REQUEST_INTERVAL_MILLISECONDS = 125,
		MAX_DELAY_REQUEST_INTERVAL_MILLISECONDS = 4000
	};
};
