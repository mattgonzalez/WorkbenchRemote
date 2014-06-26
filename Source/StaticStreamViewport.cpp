/*
==============================================================================

Copyright (C) 2014 Echo Digital Audio Corporation.

==============================================================================
*/

#include "base.h"
#include "StaticStreamViewport.h"
#include "FaultInjectionCallout.h"
#include "Identifiers.h"
#include "AddressUtilities.h"
#include "WorkbenchClient.h"

static const int MARGIN = 10;
static const int STREAM_H = 100;
static const int STREAM_GAP = 5;
static const int MIN_CHANNELS = 1;
static const int MAX_CHANNELS = 8;

StaticStreamViewport::StaticStreamViewport(ValueTree tree_,CriticalSection &lock_, WorkbenchClient* client_)
{
	setName("StaticStreamViewport " + tree_.getType().toString());

	setViewedComponent(content = new ContentComponent(tree_,lock_, client_), false);
}

StaticStreamViewport::~StaticStreamViewport()
{
}

void StaticStreamViewport::resized()
{
	int width = getWidth();
	int height = MARGIN*2 + (STREAM_H + STREAM_GAP) * content->streamBoxes.size();
	if (height > getHeight())
	{
		width -= getScrollBarThickness();
	}
	content->setSize(width, height);
	Viewport::resized();
}

void StaticStreamViewport::streamActiveStateChanged()
{
	content->streamActiveStateChanged();
}


StaticStreamViewport::ContentComponent::ContentComponent(ValueTree tree_,CriticalSection &lock_, WorkbenchClient* client_) :
tree(tree_),
lock(lock_)
{
	int i;

	setOpaque(true);

	for (i = 0; i < tree_.getNumChildren(); ++i)
	{
		StaticStreamComponent* comp = new StaticStreamComponent(tree_.getChild(i), lock_, this, client_);
		addAndMakeVisible(comp);
		streamBoxes.add(comp);
	}
}

StaticStreamViewport::ContentComponent::~ContentComponent()
{
}

void StaticStreamViewport::ContentComponent::resized()
{
	positionStaticStreamComponents();
}


void StaticStreamViewport::ContentComponent::paint( Graphics& g )
{
	g.fillAll(Colours::darkgrey);

	g.setColour(Colour::greyLevel(0.5f));
	g.drawRect(getLocalBounds(), 5);
}


void StaticStreamViewport::ContentComponent::positionStaticStreamComponents()
{
	int x = 10;
	int y = MARGIN;
	int w = getWidth() - x*2;
	int h = STREAM_H;
	int gap = STREAM_GAP;
	juce::Rectangle<int> r( x, y, w, h);

	for (int i = 0; i < streamBoxes.size(); i++)
	{
		streamBoxes[i]->setBounds(r);
		r.translate(0, r.getHeight() + gap);
	}
}

void StaticStreamViewport::ContentComponent::streamActiveStateChanged()
{
	for (int i = 0; i < streamBoxes.size(); i++)
	{
		streamBoxes[i]->enableClockReferenceButton();
	}
}



