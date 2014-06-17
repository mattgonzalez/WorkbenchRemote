#pragma once

class ChannelComponent;

class ChannelViewport : public Viewport, public ValueTree::Listener, public AsyncUpdater
{
public:
	ChannelViewport(int const deviceIndex_, ValueTree channelsTree_);

	virtual void resized() override;
	void paintOverChildren( Graphics& g );

	void setChannelComponentsVisible();

	virtual void valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property );
	virtual void valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded );
	virtual void valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved );
	virtual void valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved );
	virtual void valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged );

	virtual void buildChannelComponents() = 0;

	virtual void handleAsyncUpdate();

	class Content : public Component
	{
	public:
		virtual void paint( Graphics& g );
		int getTotalChildHeight();
		virtual void resized();

		OwnedArray<ChannelComponent> channelComponents;

	protected:
	} content;

protected:
	int const deviceIndex;
	ValueTree channelsTree;
};

