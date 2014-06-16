#include "base.h"
#include "GearButton.h"
#include "BinaryData.h"

GearButton::GearButton():
	ImageButton ("GearButton")
{
	setImages( false, false, true,
		ImageCache::getFromMemory(BinaryData::gearbutton_png, BinaryData::gearbutton_pngSize), 1.0f, Colours::transparentWhite,
		ImageCache::getFromMemory(BinaryData::gearbuttonover_png, BinaryData::gearbuttonover_pngSize), 1.0f, Colours::transparentWhite,
		ImageCache::getFromMemory(BinaryData::gearbuttondownr_png, BinaryData::gearbuttondownr_pngSize), 1.0f, Colours::transparentWhite);
}

// void GearButton::paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown )
// {
// 	Image gear(ImageCache::getFromMemory(BinaryData::gearbutton_png, BinaryData::gearbutton_pngSize));
// 
// 	g.setOpacity(1.0f);
// 	g.drawImage(gear, 0, 0, getWidth(), getHeight(),
// 		0, 0, gear.getWidth(), gear.getHeight());
// }
