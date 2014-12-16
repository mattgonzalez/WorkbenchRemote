#pragma once
#include "base.h"
//
// Contains enums for everything PTP related
//
namespace PTPInfo
{
	enum
	{
		ONCE = 1,
		REPEAT = 2,
		REPEAT_CONTINUOUSLY = 3,

		MAX_FAULT_INJECTION_CYCLE_LENGTH = ONE_MILLION ,
		MAX_FAULT_INJECTION_CYCLES = ONE_MILLION
	};
};
