

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include<iostream>
#include <fstream>
#include<conio.h>           
#include <opencv2/tracking.hpp>
#include <string>
#include<windows.h>
///////////////////////////////////////////////////////////////////////////////////////////////////
using namespace cv;
using namespace std;

int H_MIN = 0;
int H_MAX = 0;
int S_MIN = 0;
int S_MAX = 0;
int V_MIN = 0;
int V_MAX = 0;

//default capture width and height
#define FRAME_WIDTH  640
#define FRAME_HEIGHT  480

//possitionate windows
#define POS_X 600
#define POS_Y 0

//Trackbar max values
#define	TRACKBAR_MAX_VALUE 256

const string file = "threshold_settings.txt";
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const string windowBGR = "Original Image";
const string windowHSV = "HSV Image";
const string windowThreshold = "Thresholded Image";
const string windowName3 = "After Morphological Operations";
const string windowTrackbars = "Trackbars";
const string windowBlur = "Blur Thresholded Image";

char strArr[16][16];


//-------

 
//-------
void on_trackbar(int, void*)
{
 //This function gets called whenever a trackbar position is changed
}
void morphologicalOperations(Mat &threshold) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8)); //dilate with larger element so make sure object is nicely visible

	erode(threshold, threshold, erodeElement);
	erode(threshold, threshold, erodeElement);


	dilate(threshold, threshold, dilateElement);
	dilate(threshold, threshold, dilateElement);



}
void createTrackbars() 
{
	char TrackbarName[50]; //create memory to store trackbar name on window
	namedWindow(windowTrackbars, 0); //create window for trackbars
	
	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH), 
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->      
	createTrackbar("H_MIN", windowTrackbars, &H_MIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("H_MAX", windowTrackbars, &H_MAX, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("S_MIN", windowTrackbars, &S_MIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("S_MAX", windowTrackbars, &S_MAX, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("V_MIN", windowTrackbars, &V_MIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("V_MAX", windowTrackbars, &V_MAX, TRACKBAR_MAX_VALUE, on_trackbar);
	

}
string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}
void splitString(char token[], char filter[])
{
	char *str = strtok(token, filter);
	static int c = 0;

	while (str != NULL)
	{
		strcpy(strArr[c], str); //copy splitted word in array of strings
		str = strtok(NULL, filter); //in order to get next token and to continue with the same string NULL is passed as first argument since strtok maintains a static pointer to your previous passed string
		c++;
	}
}

void writeFile()
{
	ofstream writefile;
	writefile.open(file);
	writefile << "H_MIN: " << H_MIN << endl;
	writefile << "H_MAX: " << H_MAX << endl;
	writefile << "S_MIN: " << S_MIN << endl;
	writefile << "S_MAX: " << S_MAX << endl;
	writefile << "V_MIN: " << V_MIN << endl;
	writefile << "V_MAX: " << V_MAX << endl;
	writefile.close();
	cout << "File Saved!" << endl;
}

void readFile()
{
	ifstream readFile(file);
	string line;
	char filter[3] = ": ";
	int counter = 0;
	char settings[16][16];

	if (readFile.is_open())
	{
		cout << "File openend!" << endl;

		while (getline(readFile, line))
		{
			cout << line << '\n';
			strcpy(settings[counter], line.c_str());
			splitString(settings[counter], filter);
			counter++;
		}
		readFile.close();
		H_MIN = atoi(strArr[1]);
		H_MAX = stoi(strArr[3]);
		S_MIN = stoi(strArr[5]);
		S_MAX = stoi(strArr[7]);
		V_MIN = stoi(strArr[9]);
		V_MAX = stoi(strArr[11]);
	}
	
	else
	{
		cout << "File could not be openned!";
		H_MIN = 0;
		H_MAX = 256;
		S_MIN = 0;
		S_MAX = 256;
		V_MIN = 0;
		V_MAX = 256;
	}
}

void drawObject(int x, int y, Mat &frame) 
{

	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	//UPDATE:JUNE 18TH, 2013
	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);

}
void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) 
	{
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) 
		{
			for (int index = 0; index >= 0; index = hierarchy[index][0]) 
			{

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) 
				{
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else 
					objectFound = false;


			}
			//let user know you found an object
			if (objectFound == true) 
			{
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//draw object location on screen
				drawObject(x, y, cameraFeed);
			}

		}
		else 
			putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

void instructions()
{
	cout << "#======================#" << endl;
	cout << "# Instructions:        #" << endl;
	cout << "# Enable cursor      c #" << endl;
	cout << "# Record stream      r #" << endl;
	cout << "# Save settings      s #" << endl; 
	cout << "# Exit program     ESC #" << endl;
	cout << "#======================#" << endl << endl << endl;

}

void rec(Mat &frame, VideoWriter vidWriter)
{
	//	putText(frame, "REC", Point(0, 60), 2, 2, Scalar(0, 0, 255));
	vidWriter.write(frame);
	putText(frame, "REC", Point(FRAME_WIDTH - 80, 40), 2, 1, Scalar(0, 0, 255), 2);
	cout << "REC!" << endl;
}

void cursor(int x, int y, bool activateCursor)
{
	static bool flag = false;
	if (activateCursor == true)
	{
		SetCursorPos(x * 3, y * 3);

		if (flag == true)
		{
			
			cout << "Cursor enabled!" << endl;
			flag = !flag;
		}
	}
	else
	{
		if (flag == false)
		{
			
			cout << "Cursor disabled!" << endl;
			flag = !flag;
		}
	
	}
}
int main(int argc, char* argv[])
{
	
	char *word, *arr[3], conv[32][32];
	int c = 0;
	
	char *vidLoc = "Test.avi"; 
	int fcc = CV_FOURCC('D', 'I', 'V', '3');
	int fps = 15;
	
	Mat frame; //Matrix to store each frame of the webcam feed
	Mat hsv;
	Mat threshold;
	Mat blur;
	VideoCapture capture; //video capture object to acquire webcam feed
	VideoWriter vidWriter;
	Size frameSize(FRAME_WIDTH, FRAME_HEIGHT);
	vidWriter = VideoWriter(vidLoc, fcc, fps, frameSize);
	instructions();
	readFile();
	int x = 0, y = 0;
	
	bool useMorph = true;
	bool useBlur = true;
	bool trackObj = true;
	bool activateCursor = false;


	bool recording = false;

	capture.open(0); //select default webcam
	createTrackbars(); //create trackbarsq

	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH); //set width of capture frame
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT); //set height of capture frame

	if (!vidWriter.isOpened())
		cout << "error";
	
	while (1)
	{
		bool isRec = false;
		capture.read(frame); //store image to matrix
		flip(frame, frame, 1); //mirror image, 1 = horizontal flip


		cvtColor(frame, hsv, COLOR_BGR2HSV); //convert BGR to HSV
		inRange(hsv, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold); //convert to binary image
		

		if (useBlur)
		{
			medianBlur(threshold, blur, 15);
			if (useMorph)
				morphologicalOperations(blur);

			if (trackObj)
				trackFilteredObject(x, y, blur, frame);
			if (recording == true)
				rec(frame, vidWriter);
			
			moveWindow(windowBGR, POS_X, POS_Y); //position BGR windows
			imshow(windowBGR, frame); //show image in window

			moveWindow(windowBlur, POS_X, POS_Y + FRAME_HEIGHT + 33); //position threshold windows
			imshow(windowBlur, blur); //show image in window
		}

		else
		{

			if (useMorph)
				morphologicalOperations(threshold);
			if (trackObj)
				trackFilteredObject(x, y, threshold, frame);
			if (recording == true)
				rec(frame, vidWriter);
			
			moveWindow(windowBGR, POS_X, POS_Y); //position BGR windows
			imshow(windowBGR, frame); //show image in window

			moveWindow(windowThreshold, POS_X, POS_Y + FRAME_HEIGHT + 33); //position threshold  windows
			imshow(windowThreshold, threshold); //show image in window
	
		}
		if (activateCursor == true)
			cursor(x, y, activateCursor);
		else
			cursor(x, y, activateCursor);
		switch (char(waitKey(1)))
		{
		case 's': writeFile(); break; //if you hit 's' settings will be saved 
		case 'r': recording = !recording; break;
		case 'c': activateCursor = !activateCursor; break;
		case 27: cout << "Program closed!"; return 0;
		}

		waitKey(30); //wait 1ms

	}
	

	return 0;
}
