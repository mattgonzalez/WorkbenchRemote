#pragma once

class LocalLookAndFeel : public LookAndFeel_V3
{
public:

	//toggle
	Image normUnchecked;
	Image normChecked;
	Image overUnchecked;
	Image overChecked;
	Image downUnchecked;
	Image downChecked;
	Image disUnchecked;
	Image disChecked;
	//combo box
	Image comboBoxUp;
	Image comboBoxDown;
	Image comboBoxOver;
	Image comboBoxDis;
	void drawPropertyPanelSectionHeader(Graphics&, const String& name, bool isOpen, int width, int height);
	void drawToggleButton(Graphics& g, ToggleButton& button, bool isMouseOverButton, bool isButtonDown);
	void positionComboBoxText(ComboBox& box, Label& label);
	void drawComboBox(Graphics& g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box);
};

