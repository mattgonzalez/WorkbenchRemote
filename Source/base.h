/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

typedef enum
{
	AVTP_SUBTYPE_618836 = 0,
	AVTP_SUBTYPE_AUDIO = 2,
	AVTP_SUBTYPE_CRS = 5, // 0x7d,  // 5 for D6, 0x7d, for D5
	AVTP_SUBTYPE_EXPERIMENTAL = 0x7f
} StreamSubType;

const int64 ProtocolMAAP_firstStaticAddress = 0x91E0F000FE00LL;
const int64 ProtocolMAAP_lastStaticAddress = 0x91E0F000FFFFLL;

#pragma warning(disable:4100)

enum
{
	ONE_MILLION = 1000000
};

#define FAULT_INJECTION_1722 0
#define VALUE_TREE_DISPLAY 1
