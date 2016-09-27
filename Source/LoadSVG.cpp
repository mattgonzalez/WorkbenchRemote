#include "base.h"
#include "LoadSVG.h"

Drawable * LoadSVG(const char *data)
{
	String raw(data);
	XmlDocument document(raw);
	ScopedPointer<XmlElement> element = document.getDocumentElement();
	if (element == nullptr)
	{
		return nullptr;
	}

	if (element != 0)
	{
		return Drawable::createFromSVG(*element);
	}
	return nullptr;
}
