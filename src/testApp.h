#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxDirList.h"
#include "pkmEXTAudioFileReader.h"
#include <vector>
using namespace std;

class testApp : public ofBaseApp{

	public:

	void closeApp();
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
	
	pkmEXTAudioFileReader		audioFileReader;
	int							currentSampleNumber, frameSize, frame;
	float						*current_frame;
	
	ofxDirList					dirList;
	
	int							currentFile, numFiles;
	vector<ofFile>				audioFiles;
	
	bool						bSetup;
};

#endif
