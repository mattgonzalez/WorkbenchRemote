#include "base.h"
#include "PacketPTP.h"
#include "PacketSync.h"
#include "PTPSyncCorruptionComponent.h"

PTPSyncCorruptionComponent::PTPSyncCorruptionComponent(ValueTree corruptionTree_, int64 adapterGuid_) :
	PTPCorruptionComponent(corruptionTree_, adapterGuid_)
{
	titleLabel.setText("Sync field corruption", dontSendNotification);

	for (int fieldIndex = 0; fieldIndex < PacketSync::NUM_FIELDS; ++fieldIndex)
	{
		String fieldName(PacketSync::getFieldName(fieldIndex));
		PacketPTP::Field field;
		field.index = fieldIndex;
		PacketSync::getField(field, adapterGuid_);
		addFieldComponent(fieldName,field);
	}
}

PTPSyncCorruptionComponent::~PTPSyncCorruptionComponent()
{
}
