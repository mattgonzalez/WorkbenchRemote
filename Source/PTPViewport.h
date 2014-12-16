#pragma once

class WorkbenchClient;
class PTPComponent;

class PTPViewport : public Viewport
{
public:
	PTPViewport(ValueTree tree, WorkbenchClient * client_);
	~PTPViewport();

	virtual void resized() override;

protected:
	ScopedPointer<PTPComponent> ptpComponent;
};