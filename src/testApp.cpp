#include "testApp.h"

void testApp::closeApp()
{
	// deallocate	
	OF_EXIT_APP(0);
}

//--------------------------------------------------------------
void testApp::setup()
{
	bSetup				= false;

	string video_dir	= ofToDataPath("video");
	dirList.open(video_dir.c_str());
	
	numFiles			= dirList.listDir();
	videoFiles			= dirList.getFiles();
	if(numFiles == 0)
	{
		printf("[ERROR] No files found in %s\n", video_dir);
		OF_EXIT_APP(0);
	}
	else {
		printf("[OK] Read %d files\n", numFiles);
		for (int i = 0; i < numFiles; i++) {
			printf("%d.) %s\n", i+1, videoFiles[i].getAbsolutePath().c_str());
		}
	}
	
	currentFile			= 0;
	videoReader			= new ofVideoPlayer();
	videoReader->loadMovie(videoFiles[currentFile].getAbsolutePath());
	totalFrames			= videoReader->getTotalNumFrames();
	currentFrame		= 0;
	
	imageScalar			= 2;
	
	cvColorImg.allocate(videoReader->getWidth(), videoReader->getHeight());
	cvGrayImg.allocate(videoReader->getWidth(), videoReader->getHeight());
	cvGrayImgResized.allocate(videoReader->getWidth()/imageScalar, videoReader->getHeight()/imageScalar);
	
	detector			= FeatureDetector::create("DynamicSURF");
	extractor			= DescriptorExtractor::create("SURF");
	matcher				= DescriptorMatcher::create("FlannBased");
	
	int dictionarySize	= 1000;
	TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
	int retries			= 1;
	int flags			= KMEANS_PP_CENTERS;
	trainer				= new BOWKMeansTrainer(dictionarySize,tc,retries,flags);
	totalKeypoints		= 0;
	
	ofSetVerticalSync(false);
	ofSetWindowShape(1000, 520);
	ofSetFrameRate(1000);
}

bool testApp::readVocabulary( const string& filename, Mat& vocabulary )
{
    printf("Reading vocabulary...");
    FileStorage fs( filename, FileStorage::READ );
    if( fs.isOpened() )
    {
        fs["vocabulary"] >> vocabulary;
        printf("done.\n");
        return true;
    }
    return false;
}

bool testApp::writeVocabulary( const string& filename, const Mat& vocabulary )
{
    printf("Saving vocabulary...");
    FileStorage fs( filename, FileStorage::WRITE );
    if( fs.isOpened() )
    {
        fs << "vocabulary" << vocabulary;
        printf("done.\n");
        return true;
    }
    return false;
}

//--------------------------------------------------------------
void testApp::update(){
	
	if (currentFrame < totalFrames) 
	{
		videoReader->setFrame(currentFrame);
		cvColorImg.setFromPixels(videoReader->getPixels(), videoReader->getWidth(), videoReader->getHeight());
		cvGrayImg = cvColorImg;
		cvGrayImgResized.scaleIntoMe(cvGrayImg, CV_INTER_LINEAR);
		
		// create Mat header
		Mat img = cvGrayImgResized.getCvImage();
		
		// get keypoints
		detector->detect(img, keypoints);
		totalKeypoints += keypoints.size();

		// get descriptors
		extractor->compute(img, keypoints, descriptors);
		
		/*
		// add to kdtree
		vector<Mat> d;
		d.push_back(descriptors);
		matcher->add(d);
		*/
		
		trainer->add(descriptors);
		
		/*
		
		BOWImgDescriptorExtractor bowDE(extractor,matcher);
		
		int dictionarySize=1000;
		TermCriteria tc(CV_TERMCRIT_ITER,100,0.001);
		int retries=1;
		int flags=KMEANS_PP_CENTERS;
		BOWKMeansTrainer bowTrainer(dictionarySize,tc,retries,flags);
		// cluster the feature vectors
		Mat dictionary=bowTrainer.cluster(featuresUnclustered);
		//Set the dictionary we created in the first step
		bowDE.setVocabulary(dictionary);
		
		
		
		//Create the BoW representation of the image
		Mat bowDescriptor;
		bowDE.compute(img,keypoints,bowDescriptor);
		*/
		
		
		currentFrame++;
	}
	else {
		if (currentFile+1 < numFiles) {
			// close current file
			videoReader->closeMovie();
			videoReader->close();
			delete videoReader;
			
			// open the next one
			currentFile++;
			videoReader = new ofVideoPlayer();
			videoReader->loadMovie(videoFiles[currentFile].getAbsolutePath());
			currentFrame = 0;
			totalFrames = videoReader->getTotalNumFrames();
			cvColorImg.resize(videoReader->getWidth(), videoReader->getHeight());
			cvGrayImg.resize(videoReader->getWidth(), videoReader->getHeight());
			cvGrayImgResized.resize(videoReader->getWidth()/imageScalar, videoReader->getHeight()/imageScalar);
		}
		else {
			printf("[OK] Finished parsing %d video files\n", currentFile);
			delete videoReader;
			
			Mat vocabulary = trainer->cluster();
			
			writeVocabulary(ofToDataPath("vocabulary.mat"), vocabulary);
			
			OF_EXIT_APP(0);
		}

	}

}


//--------------------------------------------------------------
void testApp::draw() {
	
	ofBackground(0);
	ofSetColor(255, 255, 255);
	
	// draw movie frame
	cvColorImg.draw(20, 20);
	
	// draw keypoints
	ofNoFill();
	for (vector<KeyPoint>::iterator it = keypoints.begin(); it != keypoints.end(); ++it) {
		ofPushMatrix();
		ofTranslate(it->pt.x * imageScalar, it->pt.y * imageScalar, 1);
		ofRotate(it->angle, 0, 0, 1);
		ofRect(0, 0, it->size, it->size);
		ofPopMatrix();
	}
	
	// draw bounding rect
	ofRect(20, 20, videoReader->getWidth(), videoReader->getHeight());
	
	// stats
	char buf[256];
	float fps = ofGetFrameRate();
	sprintf(buf, "fps: %02.2f", fps);
	ofDrawBitmapString(buf, ofPoint(20,470));
	
	sprintf(buf, "frame: %d/%d", currentFrame, totalFrames);
	ofDrawBitmapString(buf, ofPoint(20,485));
	
	sprintf(buf, "keypoints %d (this frame), %f (average), %d (total)", 
			keypoints.size(), totalKeypoints / (float)currentFrame, totalKeypoints );
	ofDrawBitmapString(buf, ofPoint(20,500));
	
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

