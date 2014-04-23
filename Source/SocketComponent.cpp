/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "SocketComponent.h"

const char *test ="{\"menu\": {	\"id\": \"file\",	\"value\": \"File\",	\"popup\": {	\"menuitem\": [{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}	]}}}";

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

int port = 5678;

//==============================================================================
SocketComponent::SocketComponent ()
{
    addAndMakeVisible (addressEditor = new TextEditor ("addressEditor"));
    addressEditor->setMultiLine (false);
    addressEditor->setReturnKeyStartsNewLine (false);
    addressEditor->setReadOnly (false);
    addressEditor->setScrollbarsShown (true);
    addressEditor->setCaretVisible (true);
    addressEditor->setPopupMenuEnabled (true);
    addressEditor->setText ("127.0.0.1");

    addAndMakeVisible (label = new Label ("new label",
                                          TRANS("Address")));
    label->setFont (Font (15.00f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (connectButton = new TextButton ("connectButton"));
    connectButton->setButtonText (TRANS("Connect"));
    connectButton->addListener (this);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

	#if 0
server.beginWaitingForSocket(port);

	String s1(test);
	var v(JSON::parse(s1));

	MemoryOutputStream outputStream;
	
	JSON::writeToStream(outputStream, v, false);

	NamedValueSet n = v.getDynamicObject()->getProperties();
	for (int i = 0; i < n.size(); i++)
	{
		DynamicObject* d = n.getValueAt(i).getDynamicObject();
		DBG(n.getName(i).toString());
		outputStream.
	}
	
	//v.getDynamicObject()->writeAsJSON(outputStream,3,false);
	DBG(outputStream.toString());
#endif

#if 0
	DynamicObject d;
	var v1(&d);
	NamedValueSet &nvs(v1.getDynamicObject()->getProperties());
	nvs.set("foo","bar");
//	v1.getDynamicObject()->setProperty("foo", "bar");
	MemoryOutputStream outputStream;
	JSON::writeToStream(outputStream, v1, false);
	DBG(outputStream.toString());
#endif

    //[/Constructor]
}

SocketComponent::~SocketComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    addressEditor = nullptr;
    label = nullptr;
    connectButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SocketComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SocketComponent::resized()
{
    addressEditor->setBounds (96, 40, 150, 24);
    label->setBounds (24, 40, 88, 24);
    connectButton->setBounds (264, 40, 104, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

uint8 out[] = { 1,2,3,4,5,6,7,8};
uint8 in[8];

void SocketComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == connectButton)
    {
        //[UserButtonCode_connectButton] -- add your button handler code here..

		//bool start = listenSocket.createListener(123456);
		#if 0
listenSocket.bindToPort(123456);
		bool connected = socket.connect( addressEditor->getText(), 123456);
		DBG("connect " << (int)connected);


		zerostruct(in);

		int written = socket.write(out,sizeof(out));
		int ready = listenSocket.waitUntilReady(true, 1000);
		int read = listenSocket.read(in, sizeof(in), false);
		DBG("read " << read << "  ready " << ready);
#endif

		bool connected = client.connectToSocket("192.168.1.145",port,1000);
		DBG("connect " << (int)connected);

		MemoryBlock block(8);
		uint8 *data = (uint8*)block.getData();
		for (size_t i = 0; i < block.getSize(); i++)
		{
			data[i] = (uint8)i;
		}
		bool sent = client.sendMessage(block);
		DBG("sent " << (int)sent);
		return;
        //[/UserButtonCode_connectButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SocketComponent" componentName=""
                 parentClasses="public Component, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TEXTEDITOR name="addressEditor" id="3f555d08c19e4d34" memberName="addressEditor"
              virtualName="" explicitFocusOrder="0" pos="96 40 150 24" initialText=""
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="new label" id="ad29188d938ee741" memberName="label" virtualName=""
         explicitFocusOrder="0" pos="24 40 88 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Address" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="connectButton" id="c610ca6c4ee50ff3" memberName="connectButton"
              virtualName="" explicitFocusOrder="0" pos="264 40 104 24" buttonText="Connect"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