StaticStreamViewport::StaticStreamComponent::StaticStreamComponent( ValueTree tree_, CriticalSection &lock_,ContentComponent* parent_, WorkbenchClient* client_ ):
	tree(tree_),
	lock(lock_),
	client(client_),
	parentContentComponent(parent_),
	startButton("Start"),
	stopButton("Stop"),
	faultButton("Inject"),
	clockReferenceButton("CRS"),
	streamIdLabel(String::empty,"Stream ID"),
	multicastAddressLabel(String::empty,"Multicast address"),
	channelsLabel(String::empty,"Channels")
{
	setName("StaticStreamViewport::StaticStreamComponent " + tree_[Identifiers::Name].toString());

	streamIdLabel.setJustificationType(Justification::centred);
	streamIdEditor.setText( String::toHexString((int64)tree_[Identifiers::StreamID]), dontSendNotification);
	addAndMakeVisible(&streamIdEditor);
	addAndMakeVisible(&streamIdLabel);

	multicastAddressLabel.setJustificationType(Justification::centred);
	multicastAddressEditor.setText( tree_[Identifiers::DestinationAddress], dontSendNotification);
	addAndMakeVisible(&multicastAddressEditor);
	addAndMakeVisible(&multicastAddressLabel);

	channelsLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(&channelsCombo);
	addAndMakeVisible(&channelsLabel);
	addAndMakeVisible(&startButton);
	addAndMakeVisible(&stopButton);

	if(tree.getParent().getType() == Identifiers::Talkers)
	{
		addAndMakeVisible(&faultButton);
		faultButton.addListener(this);
	}
	
	addAndMakeVisible(&clockReferenceButton);
	startButton.setEnabled(false);
	stopButton.setEnabled(false);

	clockReferenceButton.setToggleState(AVTP_SUBTYPE_CRS == (int)tree_[Identifiers::Subtype], dontSendNotification);
	setChannelsVisible();

	addAndMakeVisible(&metricsButton);

	for (int channel = MIN_CHANNELS; channel <= MAX_CHANNELS; ++channel)
	{
		channelsCombo.addItem(String(channel),channel);
	}
	channelsCombo.setSelectedId(tree_[Identifiers::Subtype], dontSendNotification);
	channelsCombo.setEditableText(true);

	multicastAddressEditor.addListener(this);
	streamIdEditor.addListener(this);
	channelsCombo.addListener(this);
	metricsButton.addListener(this);
	startButton.addListener(this);
	stopButton.addListener(this);
	clockReferenceButton.addListener(this);

	tree.addListener(this);
}

StaticStreamViewport::StaticStreamComponent::~StaticStreamComponent()
{
	
}

void StaticStreamViewport::StaticStreamComponent::paint( Graphics& g )
{
	juce::Rectangle<float> r(getLocalBounds().toFloat());
	float c = getWidth() * 0.03f;
	g.setColour(Colours::white);
	g.fillRoundedRectangle( r, c);
	g.setColour(Colours::orange.withAlpha(0.1f));
	g.fillRoundedRectangle( r, c);

	Font f(g.getCurrentFont());
	g.setColour(Colours::black);
	r.translate(0,5);
	r.setHeight(f.getHeight());
	f.setBold(true);
	g.setFont(f);
	g.drawText( tree[Identifiers::Name], r, Justification::centred, false);
	g.fillRect( 0.0f, r.getBottom(), r.getWidth(), 1.0f);
}

void StaticStreamViewport::StaticStreamComponent::resized()
{
	streamIdEditor.setSize(120,20);
	streamIdEditor.setCentreRelative(0.18f,0.5f);
	{
		juce::Rectangle<int> r(streamIdEditor.getBounds());
		streamIdLabel.setBounds(r.translated(0, -r.getHeight()));
	}

	multicastAddressEditor.setSize(120,20);
	multicastAddressEditor.setCentreRelative(0.54f,0.5f);
	{
		juce::Rectangle<int> r(multicastAddressEditor.getBounds());
		multicastAddressLabel.setBounds(r.translated(0, -r.getHeight()));
	}

	channelsCombo.setSize(60,20);
	channelsCombo.setCentreRelative(0.82f,0.5f);
	{
		juce::Rectangle<int> r(channelsCombo.getBounds());
		channelsLabel.setBounds(r.translated(0, -r.getHeight()));
	}

	startButton.setSize(40,25);
	stopButton.setSize(40,25);
	faultButton.setSize(80, 25);
	clockReferenceButton.setSize(60,25);
	startButton.setCentreRelative(0.4f,0.8f);
	stopButton.setCentreRelative(0.6f,0.8f);
	faultButton.setCentreRelative(0.2f,0.8f);
	clockReferenceButton.setCentreRelative(0.85f,0.8f);

	if (tree.getParent().getType() == Identifiers::Talkers)
	{
		streamIdEditor.setCentreRelative(0.3f,0.5f);
		multicastAddressEditor.setCentreRelative(0.6f,0.5f);
		channelsCombo.setCentreRelative(0.85f,0.5f);
		{
			juce::Rectangle<int> r(streamIdEditor.getBounds());
			streamIdLabel.setBounds(r.translated(0, -r.getHeight()));
		}
		{
			juce::Rectangle<int> r(multicastAddressEditor.getBounds());
			multicastAddressLabel.setBounds(r.translated(0, -r.getHeight()));
		}
		{
			juce::Rectangle<int> r(channelsCombo.getBounds());
			channelsLabel.setBounds(r.translated(0, -r.getHeight()));
		}
		metricsButton.setCentreRelative(0.1f, 0.5f);
	}
	else
	{
		metricsButton.setCentreRelative(0.9f, 0.5f);
	}
}

