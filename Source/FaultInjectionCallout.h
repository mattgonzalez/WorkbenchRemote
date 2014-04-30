#pragma once
class Controller;
class Stream;
class FaultInjectionCallout : public Component, public Slider::Listener, public Button::Listener
{
public:
	FaultInjectionCallout(Controller * controller_, ReferenceCountedObjectPtr<Stream> stream_);
	~FaultInjectionCallout();

	virtual void resized();

	Label faultInjectionLab;
	TextButton resetButton;

	ReferenceCountedObjectPtr<Stream> stream;

protected:
	void configPercentSlider(Slider &slider);

	virtual void sliderValueChanged( Slider* slider );

	ValueTree getFaultInjectionTree();

	virtual void buttonClicked( Button* );
	void resetSliders();
	Controller * controller;

	class Group
	{
	public:
		ScopedPointer<ToggleButton> toggleBtn;
		ScopedPointer<Slider> percentSlider;
		ScopedPointer<Slider> amountSlider;
	};
	
	OwnedArray<Group> groups;

	class HelpTextDisplay : public MouseListener
	{
	public:
		HelpTextDisplay();
		virtual void mouseMove( const MouseEvent& event );
		Component* currentHelpComponent;
		Label label;
	} helpTextDisplay;

};
