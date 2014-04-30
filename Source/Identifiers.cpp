/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#include "base.h"
#include "Identifiers.h"

namespace Identifiers
{
#define IDENTIFIER(name) extern const Identifier name(#name);
	IDENTIFIER_LIST
#undef IDENTIFIER
}
