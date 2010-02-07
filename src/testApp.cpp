/*=======================================================
 Copyright (c) Peter Vasil, 2010
 =======================================================*/

#include "testApp.h"

#include <iostream>
//--------------------------------------------------------------
void testApp::setup(){

	std::stringstream versionStr;
	versionStr << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR;
	m_versionString = versionStr.str();
    //-----------
	//the string is printed at the top of the app
	//to give the user some feedback
	message = "loading AppSettings.xml";
    std::cout << message << std::endl;
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( XML.loadFile("AppSettings.xml") ){
		message = "AppSettings.xml loaded!";
	}else{
		message = "unable to load AppSettings.xml check data/ folder";
	}
	std::cout << message << std::endl;

    oschost = XML.getValue("settings:osc:host", "localhost");
    oscport = XML.getValue("settings:osc:port", 57120);

	nPts = 0;
	nPtsBackup = 0;
	m_runInterval = 0;
	m_prevTime = 0;
	ofBackground(255, 255, 255);
	sender.setup(oschost,oscport);
//	sender.setup(HOST, PORT);
	m_run = false;
	m_BallPos = 0;
	m_numSampls = 0;
	isFullScreen = false;
	isPlaying = true;
	if (XML.getValue("settings:startup:debugmode", 0))
		debugMode = true;
	else
		debugMode = false;

	if (XML.getValue("settings:startup:colormode", 0))
		showColors = true;
	else
		showColors = false;

	if (XML.getValue("settings:startup:helpmode", 0))
		helpMode = true;
	else
		helpMode = false;

	if(XML.getValue("settings:startup:aboutscreen", 0))
		usageMode = true;
	else
		usageMode = false;

	if (XML.getValue("settings:debug:position", 1))
		showOscDebugPosition = true;
	else
		showOscDebugPosition = false;

	if (XML.getValue("settings:debug:sysout", 1))
		debugOutput = true;
	else
		debugOutput = false;

	m_savedStatesFileName = XML.getValue("settings:states:file", "savedStatesDefault.xml");

	m_currentSample = 0;

	for (int i = 0; i < MAX_SAMPLES; ++i) {
		m_sampleColors[i].r = ofRandom(0, 255);
		m_sampleColors[i].g = ofRandom(0, 255);
		m_sampleColors[i].b = ofRandom(0, 255);
		m_sampleColors[i].a = 127;
		m_numSamples[i] = 0;
//		m_sampleVector[i].push_back(std::vector<ofxVec3f> samples);
	}


	message = "loading "+m_savedStatesFileName;
    std::cout << message << std::endl;
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( XMLstates.loadFile(m_savedStatesFileName) ){
		message = m_savedStatesFileName+" loaded!";
	}else{
		message = "unable to load "+m_savedStatesFileName+" check data/ folder";
	}
	std::cout << message << std::endl;


	lastTagNumber = 0;

	int tags = XMLstates.getNumTags("STATE");
	if (debugOutput) {printf("states : %i\n", tags);}
	std::stringstream numstates;
	numstates << tags;
	m_stateLoadMessage = numstates.str();

	m_currentState = 0;

	m_fontTitle.loadFont("Sudbury_Basin_3D.ttf", 50);
	m_fontOfVersion.loadFont("mono.ttf", 7);
	m_fontText.loadFont("mono.ttf", 10);
	
	buttonPushed = false;
}
// -----------------------------------------------------------------------------

void testApp::loadSettingsFile()
{
}

void testApp::loadStatesFile()
{
}

//--------------------------------------------------------------
void testApp::update(){

	if (m_run)
	{
		++m_BallPos;
		//ofSleepMillis(40);

		ofxOscMessage m;
		//m.setAddress( "/mouse/position" );
		m.setAddress( "/fx" );

		m.addStringArg("positionx");
		m.addIntArg( pts[m_BallPos].x );
		m.addStringArg("positionx");
		m.addIntArg( pts[m_BallPos].y );
		sender.sendMessage( m );
		if (showOscDebugPosition) {
			setOscDebugMessage(m);
		}


		if ( m_BallPos >= nPts)
		{
			m_BallPos = 0;
		}
		//if ( pts[m_BallPos].x == m_samplVec[m_BallPos]
		checkPlayPos(pts[m_BallPos].x, pts[m_BallPos].y);
	}
	else
	{
		m_BallPos = 0;
	}
	if (debugMode)
	{

	}
	ofSleepMillis(40);


}

