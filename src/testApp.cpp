#include "testApp.h"
#include <iostream>
//--------------------------------------------------------------
void testApp::setup(){

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
	debugMode = false;
	showColors = false;
	helpMode = false;
	int dbgPos = XML.getValue("settings:debug:position", 1);
	if (dbgPos > 0) 
		showOscDebugPosition = true;
	else 
		showOscDebugPosition = false;

	
	m_currentSample = 0;

	for (int i = 0; i < MAX_SAMPLES; ++i) {
		m_sampleColors[i].r = ofRandom(0, 255);
		m_sampleColors[i].g = ofRandom(0, 255);
		m_sampleColors[i].b = ofRandom(0, 255);
		m_sampleColors[i].a = 200;
		m_numSamples[i] = 0;
//		m_sampleVector[i].push_back(std::vector<ofxVec3f> samples);
	}
	
	
	message = "loading savedStates.xml";
    std::cout << message << std::endl;
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( XMLstates.loadFile("savedStates.xml") ){
		message = "savedStates.xml loaded!";
	}else{
		message = "unable to load savedStates.xml check data/ folder";
	}
	std::cout << message << std::endl;
	
	
	lastTagNumber = 0;
	
	
	m_stateLoadMessage = "0";
	//ofSetFrameRate(20);
//	ofSoundStreamSetup(0,2,this, 44100, 256, 4);
//	left = new float[256];
//	right = new float[256];
//
//	bufferCounter = 0;
//	drawCounter = 0;
}

