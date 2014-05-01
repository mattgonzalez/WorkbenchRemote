/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

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
