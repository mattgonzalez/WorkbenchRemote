#pragma once

namespace PTPFaultInjection
{
	enum
	{
		ONCE = 1,
		REPEAT,
		REPEAT_CONTINUOUSLY,

		MAX_FAULT_INJECTION_CYCLE_LENGTH = ONE_MILLION,
		MAX_FAULT_INJECTION_CYCLES = ONE_MILLION
	};
};
