/*=======================================================
 Copyright (c) Peter Vasil, 2010
=======================================================*/

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "ofxOsc.h"
#include "ofxVectorMath.h"
#include "ofxXmlSettings.h"

#define MAX_N_PTS         1500

#define HOST "localhost"
#define PORT 57120

#define MAX_SAMPLES 10

#include <vector>

class testApp : public ofSimpleApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	void checkPlayPos(int x, int y);
	void setOscDebugMessage(ofxOscMessage message);
	void saveCurrentState();
	void loadState(int state);

//	void audioReceived 	(float * input, int bufferSize, int nChannels);
//
//	float * left;
//	float * right;
//	int 	bufferCounter;
//	int 	drawCounter;

	ofxVec3f	pts[MAX_N_PTS];
	ofxVec3f	ptsBackup[MAX_N_PTS];

	int			nPts;
	int			nPtsBackup;

//private:
	ofxOscSender sender;
	ofxXmlSettings XML;
	ofxXmlSettings XMLstates;

    string oschost;
    int oscport;
    string xmlStructure;
	string message;

	bool m_run;
	int m_BallPos;
	int m_runInterval;
	int m_prevTime;
	int m_numSampls;
	int m_numSamples[MAX_SAMPLES];
	
	std::vector<ofxVec3f> m_samplVec;
	std::vector<ofxVec3f> m_sampleVector[MAX_SAMPLES];
	std::vector<ofxVec3f> m_sampleVectorBackup[MAX_SAMPLES];
	
	std::vector<ofxVec3f> m_sampleVectorPopped[MAX_SAMPLES];
	std::vector<int> m_vecColorR;
	std::vector<int> m_vecColorG;
	std::vector<int> m_vecColorB;
	int m_samplColorR;
	int m_samplColorG;
	int m_samplColorB;
	bool isFullScreen;
	bool isPlaying;
	bool debugMode;
	bool showColors;
	bool helpMode;
	std::vector<bool> playStatus;
	std::vector<bool> m_playStatuses[MAX_SAMPLES];
	std::vector<bool> m_playStatusesBackup[MAX_SAMPLES];

	int m_currentSample;
	ofColor m_sampleColors[MAX_SAMPLES];
	
	std::string m_debugMessage;
	bool showOscDebugPosition;
	
	int m_currentState;
	std::string m_stateLoadMessage;
	
	int lastTagNumber;

};

#endif

