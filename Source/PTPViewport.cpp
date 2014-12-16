#include "base.h"
#include "PTPViewport.h"
#include "WorkbenchClient.h"
#include "PTPComponent.h"

static const int MARGIN = 10;
static const int STREAM_H = 120;
static const int STREAM_GAP = 5;


PTPViewport::PTPViewport(ValueTree tree, WorkbenchClient * client_)
{
	setName("PTPViewport");
	setViewedComponent(ptpComponent = new PTPComponent(tree, client_), false);
}

PTPViewport::~PTPViewport()
{
	
}

void PTPViewport::resized()
{
	int width = getWidth();
	int height = 2000;
	if (height > getHeight())
	{
		width -= getScrollBarThickness();
	}
	ptpComponent->setSize(width, height);
	Viewport::resized();
}
