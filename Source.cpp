// CannyStill.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<iostream>
#include<conio.h>           // may have to modify this line if not using Windows
#include <opencv2/tracking.hpp>


#define COLOR Scalar(100,255,125)
#define BEGIN Point(0, 0)
#define END Point(200, 100)
#define THICKNESS 5
#define COLORIZED 1

#define B 20
#define G 15
#define R 80

#define A 60
#define LOWB Scalar(B, G, R)
#define HIGHB Scalar(B+A, G+A, R+A)
///////////////////////////////////////////////////////////////////////////////////////////////////
using namespace cv;
using namespace std;


int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;

//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowName = "Original Image";
const string windowName1 = "HSV Image";
const string windowName2 = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string trackbarWindowName = "Trackbars";
void on_trackbar(int, void*)
{//This function gets called whenever a
 // trackbar position is changed

}
string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}
int main(int argc, char* argv[])
{
	Mat frame; //Matrix to store each frame of the webcam feed
	VideoCapture capture; //video capture object to acquire webcam feed

	capture.open(0); //select default webcam

	
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH); //set width of capture frame
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT); //set height of capture frame

	while (1)
	{
		capture.read(frame); //store image to matrix
		flip(frame, frame, 1); //mirror image, 1 = horizontal flip
		imshow(windowName, frame); //show image in window
		waitKey(30); //wait 30ms
	}

	return 0;
}
