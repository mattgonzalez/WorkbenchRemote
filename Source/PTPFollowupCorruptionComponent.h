#pragma once
#include "PTPCorruptionComponent.h"

class PTPFollowupCorruptionComponent: public PTPCorruptionComponent
{
public:
	PTPFollowupCorruptionComponent(ValueTree corruptionTree_, int64 adapterGuid_);
	~PTPFollowupCorruptionComponent();

protected:
};
