#include "base.h"
#include "LookAndFeel.h"
#include "BinaryData.h"
#include "Identifiers.h"


void LocalLookAndFeel::drawPropertyPanelSectionHeader(Graphics&g, const String& name, bool isOpen, int width, int height)
{
	normUnchecked = ImageCache::getFromMemory(BinaryData::normalunchecked_png, BinaryData::normalunchecked_pngSize);
	normChecked = ImageCache::getFromMemory(BinaryData::normalchecked_png, BinaryData::normalchecked_pngSize);
	overUnchecked = ImageCache::getFromMemory(BinaryData::overunchecked_png, BinaryData::overunchecked_pngSize);
	overChecked = ImageCache::getFromMemory(BinaryData::overchecked_png, BinaryData::overchecked_pngSize);
	downUnchecked = ImageCache::getFromMemory(BinaryData::downunchecked_png, BinaryData::downunchecked_pngSize);
	downChecked = ImageCache::getFromMemory(BinaryData::downchecked_png, BinaryData::downchecked_pngSize);
	disUnchecked = ImageCache::getFromMemory(BinaryData::disunchecked_png, BinaryData::disunchecked_pngSize);
	disChecked = ImageCache::getFromMemory(BinaryData::dischecked_png, BinaryData::dischecked_pngSize);
	comboBoxUp = ImageCache::getFromMemory(BinaryData::combobox_editableupskin_png, BinaryData::combobox_editableupskin_pngSize);
	comboBoxDown = ImageCache::getFromMemory(BinaryData::combobox_editabledownskin_png, BinaryData::combobox_editabledownskin_pngSize);
	comboBoxOver = ImageCache::getFromMemory(BinaryData::combobox_editableoverskin_png, BinaryData::combobox_editableoverskin_pngSize);
	comboBoxDis = ImageCache::getFromMemory(BinaryData::combobox_editabledisabledskin_png, BinaryData::combobox_editabledisabledskin_pngSize);

	Colour c(Colours::lightgrey);
	g.setColour(c);
	g.fillRect(0, 0, width, height);
	LookAndFeel_V2::drawComboBox(g, name, isOpen, width, height);
}

void LocalLookAndFeel::drawToggleButton(Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown)
{
	int destWidth, destHeight;

	destHeight = button.getHeight();
	if (button.getButtonText().length() > 0)
	{
		destWidth = destHeight;
	}
	else
	{
		destWidth = button.getWidth();
	}
	int y;

	Image img;
	if (button.getToggleState())
	{
		if (button.isEnabled())
		{
			img = normChecked;

			if (isMouseOverButton == true)
			{
				img = overChecked;
			}
			if (isButtonDown == true)
			{
				img = downChecked;
			}
		}
		else
		{
			img = disChecked;
		}
	}
	else
	{
		if (button.isEnabled())
		{
			img = normUnchecked;

			if (isMouseOverButton == true)
			{
				img = overUnchecked;
			}
			if (isButtonDown == true)
			{
				img = downUnchecked;
			}
		}
		else
		{
			img = disUnchecked;
		}
	}
	y = (button.getHeight() - img.getHeight()) >> 1;
	g.drawImageAt(img, 2, y);

	//
	//	Button text
	//
	if (button.getButtonText().length() > 0)
	{
		g.setFont(14.0f);
		Colour c(Colours::black);
		if (false == button.isEnabled())
			c = c.withAlpha(0.5f);
		g.setColour(c);

		int textWid = button.getButtonText().length();

		g.drawText(button.getButtonText(), destWidth + 5, 0, button.getWidth() - textWid, button.getHeight(), Justification::centredLeft, false);
	}
}

void LocalLookAndFeel::drawComboBox(Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box)
{
	int w = 20;
	int x = width - w;
	int border = 1;

	g.fillAll(box.findColour(ComboBox::backgroundColourId));
	Image image(comboBoxUp);

	g.setColour(box.findColour(ComboBox::outlineColourId));
	g.setOpacity(1.0f);
	const Colour shadowColour(box.findColour(TextEditor::shadowColourId).withMultipliedAlpha(0.75f));
	drawBevel(g, 0, 0, width - w, height + 2, border + 2, shadowColour, shadowColour);

	if (box.isMouseOver(true))
	{
		image = comboBoxOver;
	}

	if (isButtonDown)
	{
		image = comboBoxDown;
		g.setColour(box.findColour(TextEditor::focusedOutlineColourId));
		g.drawRect(0, 0, width - w, height, border);

		g.setOpacity(1.0f);
		const Colour shadowColour(box.findColour(TextEditor::shadowColourId).withMultipliedAlpha(0.75f));
		drawBevel(g, 0, 0, width - w, height + 2, border + 2, shadowColour, shadowColour);
	}

	g.drawImage(image, x, 0, w, height, 0, 0, image.getWidth(), image.getHeight(), false);
	g.drawRect(0, 0, width - w, height);
}

void LocalLookAndFeel::positionComboBoxText(ComboBox& box, Label& label)
{
	label.setBounds(0, 0, box.getWidth() - 20, box.getHeight());

	label.setFont(getComboBoxFont(box));
}