#pragma once

//
// Contains enums for everything AVTP related
//
namespace AVTP
{
	enum 
	{
		AUDIO_STREAM_PACKET_RATE_CLASS_A = 8000,
		AUDIO_STREAM_PACKET_RATE_CLASS_C = 750,

		PROTOCOL_1722A_DRAFT_6 = 6,
		PROTOCOL_1722A_DRAFT_16 = 16,

		DEFAULT_STREAM_VLAN = 2,
		MAX_STREAM_VLAN = 4094,

		MAX_TALKER_PRESENTATION_OFFSET_MSEC = 40,
		MAX_LISTENER_PRESENTATION_OFFSET_MSEC = 40
	};
}
