#pragma once
class GrandmasterComponent : public GroupComponent, public Button::Listener
{
public:
	GrandmasterComponent();

	virtual void resized();

	virtual void buttonClicked(Button*);

protected:
	Label syncFollowupLabel;
	Label syncFollowupReadout;
	Label initialPTPTimeLabel;

	TextEditor initialPTPTimeEditor;

	TextButton initialPTPTimeButton;

	ToggleButton automaticPTPTimeToggle;

};