#pragma once

class ResetButtonSharedSVG
{
public:
	ResetButtonSharedSVG();

	ScopedPointer<Drawable> svg;

protected:
	JUCE_LEAK_DETECTOR(ResetButtonSharedSVG)
};

class ResetButton : public Button
{
public:
	ResetButton(String tooltip_);
	virtual void paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown );

protected:
	SharedResourcePointer<ResetButtonSharedSVG> sharedSVG;
	String tooltip;
} ;