/*
 *  Created by Parag K. Mital - http://pkmital.com 
 *  Contact: parag@pkmital.com
 *
 *  Copyright 2011 Parag K. Mital. All rights reserved.
 * 
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without
 *	restriction, including without limitation the rights to use,
 *	copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the
 *	Software is furnished to do so, subject to the following
 *	conditions:
 *	
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *	OTHER DEALINGS IN THE SOFTWARE.
 */

#include "testApp.h"

void testApp::closeApp()
{
    // deallocate	
	OF_EXIT_APP(0);
}

//--------------------------------------------------------------
void testApp::setup()
{
	bSetup              = false;
    
	string video_dir    = ofToDataPath("video");
	dirList.open(video_dir.c_str());
	
	numFiles            = dirList.listDir();
	videoFiles          = dirList.getFiles();
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
	
	currentFile         = 0;
	videoReader         = new ofVideoPlayer();
	videoReader->loadMovie(videoFiles[currentFile].getAbsolutePath());
	totalFrames         = videoReader->getTotalNumFrames();
	currentFrame        = 0;
	allVideoFrames      = 0;
	imageScalar         = 2;
	
	cvColorImg.allocate(videoReader->getWidth(), videoReader->getHeight());
	cvGrayImg.allocate(videoReader->getWidth(), videoReader->getHeight());
	cvGrayImgResized.allocate(videoReader->getWidth()/imageScalar, videoReader->getHeight()/imageScalar);
        
	detector                = FeatureDetector::create("DynamicSURF");
	extractor               = DescriptorExtractor::create("SURF");
	maxKeypointsPerFrame    = 16;
	keypointDimension       = extractor->descriptorSize(); 
	numFeatures             = maxKeypointsPerFrame * keypointDimension;
	numFrames               = totalFrames;	
    totalKeypoints          = 0;
	dataset.create(numFrames, numFeatures, CV_32FC1);
	
	ofSetVerticalSync(false);
	ofSetWindowShape(760, 520);
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

void testApp::finishCurrentMovie()
{
    // close current file
    videoReader->closeMovie();
    videoReader->close();
    delete videoReader;
    sprintf(buf, "%d.mat", currentFile);
    allVideoFrames += (currentFrame);
    writeVocabulary(ofToDataPath(buf), dataset.rowRange(0,currentFrame));
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
		
        // get descriptors (numKeypoints x 64)
		extractor->compute(img, keypoints, descriptors);
        
#ifdef _DEBUG
		assert(keypointDimension == descriptors.cols);
#endif
        
        // do pca data reduction (number of keypoints -> maxKeypointsPerFrame)
		if (descriptors.rows > maxKeypointsPerFrame) {
			PCA pca(descriptors, Mat(), CV_PCA_DATA_AS_COL, maxKeypointsPerFrame);
			
            // each image now is described by maxKeypointsPerFrame x 64 values (in the case of SURF)
			Mat compressed(maxKeypointsPerFrame, keypointDimension, CV_32FC1);
			for( int i = 0; i < keypointDimension; i++ )
			{
				Mat vec = descriptors.col(i), coeffs = compressed.col(i), reconstructed;
                // compress the vector, the result will be stored
                // in the i-th row of the output matrix
				pca.project(vec, coeffs);
			}
			
			dataset.row(currentFrame) = compressed.reshape(numFeatures);
		}
		
		
		/*
         // add to kdtree
         vector<Mat> d;
         d.push_back(descriptors);
         matcher->add(d);
         */
		
		/*
         trainer->add(descriptors);
         */
		
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
			finishCurrentMovie();
            
            // open the next one
			currentFile++;
			videoReader			= new ofVideoPlayer();
			videoReader->loadMovie(videoFiles[currentFile].getAbsolutePath());
			currentFrame		= 0;
			totalFrames			= videoReader->getTotalNumFrames();
			cvColorImg.resize(videoReader->getWidth(), videoReader->getHeight());
			cvGrayImg.resize(videoReader->getWidth(), videoReader->getHeight());
			cvGrayImgResized.resize(videoReader->getWidth()/imageScalar, videoReader->getHeight()/imageScalar);
			
			numFrames			= totalFrames;
			dataset.create(numFrames, numFeatures, CV_32FC1);
		}
		else 
		{
			printf("[OK] Finished parsing %d video files\n", currentFile);
			finishCurrentMovie();
			
            printf("Dataset size: %f Gigabytes\n", numFeatures*numFrames*sizeof(float)/1024.0f/1024.0f/1024.0f);
			Mat completeDataset(allVideoFrames, numFeatures, CV_32FC1);
			int currentRow = 0;
			for (int i = 0; i < numFiles; i++) {
				printf("Appending matrix %d/%d...", i+1, numFiles);
				sprintf(buf, "%d.mat", currentFile);
				Mat data;
				readVocabulary(ofToDataPath(buf), data);
				completeDataset.rowRange(currentRow, currentRow + data.rows) = data;
				currentRow += data.rows;
				printf("[OK].\n");
			}
			
			writeVocabulary(ofToDataPath("complete.mat"), completeDataset);
			
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
	float fps = ofGetFrameRate();
	sprintf(buf, "movie: %d/%d", currentFile+1, numFiles);
	ofDrawBitmapString(buf, ofPoint(20,440));
	
	sprintf(buf, "fps: %02.2f", fps);
	ofDrawBitmapString(buf, ofPoint(20,455));
	
	sprintf(buf, "frame: %d/%d", currentFrame, totalFrames);
	ofDrawBitmapString(buf, ofPoint(20,470));
	
	sprintf(buf, "keypoints %d (this frame), %f (average), %d (total)", 
			keypoints.size(), totalKeypoints / (float)currentFrame, totalKeypoints );
	ofDrawBitmapString(buf, ofPoint(20,486));
    
    sprintf(buf, "estimated dataset size: %f MB",
            numFeatures*(allVideoFrames+currentFrame)*sizeof(float)/1024.0f/1024.0f);
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

