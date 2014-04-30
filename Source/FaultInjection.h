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

	extern const Type types[NUM_TYPES];
};
