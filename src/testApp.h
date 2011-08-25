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

#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"

#include "ofxDirList.h"
#include "ofxCvColorImage.h"
#include "ofxCvGrayscaleImage.h"

#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/flann/flann.hpp>

using namespace cv; 
using namespace std;
using namespace cv::flann;

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
	
	bool readVocabulary( const string& filename, Mat& vocabulary );
	bool writeVocabulary( const string& filename, const Mat& vocabulary );
	
	// opencv images
	ofxCvColorImage					cvColorImg;			// original img
	ofxCvGrayscaleImage				cvGrayImg,			// convert to grayscale for performance
                                    cvGrayImgResized;
	float							imageScalar;		// resize an image for performance
    
	// keypoints/descriptors
	Mat								descriptors;		// describes each keypoint
	vector<KeyPoint>				keypoints;			// keypoints in an image
	cv::Ptr<FeatureDetector>		detector;			// keypoint class
	cv::Ptr<DescriptorExtractor>	extractor;			// descriptor class
	int								totalKeypoints;		// total keypoints
	int								maxKeypoints,		// number of keypoints per frame
                                    keypointDimension;	// dimension of each keypoint
	
	// video player
	ofVideoPlayer					*videoReader;	
	int								currentFrame,		// current frame in the video
                                    totalFrames;		// total video frames
	int								allVideoFrames;
	
	// directory listing
	ofxDirList						dirList;			// listing of all the files
	int								currentFile, 
    numFiles;
	vector<ofFile>					videoFiles;			
	
	// For kNN - using opencv version of FlANN
	Mat								dataset;			// feature dataset
	cv::flann::Index_<float>		*kdTree;			// kd-tree reference representing all the feature-frames
	cvflann::AutotunedIndexParams	flannParams;		// index parameters are stored here
	int								numFeatures,		// dimension of each point
                                    numFrames;			// number of feature point frames
	
	
	char							buf[256];
	bool							bSetup;
};

#endif
