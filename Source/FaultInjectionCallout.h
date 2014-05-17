#pragma once
#include "WorkbenchClient.h"
class Controller;
class Stream;
class FaultInjectionCallout : public Component, public Slider::Listener, public Button::Listener, public ValueTree::Listener
{
public:
	FaultInjectionCallout(ValueTree tree_, CriticalSection& lock_, WorkbenchClient* client_);
	~FaultInjectionCallout();

	virtual void resized();

	Label faultInjectionLabel;
	TextButton resetButton;

	ValueTree tree;
	CriticalSection &lock;
	WorkbenchClient* client;

protected:
	void configPercentSlider(Slider &slider);

	virtual void sliderValueChanged( Slider* slider );

	virtual void buttonClicked( Button* );
	void resetSliders();

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );

	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );

	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );

	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );

	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

	class Group
	{
	public:
		ScopedPointer<ToggleButton> toggleButton;
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