//--------------------------------------------------------------
void testApp::draw()
{

	//=========================================
	if (usageMode)
	{
		int rmargin = 20;
		ofSetColor(0x000000);
		m_fontTitle.drawString("SoundPainter", rmargin, 100);

		m_fontOfVersion.drawString("version "+m_versionString+", openFrameworks 0061, "+APP_AUTHOR, rmargin, 130);

		m_fontText.drawString("Paint program where you draw your musical timeline, and", rmargin, 170);
		m_fontText.drawString("put sampledots on it. Each color dot represents a sample.", rmargin, 190);
		m_fontText.drawString("You can put many sampledots with the same color on the", rmargin, 210);
		m_fontText.drawString("canvas. But SoundPainter does not play the samples itself.", rmargin, 230);
		m_fontText.drawString("OSC messages are sent to a listening Application", rmargin, 250);
		m_fontText.drawString("(e.g. Pure data, SuperCollider, Max/MSP), which plays the", rmargin, 270);
		m_fontText.drawString("sound files.", rmargin, 290);

		m_fontText.drawString("The OSC messages are:", rmargin, 350);
		m_fontText.drawString("- Sample trigger:   /play <sample number>", rmargin, 370);
		m_fontText.drawString("- Dot position:     /fx positionx <actual x> positiony <actual y>", rmargin, 390);

		//m_fontText.drawString("", rmargin, 410);
	}
	else
	{
		ofSetColor(0x000000);
		ofNoFill();
		ofBeginShape();
		for (int i = 0; i < nPts; i++){
			ofVertex(pts[i].x, pts[i].y);
		}
		ofEndShape();

		if (m_run) {
			ofBeginShape();
			ofSetColor(0, 0, 0);
			ofFill();
			ofEllipse(pts[m_BallPos].x, pts[m_BallPos].y, 15, 15);
			ofEndShape();
		}

		ofEnableAlphaBlending();
		ofBeginShape();
		for ( int i = 0; i < MAX_SAMPLES; ++i )
		{
			for (int j = 0; j < m_numSamples[i]; ++j)
			{
				ofSetColor(m_sampleColors[i].r, m_sampleColors[i].g, m_sampleColors[i].b, m_sampleColors[i].a);
				ofFill();
				ofEllipse(m_sampleVector[i][j].x, m_sampleVector[i][j].y, 40, 40);
			}
		}
		ofEndShape();
		// -----------------------------------------------------------------------------

		ofSetColor(0, 0, 0);
		std::string text = "Current sample: ";
		std::stringstream strstr;
		strstr << text;
		strstr << m_currentSample;
		ofDrawBitmapString(strstr.str(), 10, 10);
		ofSetColor(m_sampleColors[m_currentSample].r, m_sampleColors[m_currentSample].g, m_sampleColors[m_currentSample].b);
		ofFill();
		ofRect(150, 1, 10,10);
		if (showColors) {
			for (int i = 0; i < MAX_SAMPLES; ++i) {
				ofSetColor(m_sampleColors[i].r, m_sampleColors[i].g, m_sampleColors[i].b, 200);
				ofRect(40 * i + 20 , 40, 40, 40);
				std::stringstream strstr;
				strstr << i;
				ofDrawBitmapString(strstr.str(), 40*i + 20, 90);
			}

		}
		if (helpMode) {
			ofSetColor(0, 0, 0);
			std::stringstream stream;
			stream << "Keyboard navigation:\n\n";
			stream << "Start/Stop (play mode/draw mode):---- space\n";
			stream << "Set current sample:------------------ 0 - 9\n";
			stream << "Remove all dots for current sample:-- r\n";
			stream << "Remove last dot from current sample:- z\n";
			stream << "Put last removed dot back:----------- shift + z\n";
			stream << "Remove all dots:--------------------- c\n";
			stream << "Remove all dots and line:------------ shift + c\n";
			stream << "Fullscreen mode:--------------------- shift + F\n";
			stream << "Show sample colors:------------------ f\n";
			stream << "Change sample colors:---------------- g\n";
			stream << "Debug mode:-------------------------- d\n";
			stream << "Switch between saved states:--------- + / -\n";
			stream << "Save current state (debug mode):----- w\n";
			stream << "Reload states (debug mode):---------- e\n";
			stream << "Show about screen:------------------- return\n";
			stream << "This menu:--------------------------- h\n";
			ofDrawBitmapString(stream.str(), 10, 150);
		}
		if (debugMode)
		{
			if (buttonPushed)
				ofSetColor(0, 200, 0);
			else
				ofSetColor(200, 0, 0);
			

			//ofNoFill();
			ofRect(10, ofGetHeight()-130, 20, 20);
			//ofFill();
			ofSetColor(0, 0, 0);
			ofDrawBitmapString("push red square to save current state or 'w' on your keyboard.", 40, ofGetHeight()-116);
			ofSetColor(0, 0, 0);
			std::string texttext = "Current state: ";
			std::stringstream strstrstr;
			strstrstr << texttext;
			strstrstr << m_currentState;
			strstrstr << ", Saved states: ";
			strstrstr << m_stateLoadMessage;
			strstrstr << "\n";
			ofDrawBitmapString(strstrstr.str(), 10, ofGetHeight()-90);
			
			std::stringstream strstrstrstr;
			strstrstrstr << m_debugMessage;
			if (m_debugMessage != "") {
			}
			ofDrawBitmapString(strstrstrstr.str(), 10, ofGetHeight() - 50);
			

		}
		if (debugMode || helpMode) {
			ofSetColor(0, 0, 0);
			ofDrawBitmapString("Peter Vasil, 2010", ofGetWidth()-150, ofGetHeight()-10);
		}
	}
}
//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	//if (debugOutput) {printf("Pressed key number: %i\n", key);}
	if ( key == ' ' )
	{
		if ( m_run )
		{
			m_run = false;
			ofxOscMessage m;
			m.setAddress("/stop");
			m.addIntArg(0);
			sender.sendMessage ( m );
			setOscDebugMessage(m);
		}
		else
		{
			m_run = true;
			ofxOscMessage m;
			m.setAddress("/start");
			m.addIntArg(1);
			sender.sendMessage ( m );
			setOscDebugMessage(m);
		}
	}
	else if (key == 'r' || key == 'R')
	{
		m_numSamples[m_currentSample] = 0;
		m_sampleVector[m_currentSample].clear();
	}
	else if (key == 'z')
	{
		if (m_numSamples[m_currentSample] > 0)
		{
			--m_numSamples[m_currentSample];
			m_sampleVectorPopped[m_currentSample].push_back(m_sampleVector[m_currentSample].back());
			m_sampleVector[m_currentSample].pop_back();
		}
	}
	else if (key == 'Z')
	{
		if (m_sampleVectorPopped[m_currentSample].size() > 0) {
			m_sampleVector[m_currentSample].push_back(m_sampleVectorPopped[m_currentSample].back());
			m_sampleVectorPopped[m_currentSample].pop_back();
			++m_numSamples[m_currentSample];
		}
	}
	else if ( key == 'c' )
	{
		// clear all samples
		for (int i = 0; i < MAX_SAMPLES; ++i) {
			m_numSamples[i] = 0;
			m_sampleVector[i].clear();
		}
	}
	else if ( key == 'C' )
	{
		clearAll();
	}
	else if ( key == 'F' )
	{
		if (isFullScreen)
		{
			ofSetFullscreen(false);
			isFullScreen = false;
		}
		else
		{
			ofSetFullscreen(true);
			isFullScreen = true;
		}
	}
	else if (key >= 48 && key <=57)
	{
		m_currentSample = key - 48;
		if (debugOutput) {printf("set sample: %i\n", m_currentSample);}
	}
	else if (key == 'd')
	{
//		if (!helpMode) {
			if (debugOutput) {printf("debug mode\n");}
			debugMode = !debugMode;
//		}
	}
	else if (key == 'f')
	{
		showColors = !showColors;
	}
	else if (key == 'g')
	{
		for (int i = 0; i < MAX_SAMPLES; ++i) {
			m_sampleColors[i].r = ofRandom(0, 255);
			m_sampleColors[i].g = ofRandom(0, 255);
			m_sampleColors[i].b = ofRandom(0, 255);
			m_sampleColors[i].a = 127;
		}
	}
	else if (key == 'h')
	{
//		if (!debugMode) {
			if (debugOutput) {printf("help mode\n");}
			helpMode = !helpMode;
//		}
	}
	else if (key == 'm')
	{
		showOscDebugPosition = !showOscDebugPosition;
	}
	else if (key == OF_KEY_UP)
	{
		if (m_currentState == 0) {
			nPtsBackup = nPts;
			for (int i = 0; i < MAX_N_PTS; ++i) {
				ptsBackup[i] = 0;
			}

			for (int i = 0; i < nPts; ++i) {
				ptsBackup[i] = pts[i];
			}
			for (int i = 0; i < MAX_SAMPLES; ++i) {
				m_sampleVectorBackup[i] = m_sampleVector[i];
				m_numSamplesBackup[i] = m_numSamples[i];
				m_playStatusesBackup[i] = m_playStatuses[i];
			}
		}
		++m_currentState;

		if (m_currentState > XMLstates.getNumTags("STATE"))
		{
			m_currentState = 0;
		}
		loadState(m_currentState);
		if (debugOutput) {printf("current state : %i keyup\n", m_currentState);}

	}
	else if (key == OF_KEY_DOWN)
	{
		if (m_currentState == 0) {
			nPtsBackup = nPts;
			for (int i = 0; i < MAX_N_PTS; ++i) {
				ptsBackup[i] = 0;
			}

			for (int i = 0; i < nPts; ++i) {
				ptsBackup[i] = pts[i];
			}
			for (int i = 0; i < MAX_SAMPLES; ++i) {
				m_sampleVectorBackup[i] = m_sampleVector[i];
				m_numSamplesBackup[i] = m_numSamples[i];
				m_playStatusesBackup[i] = m_playStatuses[i];
			}
		}
		--m_currentState;
		if (m_currentState < 0)
		{
			m_currentState =  XMLstates.getNumTags("STATE");
		}
		loadState(m_currentState);
		if (debugOutput) {printf("current state : %i keydown\n", m_currentState);}
	}
	else if (key == OF_KEY_RETURN)
	{
		usageMode = !usageMode;
	}
	else if (key == 'w')
	{
		if (debugMode) {
			saveCurrentState();
			if (debugOutput) {printf("saved current state!\n");}
			buttonPushed = true;
		}
	}
	else if (key == 'e')
	{
		if (debugMode)
		{
			readStates();
		}
	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
	if (buttonPushed) {
		buttonPushed = false;
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if ( m_run )
	{
	}
	else
	{
		if (nPts < MAX_N_PTS)
		{
			pts[nPts].x = x;
			pts[nPts].y = y;
			nPts++;
			nPtsBackup = nPts;
		}
	}


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	int h = ofGetHeight();
	if (debugMode)
	{
		//ofRect(10, ofGetHeight()-100, 20, 20);

		if (x >= 10 && x <= 30 && y >= (h-130) && y <= (h-110) ) {
			buttonPushed = true;
			saveCurrentState();
			//clearAll();
			if (debugOutput) {printf("saved current state!\n");}
		}
	}
	if ( m_run && m_currentSample < MAX_SAMPLES)
	{
		if (x >= 10 && x <= 30 && y >= (h-130) && y <= (h-110) ) 
		{
		}
		else
		{
			ofxVec3f tmpVec;
			tmpVec.x = x;
			tmpVec.y = y;
			m_sampleVector[m_currentSample].push_back(tmpVec);
			//m_sampleVectorBackup[m_currentSample].push_back(tmpVec);

			++m_numSamples[m_currentSample];
			m_playStatuses[m_currentSample].push_back(false);
			//m_playStatusesBackup[m_currentSample].push_back(false);
		}
	}
	else
	{
		nPts = 0;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased()
{
	if (buttonPushed) {
		buttonPushed = false;
	}

}

void testApp::checkPlayPos(int x, int y)
{
	for (int j = 0; j<MAX_SAMPLES; ++j)
	{
		if (m_numSamples[j] > 0) {
			for ( unsigned int i = 0; i < m_sampleVector[j].size(); ++i )
			{
				if ( (x > m_sampleVector[j][i].x - 40 && x < m_sampleVector[j][i].x + 40) &&
					(y > m_sampleVector[j][i].y - 40 && y < m_sampleVector[j][i].y + 40) )
				{
					if(isPlaying && !m_playStatuses[j][i])
					{
						ofxOscMessage m;
						m.setAddress( "/play" );
						//m.addStringArg("sample");
						m.addIntArg(j);
						sender.sendMessage ( m );
						isPlaying = false;
						m_playStatuses[j][i] = true;
						setOscDebugMessage(m);
					}
				}
				else
				{
					isPlaying = true;
					m_playStatuses[j][i] = false;
				}
			}
		}
	}
}

void testApp::setOscDebugMessage(ofxOscMessage message)
{
	std::stringstream stream;
	stream << "OSC messages";
	if (!showOscDebugPosition)
	{
		stream << " (Not showing position messages)";
	}
	stream << " :\n";
	stream << "[address]: " << message.getAddress() << ", ";
	for (int i = 0; i < message.getNumArgs(); ++i) {
		stream << "[" << (i+1) << "]: ";
		if (message.getArgType(i) == OFXOSC_TYPE_INT32) {
			stream << message.getArgAsInt32(i);
		}
		else if (message.getArgType(i) == OFXOSC_TYPE_STRING) {
			stream << message.getArgAsString(i);
		}
		else if (message.getArgType(i) == OFXOSC_TYPE_FLOAT) {
			stream << message.getArgAsFloat(i);
		}
		if (i != message.getNumArgs()-1) {stream << ", ";};
	}
	stream << ", time: ";
	stream << ofGetElapsedTimef();
	m_debugMessage = stream.str();
}

void testApp::loadState(int state)
{
	if (state == 0)
	{
		nPts = nPtsBackup;
		for (int i = 0; i < nPts; ++i) {
			pts[i] = ptsBackup[i];
		}
		for (int i = 0; i < MAX_SAMPLES; ++i) {
			m_sampleVector[i].clear();
			m_playStatuses[i].clear();
			m_sampleVector[i] = m_sampleVectorBackup[i];
			m_numSamples[i] = m_numSamplesBackup[i];
			m_playStatuses[i] = m_playStatusesBackup[i];
		}
	}
	else
	{
		int numStateTags = XMLstates.getNumTags("STATE:NUMBER");
		if(numStateTags > 0)
		{
			//XML.pushTag("STATE", numStateTags-1);
			XMLstates.pushTag("STATE", state-1);

			int stateNum = XMLstates.getValue("NUMBER", 0);
			if (debugOutput) {printf("NUMBER: %i\n", stateNum);}
			//XMLstates.pushTag(, )
			//int numPtTags = XMLstates.getNumTags("LINE:PT");
			XMLstates.pushTag("LINE", 0);
			nPts = XMLstates.getValue("PTNUM", 0);
			if (debugOutput) {printf("nPts: %i\n", nPts);}
			int numPtTags = XMLstates.getNumTags("PT");
			if (debugOutput) {printf("num PT: %i\n", numPtTags);}
			for (int i = 0; i < MAX_N_PTS; ++i) {
				pts[i].x = 0;
				pts[i].y = 0;
			}
			for (int i = 0; i < numPtTags; ++i) {
				pts[i].x = XMLstates.getValue("PT:X", 0, i);
				pts[i].y = XMLstates.getValue("PT:Y", 0, i);
				if (debugOutput) {printf("ptsX: %f, ptsY: %f\n", pts[i].x, pts[i].y);}
			}
			for (int i = 0; i < MAX_SAMPLES; ++i) {
				//m_sampleVectorBackup[i] = m_sampleVector[i];
				m_sampleVector[i].clear();
				//m_playStatusesBackup[i] = m_playStatuses[i];
				m_playStatuses[i].clear();
				m_numSamples[i] = 0;
			}
			XMLstates.popTag();
			int numSampleTags = XMLstates.getNumTags("SAMPLE");
			if (debugOutput) {printf("num SAMPLE: %i\n", numSampleTags);}
			for (int i = 0; i < numSampleTags; ++i) {
				int snum = XMLstates.getValue("SAMPLE:NUMBER",0,i);
				int amount = XMLstates.getValue("SAMPLE:AMOUNT", 0, i);
				if (debugOutput) {printf("SAMPLE number: %i\n", snum);}
				if (debugOutput) {printf("AMOUNT: %i\n", amount);}
				m_sampleVector[snum].clear();
				//m_playStatuses[snum].clear();
				XMLstates.pushTag("SAMPLE", i);
				for (int j = 0; j < amount; ++j) {
					//XMLstates.pushTag("SAMPLE", j);
					ofxVec3f tmpVec;
					int x = XMLstates.getValue("DOT:X", 0, j);
					int y = XMLstates.getValue("DOT:Y", 0, j);
					if (debugOutput) {printf("sample vector x: %i, y: %i\n", x, y);}
					tmpVec.set(x, y,0);
					m_sampleVector[snum].push_back(tmpVec);
					m_playStatuses[snum].push_back(false);
					//m_playStatuses[snum].push_back(true);

				}
				m_numSamples[snum] = amount;
				XMLstates.popTag();
			}
			XMLstates.popTag();
		}
	}

}

void testApp::saveCurrentState()
{
	lastTagNumber = XMLstates.addTag("STATE");
	if( XMLstates.pushTag("STATE", lastTagNumber) )
	{
		XMLstates.addTag("NUMBER");
		XMLstates.setValue("NUMBER", lastTagNumber+1, 0);
		//now we will add a pt tag - with two
		//children - X and Y
		XMLstates.addTag("LINE");
		XMLstates.pushTag("LINE", 0);
		XMLstates.addTag("PTNUM");
		XMLstates.setValue("PTNUM", nPts);
		for (int i = 0; i < nPts; ++i)
		{
			int tagNum = XML.addTag("PT");
			XMLstates.setValue("PT:X", pts[i].x, i);
			XMLstates.setValue("PT:Y", pts[i].y, i);
		}
		XMLstates.popTag();
		for (int i = 0; i<MAX_SAMPLES; ++i)
		{
			int tagNum = 0;
			if (m_sampleVector[i].size() > 0)
			{
				tagNum = XMLstates.addTag("SAMPLE");
				XMLstates.pushTag("SAMPLE", tagNum);
				XMLstates.addTag("NUMBER");
				XMLstates.setValue("NUMBER", i, 0);
				XMLstates.addTag("AMOUNT");
				XMLstates.setValue("AMOUNT", (int)m_sampleVector[i].size(), 0);
				for (unsigned int j = 0; j < m_sampleVector[i].size(); ++j)
				{
					int dotTagNum = XMLstates.addTag("DOT");
					float x = m_sampleVector[i][j].x;
					float y = m_sampleVector[i][j].y;
					XMLstates.setValue("DOT:X", x, j);
					XMLstates.setValue("DOT:Y", y, j);
				}
				XMLstates.popTag();
			}
		}
		XMLstates.popTag();
	}
	XMLstates.saveFile(m_savedStatesFileName);
	std::stringstream strstr;
	strstr << XMLstates.getNumTags("STATE");
	m_stateLoadMessage = strstr.str();
}
void testApp::readStates()
{
	XML.loadFile("AppSettings.xml");
	m_savedStatesFileName = XML.getValue("settings:states:file", "savedStatesDefault.xml");
	XMLstates.loadFile(m_savedStatesFileName);
	std::stringstream tags;
	tags << XMLstates.getNumTags("STATE");
	m_stateLoadMessage = tags.str();
	if(debugOutput){std::cout << "Reloaded states file with " << m_stateLoadMessage << " states" << std::endl;};
}

void testApp::clearAll()
{
	nPts = 0;
	isPlaying = true;
	m_run = false;
	m_currentState=0;
	std::stringstream strstr;
	strstr << XMLstates.getNumTags("STATE");
	m_stateLoadMessage = strstr.str();

	for (int i = 0; i < MAX_N_PTS; ++i) {
		pts[i] = 0;
	}
	for (int i = 0; i < MAX_SAMPLES; ++i) {
		m_numSamples[i] = 0;
		m_sampleVector[i].clear();
		m_playStatuses[i].clear();
	}
}