void StaticStreamViewport::StaticStreamComponent::textEditorTextChanged( TextEditor&)
{
}

void StaticStreamViewport::StaticStreamComponent::textEditorReturnKeyPressed( TextEditor& editor)
{
	if (&multicastAddressEditor == &editor)
	{
		userInputMulticastAddress();
		return;
	}
	if (&streamIdEditor == &editor)
	{
		userInputStreamID();
		return;
	}
}

void StaticStreamViewport::StaticStreamComponent::textEditorEscapeKeyPressed( TextEditor& editor)
{
	if (&multicastAddressEditor == &editor)
	{
		multicastAddressEditor.setText( Int64ToMACAddress(tree[Identifiers::DestinationAddress]).toString(), dontSendNotification);
		return;
	}
	if (&streamIdEditor == &editor)
	{
		streamIdEditor.setText( String::toHexString((int64)tree[Identifiers::StreamID]), dontSendNotification);
		return;
	}
}

void StaticStreamViewport::StaticStreamComponent::textEditorFocusLost( TextEditor& editor)
{
	if (&multicastAddressEditor == &editor)
	{
		userInputMulticastAddress();
		return;
	}
	if (&streamIdEditor == &editor)
	{
		userInputStreamID();
		return;
	}
}

void StaticStreamViewport::StaticStreamComponent::userInputMulticastAddress()
{
	ScopedLock locker(lock);

	String text = multicastAddressEditor.getText();
	int64 temp = text.getHexValue64();
	tree.setProperty(Identifiers::DestinationAddress, temp, nullptr);
	client->setStreamProperty(tree.getParent().getType(), tree[Identifiers::Index], Identifiers::DestinationAddress, text);
}

void StaticStreamViewport::StaticStreamComponent::userInputStreamID()
{
	ScopedLock locker(lock);

	String text = streamIdEditor.getText();
	int64 streamID = text.getHexValue64();
	tree.setProperty(Identifiers::StreamID, streamID, nullptr);
	client->setStreamProperty(tree.getParent().getType(), tree[Identifiers::Index], Identifiers::StreamID, text);
}

void StaticStreamViewport::StaticStreamComponent::comboBoxChanged( ComboBox* comboBoxThatHasChanged )
{
	ScopedLock locker(lock);

	String text(channelsCombo.getText());
	int channels = text.getIntValue();
	tree.setProperty(Identifiers::ChannelCount, channels, nullptr);
	client->setStreamProperty(tree.getParent().getType(), tree[Identifiers::Index], Identifiers::ChannelCount, channels);
}

void StaticStreamViewport::StaticStreamComponent::buttonClicked( Button* button)
{
	ScopedLock locker(lock);

	if (& startButton == button)
	{
		tree.setProperty(Identifiers::Active, true, nullptr);
		client->setStreamProperty(tree.getParent().getType(), tree[Identifiers::Index], Identifiers::Active, true);
		enableControls(true);
		return;
	}

	if (& faultButton == button)
	{
		if (tree.getParent().isValid())
		{
			FaultInjectionCallout * content = new FaultInjectionCallout(tree.getParent(), lock, client);
			CallOutBox::launchAsynchronously(content, button->getScreenBounds(), nullptr);
		}
		return;
	}

	if (& stopButton == button)
	{
		tree.setProperty(Identifiers::Active, false, nullptr);
		client->setStreamProperty(tree.getParent().getType(), tree[Identifiers::Index], Identifiers::Active, false);
		enableControls(false);
		return;
	}

	if (& clockReferenceButton == button)
	{
		int subtype = (button->getToggleState()) ? AVTP_SUBTYPE_CRS : AVTP_SUBTYPE_AUDIO;
		tree.setProperty(Identifiers::Subtype, subtype, nullptr);
		client->setStreamProperty(tree.getParent().getType(), tree[Identifiers::Index], Identifiers::Subtype, subtype);
		return;
	}
}



void StaticStreamViewport::StaticStreamComponent::enableControls( bool started )
{
	streamIdEditor.setEnabled(!started);
	multicastAddressEditor.setEnabled(!started);
	channelsCombo.setEnabled(!started);
	startButton.setEnabled(!started);
	stopButton.setEnabled(started);
	enableClockReferenceButton();
	setChannelsVisible();

	repaint();
}


