/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/
#pragma once

#define IDENTIFIER_LIST IDENTIFIER( Name						) \
						IDENTIFIER( StreamID					) \
						IDENTIFIER( DestinationAddress			) \
						IDENTIFIER( ChannelCount				) \
						IDENTIFIER( Subtype						) \
						IDENTIFIER( HelpText					) \
						IDENTIFIER( TimestampJump				) \
						IDENTIFIER( Enabled						) \
						IDENTIFIER( FaultInjection				) \
						IDENTIFIER( FaultLogging				) \
						IDENTIFIER( Count						) \
						IDENTIFIER( Percent						) \
						IDENTIFIER( Amount						) \
						IDENTIFIER( CorruptPackets				) \
						IDENTIFIER( SkipPackets					) \
						IDENTIFIER( DuplicatePackets			) \
						IDENTIFIER( TimestampJitter				) \
						IDENTIFIER( TrafficShapingJitter		) \
						IDENTIFIER( Talkers						) \
						IDENTIFIER( Listeners					) \
						IDENTIFIER( GetCommand					) \
						IDENTIFIER( GetResponse					) \
						IDENTIFIER( SetCommand					) \
						IDENTIFIER( SetResponse					) \
						IDENTIFIER( System						) \
						IDENTIFIER( Version						) \
						IDENTIFIER( Sequence					) \
						IDENTIFIER( Status						) \
						IDENTIFIER( Index						) \
						IDENTIFIER( Active						) \
						IDENTIFIER( PropertyChanged				) \
						IDENTIFIER( EventNotification			) \
						IDENTIFIER( Input						) \
						IDENTIFIER( Output						) \
						IDENTIFIER( DeviceName					) \
						IDENTIFIER( SampleRate					) \
						IDENTIFIER( SampleRates					) \
						IDENTIFIER( CurrentChannelCount			) \
						IDENTIFIER( MaxChannelCount				) \
						IDENTIFIER( Channel						) \
						IDENTIFIER( Mode						) \
						IDENTIFIER( Source						) \
						IDENTIFIER( GainDecibels				) \
						IDENTIFIER( Mute						) \
						IDENTIFIER( ToneFrequency				) \
						IDENTIFIER( SourceDeviceName			) \
						IDENTIFIER( SourceChannel				) \
						IDENTIFIER( AudioDevices				) \
						IDENTIFIER( AvailableAudioDevices		) \
						IDENTIFIER( CurrentAudioDevices			) \
						IDENTIFIER( Workbench					) \
						IDENTIFIER( Port						) \
						IDENTIFIER(	StaticPTPRole				) \
						IDENTIFIER( PTPSendFollowupTLV			) \
						IDENTIFIER( PTPSendAnnounce				) \
						IDENTIFIER( PTPSendSignalingFlag		) \
						IDENTIFIER( PTPDelayRequestIntervalMsec ) \
						IDENTIFIER( TalkerTimestampOffsetMsec	) \
						IDENTIFIER( ListenerTimestampOffsetMsec	) \
						IDENTIFIER( TimestampTolerancePercent	) \
						IDENTIFIER( EthernetMode				) \

namespace Identifiers
{
#define IDENTIFIER(name) extern const Identifier name;
	IDENTIFIER_LIST
#undef IDENTIFIER
};