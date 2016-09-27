#include "base.h"
#include "ValueTreeDisplay.h"

#if 0 // Currently not being using but keeping it in case we want to resurrect it
class PropertyEditor : public PropertyPanel, public ValueTree::Listener
{
public:
	PropertyEditor(ValueTree &selectedTree_) :
		selectedTree(selectedTree_)
	{
		noEditValue = "not editable";

		selectedTree_.addListener(this);
	}

private:
	Value noEditValue;
	ValueTree &selectedTree;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyEditor);

	virtual void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override
	{
		if (treeWhosePropertyHasChanged == selectedTree)
		{
			update();
		}
	}
	virtual void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override {}
	virtual void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override {}
	virtual void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override {}
	virtual void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override {}
	virtual void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override
	{
		update();
	}

	void update()
	{
		clear();
		
		const int maxChars = 200;
		Array<PropertyComponent*> propertyComponents;

		for (int i = 0; i < selectedTree.getNumProperties(); ++i)
		{
			const Identifier name = selectedTree.getPropertyName(i).toString();
			Value value = selectedTree.getPropertyAsValue(name, nullptr);
			TextPropertyComponent * textPropertyComponent;

			if (value.getValue().isObject())
			{
				textPropertyComponent = new TextPropertyComponent(noEditValue, name.toString(), maxChars, false);
				textPropertyComponent->setEnabled(false);
			}
			else
			{
				textPropertyComponent = new TextPropertyComponent(value, name.toString(), maxChars, false);
			}

			propertyComponents.add(textPropertyComponent);
		}

		addProperties(propertyComponents);
	}
};
#endif

String const leftBracket("[");
String const rightBracket("] ");

class PropertyTreeComponent : public TreeView, public ValueTree::Listener
{
private:
	class VarItem : public TreeViewItem
	{
	public:
		VarItem( var const &value_) :
			value(value_)
		{
			if (value_.isArray())
			{
				for (int i = 0; i < value_.size(); ++i)
				{
					addSubItem(new ArrayEntryVarItem(i, value_[i]));
				}
			}
			else if (DynamicObject* object = value_.getDynamicObject())
			{
				NamedValueSet& objectProperties(object->getProperties());
				for (int i = 0; i < objectProperties.size(); ++i)
				{
					Identifier objectProperty(objectProperties.getName(i));
					addSubItem(new NamedVarItem(objectProperty, objectProperties[objectProperty]));
				}
			}
		}

		virtual ~VarItem()
		{
		}

		virtual bool mightContainSubItems() override
		{
			if (value.isArray())
			{ 
				return value.size() > 0;
			}

			if (DynamicObject* object = value.getDynamicObject())
			{
				return object->getProperties().size() > 0;
			}

			return false;
		}

		var const &value;
	};

	class NamedVarItem : public VarItem
	{
	public:
		NamedVarItem(Identifier const property_, var const &value_) :
			VarItem(value_),
			property(property_)
		{
		}

		virtual ~NamedVarItem()
		{
		}

		virtual void paintItem(Graphics& g, int width, int height) override
		{
			g.setColour(Colours::black);
			String text(leftBracket + property.toString() + rightBracket + value.toString());
			g.drawText(text, 0, 0, width, height, Justification::centredLeft, true);
		}

		Identifier const property;
	};

	class ArrayEntryVarItem : public VarItem
	{
	public:
		ArrayEntryVarItem(int const arrayIndex_, var const &value_) :
			VarItem(value_),
			arrayIndex(arrayIndex_)
		{
		}

		virtual ~ArrayEntryVarItem()
		{
		}

		virtual void paintItem(Graphics& g, int width, int height) override
		{
			g.setColour(Colours::black);
			String text(leftBracket + String(arrayIndex) + rightBracket + value.toString());
			g.drawText(text, 0, 0, width, height, Justification::centredLeft, true);
		}

		int const arrayIndex;
	};

	class RootItem : public TreeViewItem
	{
	public:
		RootItem(ValueTree &selectedTree_) :
			selectedTree(selectedTree_)
		{
		}

		virtual ~RootItem()
		{
		}

		virtual bool mightContainSubItems() override
		{
			return selectedTree.getNumProperties() > 0;
		}

		virtual void paintItem(Graphics& g, int width, int height) override
		{
			g.setColour(Colours::black);
			g.drawText(selectedTree.getType().toString(), 0, 0, width, height, Justification::centredLeft, true);
		}

		void update()
		{
			clearSubItems();

			for (int i = 0; i < selectedTree.getNumProperties(); ++i)
			{
				Identifier property(selectedTree.getPropertyName(i));
				addSubItem(new NamedVarItem(property, selectedTree[property]));
			}
		}

		ValueTree &selectedTree;
	} rootItem;