void StaticStreamViewport::StaticStreamComponent::setChannelsVisible()
{
	bool crs = clockReferenceButton.getToggleState();
	channelsCombo.setVisible(!crs);
	int channels = tree[Identifiers::ChannelCount];
	channelsCombo.setSelectedId(channels, dontSendNotification);
	channelsLabel.setVisible(!crs);
}

void StaticStreamViewport::StaticStreamComponent::valueTreePropertyChanged( ValueTree& treeWhosePropertyHasChanged, const Identifier& property )
{
	ScopedLock locker(lock);

	DBG("StaticStreamViewport::StaticStreamComponent::valueTreePropertyChanged " + treeWhosePropertyHasChanged.getType().toString() + " " + property.toString());

	if (Identifiers::Name == property)
	{
		repaint();
		return;
	}

	if (Identifiers::StreamID == property)
	{
		int64 streamID = tree[Identifiers::StreamID];
		streamIdEditor.setText(String::toHexString(streamID));
		return;
	}

	if (Identifiers::DestinationAddress == property)
	{
		int64 address64 = tree[Identifiers::DestinationAddress];
		MACAddress address(Int64ToMACAddress(address64));
		multicastAddressEditor.setText(address.toString());
		return;
	}

	if (Identifiers::ChannelCount == property)
	{
		channelsCombo.setSelectedId(tree[Identifiers::ChannelCount], dontSendNotification);
		return;
	}

	if (Identifiers::Subtype == property)
	{
		int subtype = tree[Identifiers::Subtype];
		clockReferenceButton.setToggleState(AVTP_SUBTYPE_CRS == subtype,dontSendNotification);
		setChannelsVisible();
		return;
	}

	if (Identifiers::Active == property)
	{
		enableControls(treeWhosePropertyHasChanged.getProperty(property));
		return;
	}
}

void StaticStreamViewport::StaticStreamComponent::valueTreeChildAdded( ValueTree& parentTree, ValueTree& childWhichHasBeenAdded )
{
}

void StaticStreamViewport::StaticStreamComponent::valueTreeChildRemoved( ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved )
{
}

void StaticStreamViewport::StaticStreamComponent::valueTreeChildOrderChanged( ValueTree& parentTreeWhoseChildrenHaveMoved )
{
}

void StaticStreamViewport::StaticStreamComponent::valueTreeParentChanged( ValueTree& treeWhoseParentHasChanged )
{
}

void StaticStreamViewport::StaticStreamComponent::enableClockReferenceButton()
{
	ValueTree streamsTree(tree.getParent().getParent());
	ScopedLock locker(lock);
	bool anyStreamActive = false;

	ValueTree talkersTree(streamsTree.getChildWithName(Identifiers::Talkers));
	for (int i = 0; i < talkersTree.getNumChildren(); ++i)
	{
		ValueTree streamTree(talkersTree.getChild(i));
		anyStreamActive |= (bool) streamTree[Identifiers::Active];
	}

	ValueTree listenersTree(streamsTree.getChildWithName(Identifiers::Listeners));
	for (int i = 0; i < listenersTree.getNumChildren(); ++i)
	{
		ValueTree streamTree(listenersTree.getChild(i));
		anyStreamActive |= (bool) streamTree[Identifiers::Active];
	}

	clockReferenceButton.setEnabled(!anyStreamActive);
}

StaticStreamViewport::StaticStreamComponent::MetricsButton::MetricsButton() : Button("Metrics Button")
{
}

void StaticStreamViewport::StaticStreamComponent::MetricsButton::paintButton( Graphics& g, bool isMouseOverButton, bool isButtonDown )
{
#if 0
	Image metrics = ImageCache::getFromMemory(BinaryData::infobtn_png, BinaryData::infobtn_pngSize);
	g.setOpacity(0.750f);
	if (isMouseOverButton)
	{
		metrics = ImageCache::getFromMemory(BinaryData::infobtnover_png, BinaryData::infobtnover_pngSize);
	}
	if (isButtonDown)
	{
		metrics = ImageCache::getFromMemory(BinaryData::infobtndown_png, BinaryData::infobtndown_pngSize);
	}
	g.drawImageAt(metrics,0,0);
#endif
}

