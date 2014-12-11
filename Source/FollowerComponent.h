#pragma once

class FollowerComponent : public GroupComponent
{
public:
	FollowerComponent();

	virtual void resized();
	void update();

protected:
	Label syncFollowupLabel;
	Label syncFollowupReadout;
};