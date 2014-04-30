/*
==============================================================================

Copyright (C) 2013 Echo Digital Audio Corporation.
This source code is considered to be proprietary and confidential information.

==============================================================================
*/
#include "base.h"

MACAddress Int64ToMACAddress(int64 const integer_value)
{
	int64 swapped = ByteOrder::swap( (uint64) integer_value << 16);
	return MACAddress( (const uint8 *) &swapped);
}


int64 MACAddressToBigEndian64(MACAddress const &address)
{
	return ByteOrder::swap( (uint64) (address.toInt64() << 16));
}


int64 MACAddressToLittleEndian64(MACAddress const & address)
{
	return ByteOrder::swap(uint64(address.toInt64())) >> 16;
}


MACAddress MACAddressOffsetLow16(MACAddress const &base, uint16 offset)
{
	int64 address;
	
	address = MACAddressToBigEndian64(base);
	address += offset;

	return Int64ToMACAddress (address);
}