	ValueTree &selectedTree;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PropertyTreeComponent);

	virtual void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override
	{
		if (treeWhosePropertyHasChanged == selectedTree)
		{
			update();
		}
	}
	virtual void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override {}
	virtual void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override {}
	virtual void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override {}
	virtual void valueTreeParentChanged(ValueTree& treeWhoseParentHasChanged) override {}
	virtual void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override
	{
		update();
	}

	void update()
	{
		rootItem.update();
	}

public:
	PropertyTreeComponent(ValueTree &selectedTree_) :
		rootItem(selectedTree_),
		selectedTree(selectedTree_)
	{
		setDefaultOpenness(true);
		setRootItem(&rootItem);
		selectedTree_.addListener(this);
		update();
	}
};


ValueTreeDisplay::ValueTreeDisplay() : 
	layoutResizer(&layout, 1, false)
{
	layout.setItemLayout(0, -0.1, -0.9, -0.6);
	layout.setItemLayout(1, 5, 5, 5);
	layout.setItemLayout(2, -0.1, -0.9, -0.4);

	setSize(1000, 700);

	propertyEditor = new PropertyTreeComponent(selectedTree);

	addAndMakeVisible(treeView);
	addAndMakeVisible(propertyEditor);
	addAndMakeVisible(layoutResizer);

	treeView.setDefaultOpenness(true);
}

ValueTreeDisplay::~ValueTreeDisplay()
{
	treeView.setRootItem(nullptr);
}

void ValueTreeDisplay::paint(Graphics &g)
{
	g.setColour(Colours::lightgrey.withAlpha(0.5f));
	g.fillRect(propertyEditor->getBounds());
}

void ValueTreeDisplay::resized()
{
	Component * comps[] = { &treeView, &layoutResizer, propertyEditor };
	layout.layOutComponents(comps, 3, 0, 0, getWidth(), getHeight(), true, true);
}

void ValueTreeDisplay::setTree(ValueTree newTree)
{
	if (newTree == ValueTree::invalid)
	{
		treeView.setRootItem(nullptr);
		selectedTree = ValueTree::invalid;
	}
	else if (tree != newTree)
	{
		tree = newTree;
		item = new Item(tree, selectedTree);
		treeView.setRootItem(item);
		selectedTree = tree;
	}
}

ValueTreeDisplay::Item::Item(ValueTree tree_, ValueTree &selectedTree_) :
	tree(tree_),
	selectedTree(selectedTree_)
{
    updateSubItems();
    
	tree.addListener(this);
}

ValueTreeDisplay::Item::~Item()
{
	clearSubItems();
}

bool ValueTreeDisplay::Item::mightContainSubItems()
{
	return tree.getNumChildren() > 0;
}

void ValueTreeDisplay::Item::itemOpennessChanged(bool isNowOpen)
{
}

void ValueTreeDisplay::Item::updateSubItems()
{
	clearSubItems();

	for (int i = 0; i <  tree.getNumChildren(); ++i)
    {
		addSubItem(new Item(tree.getChild(i), selectedTree));
    }
}

void ValueTreeDisplay::Item::paintItem(Graphics & g, int w, int h)
{
	Font font;
	Font smallFont(11.0);

	if (isSelected())
		g.fillAll(Colours::white);

	String typeName = tree.getType().toString();
	const float nameWidth = font.getStringWidthFloat(typeName);
	const int propertyX = roundFloatToInt(nameWidth);

	g.setColour(Colours::black);
	g.setFont(font);

	g.drawText(tree.getType().toString(), 0, 0, w, h, Justification::left, false);

	int propertyW = w - propertyX;
	if (propertyW > 0)
	{
		g.setColour(Colours::blue);
		g.setFont(smallFont);
		String propertySummary;

		for (int i = 0; i < tree.getNumProperties(); ++i)
		{
			const Identifier name = tree.getPropertyName(i).toString();
			propertySummary += leftBracket + name.toString() + rightBracket + tree.getProperty(name).toString();
		}

		g.drawText(propertySummary, propertyX, 0, propertyW, h, Justification::left, true);
	}
}

void ValueTreeDisplay::Item::itemSelectionChanged(bool isNowSelected)
{
	if (isNowSelected)
	{
		selectedTree = tree;
	}
}

void ValueTreeDisplay::Item::valueTreePropertyChanged(ValueTree &treeWhosePropertyHasChanged, const Identifier &/*property*/)
{
	if (tree != treeWhosePropertyHasChanged) return;
	tree.removeListener(this);
	//            if (isSelected())

	//                propertiesEditor-&gt;setSource(t);

	repaintItem();
	tree.addListener(this);
}

ValueTreeDisplayWindow::ValueTreeDisplayWindow() : DocumentWindow("ValueTree",
	Colours::lightgrey,
	DocumentWindow::allButtons)
{
	setUsingNativeTitleBar(true);

	setContentNonOwned(&treeDisplay, false);

	centreWithSize(300, 600);
	setVisible(true);

	setResizable(true, false);
}

void ValueTreeDisplayWindow::closeButtonPressed()
{
	delete this;
}
