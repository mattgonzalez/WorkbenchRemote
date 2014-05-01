/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/
#pragma once

int64 MACAddressToBigEndian64(MACAddress const &address);
MACAddress Int64ToMACAddress(int64 const integer_value);
MACAddress MACAddressOffsetLow16(MACAddress const &base, uint16 offset);
int64 MACAddressToLittleEndian64(MACAddress const & address);
