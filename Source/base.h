#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

typedef enum
{
	AVTP_SUBTYPE_618836 = 0,
	AVTP_SUBTYPE_AUDIO = 2,
	AVTP_SUBTYPE_CRS = 0x7d,
	AVTP_SUBTYPE_EXPERIMENTAL = 0x7f
} StreamSubType;

const int64 ProtocolMAAP_firstStaticAddress = 0x91E0F000FE00LL;
const int64 ProtocolMAAP_lastStaticAddress = 0x91E0F000FFFFLL;

#pragma warning(disable:4100)
