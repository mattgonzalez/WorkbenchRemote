#include "base.h"
#include "PacketPTP.h"
#include "PacketFollowUp.h"
#include "PTPFollowupCorruptionComponent.h"

PTPFollowupCorruptionComponent::PTPFollowupCorruptionComponent(ValueTree corruptionTree_, int64 adapterGuid_) :
	PTPCorruptionComponent(corruptionTree_, adapterGuid_)
{
	titleLabel.setText("Follow_Up field corruption", dontSendNotification);

	for (int fieldIndex = 0; fieldIndex < PacketFollowUp::NUM_FIELDS; ++fieldIndex)
	{
		String fieldName(PacketFollowUp::getFieldName(fieldIndex));
		PacketPTP::Field field;
		field.index = fieldIndex;
		PacketFollowUp::getField(field, adapterGuid_);
		addFieldComponent(fieldName, field);
	}
}

PTPFollowupCorruptionComponent::~PTPFollowupCorruptionComponent()
{

}
