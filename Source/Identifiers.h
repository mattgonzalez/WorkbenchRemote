/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/
#pragma once

#define IDENTIFIER_LIST IDENTIFIER( Name					) \
						IDENTIFIER( StreamID				) \
						IDENTIFIER( DestinationAddress		) \
						IDENTIFIER( ChannelCount			) \
						IDENTIFIER( Subtype					) \
						IDENTIFIER( HelpText				) \
						IDENTIFIER( TimestampJump			) \
						IDENTIFIER( Enabled					) \
						IDENTIFIER( FaultInjection			) \
						IDENTIFIER( FaultLogging			) \
						IDENTIFIER( Count					) \
						IDENTIFIER( Percent					) \
						IDENTIFIER( Amount					) \
						IDENTIFIER( CorruptPackets			) \
						IDENTIFIER( SkipPackets				) \
						IDENTIFIER( DuplicatePackets		) \
						IDENTIFIER( TimestampJitter			) \
						IDENTIFIER( TrafficShapingJitter	) \
						IDENTIFIER( Talkers					) \
						IDENTIFIER( Listeners				) \
						IDENTIFIER( GetCommand				) \
						IDENTIFIER( GetResponse				) \
						IDENTIFIER( SetCommand				) \
						IDENTIFIER( SetResponse				) \
						IDENTIFIER( System					) \
						IDENTIFIER( Version					) \
						IDENTIFIER( Sequence				) \
						IDENTIFIER( Status					) \
						IDENTIFIER( Index					) \
						IDENTIFIER( Active					) \
						IDENTIFIER( PropertyChanged			) \
						IDENTIFIER( FaultNotification		) \
						IDENTIFIER( Input					) \
						IDENTIFIER( Output					) \
						IDENTIFIER( DeviceName				) \
						IDENTIFIER( SampleRate				) \
						IDENTIFIER( CurrentChannelCount		) \
						IDENTIFIER( MaxChannelCount			) \
						IDENTIFIER( Mode					) \
						IDENTIFIER( Source					) \
						IDENTIFIER( GainDecibels			) \
						IDENTIFIER( Mute					) \
						IDENTIFIER( ToneFrequency			) \
						IDENTIFIER( SourceDeviceName		) \
						IDENTIFIER( SourceDeviceIndex		) \
						IDENTIFIER( SourceChannel			)

namespace Identifiers
{
#define IDENTIFIER(name) extern const Identifier name;
	IDENTIFIER_LIST
#undef IDENTIFIER
};