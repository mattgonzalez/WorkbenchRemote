#pragma once
#include "PTPCorruptionComponent.h"

class PTPSyncCorruptionComponent: public PTPCorruptionComponent
{
public:
	PTPSyncCorruptionComponent(ValueTree corruptionTree_, int64 adapterGuid_);
	~PTPSyncCorruptionComponent();

protected:
};