//--------------------------------------------------------------
void testApp::update(){

	if (m_run)
	{
		//int prevTime = m_runInterval;
//		m_runInterval = ofGetElapsedTimeMillis();
//		float dt =(float) m_runInterval - (float)m_prevTime;
//		m_prevTime = m_runInterval;
//		if ( dt > 2 )
//		{
//			++m_BallPos;
//		}
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
//	// draw the left:
//	ofSetColor(0x333333);
//	ofRect(100,100,256,200);
//	ofSetColor(0xFFFFFF);
//	for (int i = 0; i < 256; i++){
//		ofLine(100+i,200,100+i,200+left[i]*100.0f);
//	}
//
//	// draw the right:
//	ofSetColor(0x333333);
//	ofRect(600,100,256,200);
//	ofSetColor(0xFFFFFF);
//	for (int i = 0; i < 256; i++){
//		ofLine(600+i,200,600+i,200+right[i]*100.0f);
//	}
//
//
//
//	ofSetColor(0x333333);
//	drawCounter++;
//	char reportString[255];
//	sprintf(reportString, "buffers received: %i\ndraw routines called: %i\n", bufferCounter,drawCounter);
//	ofDrawBitmapString(reportString,80,380);


	//=========================================
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
			ofSetColor(m_sampleColors[i].r, m_sampleColors[i].g, m_sampleColors[i].b, 127);
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
			ofSetColor(m_sampleColors[i].r, m_sampleColors[i].g, m_sampleColors[i].b);
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
		stream << "Start/Stop (play mode/draw mode):---- s\n";
		stream << "Set current sample:------------------ 0 - 9\n";
		stream << "Remove all dots for current sample:-- r\n";
		stream << "Remove last dot from current sample:- z\n";
		stream << "Put last removed dot back:----------- shift + z\n";
		stream << "Remove all dots:--------------------- c\n";
		stream << "Fullscreen mode:--------------------- shift + F\n";
		stream << "Show sample colors:------------------ f\n";
		stream << "Change sample colors:---------------- g\n";
		stream << "Debug mode:-------------------------- d\n";
		stream << "This menu:--------------------------- h\n";
		ofDrawBitmapString(stream.str(), 10, 130);
	}
	if (debugMode)
	{
		ofSetColor(255, 0, 0);
		//ofNoFill();
		ofRect(10, ofGetHeight()-100, 20, 20);
		//ofFill();
		ofSetColor(0, 0, 0);
		ofDrawBitmapString("push square to save current state. "+m_stateLoadMessage+" states saved.", 40, ofGetHeight()-86);
//		stream << "OSC messages:\n";
//		for (unsigned int i; i < m_debugMessages.size(); ++i) {
//			stream << "[" << i << "] :";
//			stream << m_debugMessages[i];
//			if (i != m_debugMessages.size()-1) {stream << ", ";};
//		}
		ofDrawBitmapString(m_debugMessage, 10, ofGetHeight() - 50);
	
	}
}
//static float adder = 0;
////--------------------------------------------------------------
//void testApp::audioReceived 	(float * input, int bufferSize, int nChannels){
//	// samples are "interleaved"
//	for (int i = 0; i < bufferSize; i++){
//		left[i] = input[i*2];
//		right[i] = input[i*2+1];
//	}
//	bufferCounter++;
//
//}
//--------------------------------------------------------------
void testApp::keyPressed  (int key){

	//printf("Pressed key number: %i\n", key);
	if ( key == 's' || key == 'S' )
	{
		if ( m_run )
		{
			m_run = false;
			ofxOscMessage m;
			m.setAddress("/test");
			m.addIntArg(0);
			sender.sendMessage ( m );
			setOscDebugMessage(m);
		}
		else
		{
			m_run = true;
			ofxOscMessage m;
			m.setAddress("/test");
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
	else if ( key == 'c' || key == 'C' )
	{
		// clear all samples
		for (int i = 0; i < MAX_SAMPLES; ++i) {
			m_numSamples[i] = 0;
			m_sampleVector[i].clear();
		}
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
		printf("in set sample\n");
		m_currentSample = key - 48;
	}
	else if (key == 'd') 
	{
		debugMode = !debugMode;
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
			m_sampleColors[i].a = 200;
		}
	}
	else if (key == 'h') 
	{
		printf("helpmode\n");
		helpMode = !helpMode;
	}
	else if (key == 'm') 
	{
		showOscDebugPosition = !showOscDebugPosition;
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

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
		}
	}


}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	if (debugMode) 
	{
		//ofRect(10, ofGetHeight()-100, 20, 20);
		int h = ofGetHeight();
		if (x >= 10 && x <= 30 && y >= (h-100) && y <= (h-80) ) {
			printf("pushed save square!\n");
			saveCurrentState();
		}
	}
	else 
	{
		if ( m_run && m_currentSample < MAX_SAMPLES)
		{
			ofxVec3f tmpVec;
			tmpVec.x = x;
			tmpVec.y = y;
			m_sampleVector[m_currentSample].push_back(tmpVec);
			
			//m_samplVec.push_back(tmpVec);
			//		m_samplColorR = (float)ofRandom(0.0, 255.0f);
			//		m_samplColorG = (float)ofRandom(0.0, 255.0f);
			//		m_samplColorB = (float)ofRandom(0.0, 255.0f);
			//		m_vecColorR.push_back(m_samplColorR);
			//		m_vecColorG.push_back(m_samplColorG);
			//		m_vecColorB.push_back(m_samplColorB);
			++m_numSampls;
			++m_numSamples[m_currentSample];
			playStatus.push_back(false);
			m_playStatuses[m_currentSample].push_back(false);
		}
		else
		{
			nPts = 0;
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased()
{


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
					//			ofxOscMessage m;
					//			m.setAddress( "/play" );
					//			m.addIntArg(0);
					//			sender.sendMessage ( m );
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
		stream << "[" << (i+1) << "] :";				
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
	m_debugMessage = stream.str();
}

void testApp::loadState(int state)
{
	//-----------
	//the string is printed at the top of the app
	//to give the user some feedback
	message = "loading mySettings.xml";
	
	//we load our settings file
	//if it doesn't exist we can still make one
	//by hitting the 's' key
	if( XMLstates.loadFile("savedStates.xml") ){
		message = "savedStates.xml loaded!";
	}else{
		message = "unable to load savedStates.xml check data/ folder";
	}
	
	int numStateTags = XMLstates.getNumTags("STATE:NUMBER");
	
	if(numStateTags > 0 && numStateTags > state)
	{
		//XML.pushTag("STATE", numStateTags-1);
		XMLstates.pushTag("STATE", state-1);
		
		int stateNum = XMLstates.getValue("NUMBER", 0, state);
		
		//XMLstates.pushTag(, )
		int numPtTags = XMLstates.getNumTags("LINE:PT");
		
	}
}

void testApp::saveCurrentState()
{
	if (m_currentState == 0) 
	{
		lastTagNumber = XMLstates.addTag("STATE");
		if( XMLstates.pushTag("STATE", lastTagNumber) )
		{
			
			XMLstates.addTag("NUMBER");
			XMLstates.setValue("NUMBER", lastTagNumber, 0);
			//now we will add a pt tag - with two
			//children - X and Y
			XMLstates.addTag("LINE");
			XMLstates.pushTag("LINE", 0);
			
			for (int i = 0; i < MAX_N_PTS; ++i) 
			{
				if (pts[i].x != 0 && pts[i] != 0) {
					int tagNum = XML.addTag("PT");
					XMLstates.setValue("PT:X", pts[i].x, tagNum);
					XMLstates.setValue("PT:Y", pts[i].y, tagNum);					
				}
				else 
				{
					break;
				}
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
						XMLstates.setValue("DOT:X", x, dotTagNum);
						XMLstates.setValue("DOT:y", y, dotTagNum);
					}
					XMLstates.popTag();
				}
			}
			
			XMLstates.popTag();
			
		}
		XMLstates.saveFile("savedStates.xml");
		std::stringstream strstr;
		strstr << lastTagNumber;
		m_stateLoadMessage = strstr.str();
	}
}
