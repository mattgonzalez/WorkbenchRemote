#pragma once

class ValueTreeDisplay : public Component
{
public:
	ValueTreeDisplay();
	~ValueTreeDisplay();

	void paint(Graphics &g) override;
	void resized() override;

	void setTree(ValueTree newTree);

	ValueTree selectedTree;

private:
	//
	// Item is a custom TreeViewItem.
	//
	class Item : public TreeViewItem, public ValueTree::Listener
	{
	public:
		Item(/*PropertyEditor * propertiesEditor_,*/ValueTree tree_, ValueTree &selectedTree_);

		~Item();

		bool mightContainSubItems();

		void itemOpennessChanged(bool isNowOpen);

		void paintItem(Graphics & g, int w, int h);

		void itemSelectionChanged(bool isNowSelected);

		void valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &property);

		void valueTreeChildAdded(ValueTree &/*parentTree*/, ValueTree &/*childWhichHasBeenAdded*/)
		{
			updateSubItems();
		}

		void valueTreeChildRemoved(ValueTree &/*parentTree*/, ValueTree &/*childWhichHasBeenRemoved*/, int)
		{
			updateSubItems();
		}

		void valueTreeChildOrderChanged(ValueTree &/*parentTreeWhoseChildrenHaveMoved*/, int, int)
		{
			updateSubItems();
		}

		void valueTreeParentChanged(ValueTree &/*treeWhoseParentHasChanged*/)
		{
			updateSubItems();
		}

		void valueTreeRedirected(ValueTree &/*treeWhichHasBeenChanged*/)
		{
			updateSubItems();
		}

	private:
		//PropertyEditor * propertiesEditor;
		ValueTree tree;
		ValueTree &selectedTree;
		Array<Identifier> currentProperties;

		void updateSubItems();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Item);
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueTreeDisplay);
	ValueTree tree;
	ScopedPointer<Item> item;
	TreeView treeView;
	//PropertyEditor propertyEditor;
	ScopedPointer<Component> propertyEditor;
	StretchableLayoutManager layout;
	StretchableLayoutResizerBar layoutResizer;
};

class ValueTreeDisplayWindow : public DocumentWindow
{
public:
	ValueTreeDisplayWindow();

	ValueTreeDisplay treeDisplay;

	virtual void closeButtonPressed() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValueTreeDisplayWindow)
};
