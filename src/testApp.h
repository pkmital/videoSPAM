#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxDirList.h"
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
#include "ofxCvColorImage.h"
#include "ofxCvGrayscaleImage.h"

//#include "gist-classifier.hpp"

#ifndef GIST_SIZE 
#define GIST_SIZE 32
#endif

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
	
	bool readVocabulary( const string& filename, Mat& vocabulary );
	bool writeVocabulary( const string& filename, const Mat& vocabulary );
	
	ofxCvColorImage					cvColorImg;
	ofxCvGrayscaleImage				cvGrayImg, cvGrayImgResized;
	float							imageScalar;

	Mat								descriptors;
	vector<KeyPoint>				keypoints;
	cv::Ptr<FeatureDetector>		detector;
	cv::Ptr<DescriptorExtractor>	extractor;
	cv::Ptr<DescriptorMatcher>		matcher;
	BOWKMeansTrainer				*trainer;
	
	int								totalKeypoints;
	
	ofVideoPlayer					*videoReader;
	int								currentFrame, totalFrames;
	
	ofxDirList						dirList;
	int								currentFile, numFiles;
	vector<ofFile>					videoFiles;
	
	bool							bSetup;
};

#endif
