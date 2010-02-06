#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main( ){

	//Test from master 2
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	//ofAppGlutWindow window;
	ofSetupOpenGL(/*&window, */800,600, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	ofRunApp(new testApp);
	//Test tester
}
