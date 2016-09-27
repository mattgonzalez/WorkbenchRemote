#pragma once

namespace FaultInjection
{
	typedef struct 
	{
		const char *text;
		Identifier const * id;
		const char* units;
		double max;
		double step;

		const char *toggleHelpText;
		const char *percentHelpText;
		const char *amountHelpText;
	} Type;

	enum 
	{
		CORRUPT,
		DUPE,
		TIMESTAMP_JITTER,
		TIMESTAMP_JUMP,
		
		NUM_TYPES
	};

	enum
	{
		ONCE = 1,
		REPEAT = 2,
		REPEAT_CONTINUOUSLY = 3,

		MAX_FAULT_INJECTION_CYCLE_LENGTH = ONE_MILLION,
		MAX_FAULT_INJECTION_CYCLES = ONE_MILLION,
	};

	extern const Type types[NUM_TYPES];
};
