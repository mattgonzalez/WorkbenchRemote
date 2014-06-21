/*
  ==============================================================================

	ChannelComponent.h
	Created: 15 Nov 2013 10:46:41am
	Author:  devnull

  ==============================================================================
*/
 
#ifndef OUTPUTCHANNELCOMPONENT_H_INCLUDED
#define OUTPUTCHANNELCOMPONENT_H_INCLUDED

#include "ChannelComponent.h"

//==============================================================================
/*
*/
class OutputChannelComponent : 
	public ChannelComponent, 
	public ValueTree::Listener,
	public Slider::Listener, 
	public Button::Listener
{
public:
	OutputChannelComponent(int deviceIndex_, ValueTree outputChannelTree_, int channelNumber_, CriticalSection &lock_);
	~OutputChannelComponent();

	virtual void timerCallback();

	void resized();
	virtual void mouseUp( const MouseEvent& event ) override;

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );


	virtual void sliderValueChanged( Slider* slider );	
	virtual void buttonClicked( Button* );

protected:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputChannelComponent)

	ValueTree outputChannelTree;
	CriticalSection& lock;
	Slider gainSlider;
	TextButton muteButton;
	Slider toneFrequencySlider;

	class ModeButton : public Button
	{
	public:
		ModeButton();

		virtual void paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown );

	} modeButton;

	void showModePopup();
	static void popupMenuFinishedCallback (int, OutputChannelComponent*);

	static int getDeviceIndexFromMenuID(int const menuID);
	static int getChannelFromMenuID(int const menuID);
	static int makeMenuID(int const deviceIndex, int const channel);
	void setModeButtonText();
	void updateMode();

	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );
};


#endif  // OUTPUTCHANNELCOMPONENT_H_INCLUDED
