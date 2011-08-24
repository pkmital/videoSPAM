#include "testApp.h"

void testApp::closeApp()
{
	// deallocate
	free(current_frame);
	
	OF_EXIT_APP(0);
}

//--------------------------------------------------------------
void testApp::setup(){
	bSetup = false;
	frameSize = 512;

	string audio_dir = ofToDataPath("audio");
	dirList.open(audio_dir.c_str());
	
	numFiles = dirList.listDir();
	if(numFiles == 0)
	{
		printf("[ERROR] No files found in %s\n", audio_dir);
		OF_EXIT_APP(0);
	}
	else {
		printf("[OK] Read %d files\n", numFiles);
	}

	audioFiles = dirList.getFiles();
	
	currentFile = 0;
	
	audioFileReader.open(audioFiles[currentFile].getAbsolutePath());
	current_frame = (float *)malloc(sizeof(float) * frameSize);
	frame = 0;
	
	ofSetVerticalSync(false);
	ofSetWindowShape(1000, 520);
	ofSetFrameRate(500);
}

//--------------------------------------------------------------
void testApp::update(){
	
	if (currentSampleNumber < audioFileReader.mNumSamples) 
	{
		audioFileReader.read(current_frame, (frame)*frameSize, frameSize);
		// segment
		
		
		
		frame++;
		
	}
	else {
		if (currentFile < numFiles) {
			// close current file
			audioFileReader.close();
			
			// open the next one
			currentFile++;
			audioFileReader.open(audioFiles[currentFile].getAbsolutePath());
			frame = 0;
		}
		else {
			printf("[OK] Finished parsing %d audio files\n", currentFile);
		}

	}

}


//--------------------------------------------------------------
void testApp::draw() {
		
	float fps = ofGetFrameRate();
	
	char buf[256];
	
	sprintf(buf, "fps: %02.2f", fps);
	ofDrawBitmapString(buf, ofPoint(20,470));
	
	sprintf(buf, "processed: %d/%d", frame, audioFileReader.mNumSamples / frameSize);
	ofDrawBitmapString(buf, ofPoint(20,485));
	
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::resized(int w, int h){

}

