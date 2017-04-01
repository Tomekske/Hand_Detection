

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <conio.h>           
#include <opencv2/tracking.hpp>
#include <string>
#include <windows.h>
#include <vector>

#include <time.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
using namespace cv;
using namespace std;

int H_MIN = 0;
int H_MAX = 0;
int S_MIN = 0;
int S_MAX = 0;
int V_MIN = 0;
int V_MAX = 0;


int HMIN = 0;
int HMAX = 0;
int SMIN = 0;
int SMAX = 0;
int VMIN = 0;
int VMAX = 0;

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
const string windowThreshold2 = "Thresholded2 Image";
const string windowTrackbars = "Trackbars";
const string windowBlur = "Blured Thresholded Image";
const string windowBlur2 = "Blured2 Thresholded Image";

#define STORE_ARR_SIZE 32
char strArr[STORE_ARR_SIZE][STORE_ARR_SIZE];


//-------

void LeftClick()
{
	INPUT    Input = { 0 };
	// left down 
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	::SendInput(1, &Input, sizeof(INPUT)); //It means global scope. You might need to use this operator when you have conflicting functions or variables in the same scope and you need to use a global one

	// left up
	::ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	::SendInput(1, &Input, sizeof(INPUT));
}
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

	sprintf(TrackbarName, "HMIN", H_MIN);
	sprintf(TrackbarName, "HMAX", H_MAX);
	sprintf(TrackbarName, "SMIN", S_MIN);
	sprintf(TrackbarName, "SMAX", S_MAX);
	sprintf(TrackbarName, "VMIN", V_MIN);
	sprintf(TrackbarName, "VMAX", V_MAX);

	//create trackbars and insert them into window
	createTrackbar("H_MIN", windowTrackbars, &H_MIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("H_MAX", windowTrackbars, &H_MAX, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("S_MIN", windowTrackbars, &S_MIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("S_MAX", windowTrackbars, &S_MAX, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("V_MIN", windowTrackbars, &V_MIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("V_MAX", windowTrackbars, &V_MAX, TRACKBAR_MAX_VALUE, on_trackbar);

	createTrackbar("HMIN", windowTrackbars, &HMIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("HMAX", windowTrackbars, &HMAX, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("SMIN", windowTrackbars, &SMIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("SMAX", windowTrackbars, &SMAX, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("VMIN", windowTrackbars, &VMIN, TRACKBAR_MAX_VALUE, on_trackbar);
	createTrackbar("VMAX", windowTrackbars, &VMAX, TRACKBAR_MAX_VALUE, on_trackbar);

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
		//cout << c << endl;
	}
}

void writeFile(int object,char *filename)
{
	ofstream writefile;
	writefile.open(filename);
	if (object == 1)
	{
		writefile << "H_MIN: " << H_MIN << endl;
		writefile << "H_MAX: " << H_MAX << endl;
		writefile << "S_MIN: " << S_MIN << endl;
		writefile << "S_MAX: " << S_MAX << endl;
		writefile << "V_MIN: " << V_MIN << endl;
		writefile << "V_MAX: " << V_MAX << endl;
	}

	else if (object == 2)
	{
		writefile << "H_MIN: " << HMIN << endl;
		writefile << "H_MAX: " << HMAX << endl;
		writefile << "S_MIN: " << SMIN << endl;
		writefile << "S_MAX: " << SMAX << endl;
		writefile << "V_MIN: " << VMIN << endl;
		writefile << "V_MAX: " << VMAX << endl;
	}

	writefile.close();
	cout << "File Saved!" << endl;
}

void readFile(int object,string filename)
{
	ifstream readFile(filename);
	string line;
	char filter[3] = ": ";
	int counter = 0;
	char settings[STORE_ARR_SIZE][STORE_ARR_SIZE];

	if (readFile.is_open())
	{
		cout << "File openend!" << endl;

		while (getline(readFile, line))
		{
			strcpy(settings[counter], line.c_str());
			splitString(settings[counter], filter);
			counter++;
		}
		if (object == 1)
		{

		
		H_MIN = atoi(strArr[1]);
		H_MAX = stoi(strArr[3]);
		S_MIN = stoi(strArr[5]);
		S_MAX = stoi(strArr[7]);
		V_MIN = stoi(strArr[9]);
		V_MAX = stoi(strArr[11]);

		}
		else if (object == 2)
		{
			HMIN = atoi(strArr[13]);
			HMAX = stoi(strArr[15]);
			SMIN = stoi(strArr[17]);
			SMAX = stoi(strArr[19]);
			VMIN = stoi(strArr[21]);
			VMAX = stoi(strArr[23]);
		}	
	}
	
	else
	{
		cout << "File could not be openned!" << endl;
		H_MIN = 0;
		H_MAX = 256;
		S_MIN = 0;
		S_MAX = 256;
		V_MIN = 0;
		V_MAX = 256;

		HMIN = 0;
		HMAX = 256;
		SMIN = 0;
		SMAX = 256;
		VMIN = 0;
		VMAX = 256;
	}

	readFile.close();
}

void drawObject(int x, int y, Mat &frame) 
{
	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2); //draw circle on frame, @20 = radius ciricle
	
	//making sure center cross is drawn correct in every possition
	
	//show upper line 
	if (y - 20>0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);

	//show under line	
	if (y + 25<FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	
	//show left line
	if (x - 25>0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	
	//show right line
	if (x + 25<FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else 
		line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);
	


	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(255, 0, 0), 2); //show coordinates!

}
bool trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
{
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
				{
					objectFound = false;
				}
			}
			//let user know you found an object
			if (objectFound == true) 
			{
				drawObject(x, y, cameraFeed);	//draw object location on screen
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			putText(cameraFeed, "TOO MUCH NOISE!", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
			return false;
		}
		return false;
	}
	return false;
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

void recordFrame(Mat &frame, VideoWriter vidWriter,bool recording)
{
	static bool flag = false;
	if (recording == true)
	{
		vidWriter.write(frame);
		putText(frame, "REC", Point(FRAME_WIDTH - 80, 40), 2, 1, Scalar(0, 0, 255), 2);

		if (flag == true)
		{

			cout << "recording!" << endl;
			flag = !flag;
		}
	}

	else
	{
		if (flag == false)
		{

			cout << "not recording!" << endl;
			flag = !flag;
		}
	}
}



int summation(int value)
{
	int sum = 0;

	for (int i = 1; i <= value; i++)
		sum += i;
	return sum;
}

int weightedMean(vector<int>&vec)
{
	int wMean = 0;
	int sum = summation(vec.size());
	for (int i = 0; i < vec.size(); i++)
		wMean += (i + 1)*vec[i];

	return wMean /= sum;
}

int cursorStabilisator(vector<int>&vec, int amountSamples, int data)
{
	int vectorSize = vec.size();
	static int flag = false;

	if (vectorSize > amountSamples)
	{
		vec.erase(vec.begin());
		flag = true;
		return weightedMean(vec);
		
	}
	else
	{
		vec.push_back(data);
		if (flag == false)
			return data;
	}
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
	vector<int> vecX; 
	vector<int> vecY;



	int g = 0;
	int gem = 0;
	int wMean = 0;



	bool useBlur = true;
	bool useMorph = true;
	bool trackObj = true;
	bool activateCursor = false;

	bool tr = false;
	bool tr2 = false;
	bool recording = false;

	char *word, *arr[3], conv[32][32];
	int c = 0;
	
	char *vidLoc = "Test.avi"; 
	int fcc = CV_FOURCC('D', 'I', 'V', '3');
	int fps = 10;

	Mat frame; //Matrix to store each frame of the webcam feed
	Mat hsv; //Matrix to store each hsv frame of the first object of the webcam feed
	Mat hsv2; //Matrix to store each hsv frame of the second object of the webcam feed
	Mat threshold; //Matrix to store each binairy frame of the first object of the webcam feed
	Mat threshold2; //Matrix to store each binairy frame of the second object of the webcam feed
	Mat blur;
	Mat blur2;
	VideoCapture capture; //video capture object to acquire webcam feed
	VideoWriter vidWriter;
	Size frameSize(FRAME_WIDTH, FRAME_HEIGHT);
	vidWriter = VideoWriter(vidLoc, fcc, fps, frameSize);
	instructions();
	readFile(1, "threshold1.txt"); 
	readFile(2, "threshold2.txt");
	int x = 0, y = 0;
	int x2 = 0, y2 = 0;
	int nX = 0, nY = 0;



	capture.open(0); //select default webcam
	createTrackbars(); //create trackbarsq

	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH); //set width of capture frame
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT); //set height of capture frame

	if (!vidWriter.isOpened())
		cout << "error";
	
	while (1)
	{
		capture.read(frame); //store image to matrix
		flip(frame, frame, 1); //mirror image, 1 = horizontal flip

		cvtColor(frame, hsv, COLOR_BGR2HSV); //convert BGR to HSV
		cvtColor(frame, hsv2, COLOR_BGR2HSV); //convert BGR to HSV

		inRange(hsv, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold); //convert to binary 
		inRange(hsv2, Scalar(HMIN, SMIN, VMIN), Scalar(HMAX, SMAX, VMAX), threshold2); //convert to binary image
		
		if (useBlur)
		{
			if (useMorph)
			{
				morphologicalOperations(threshold);
				morphologicalOperations(threshold2);
			}

			medianBlur(threshold, blur, 15);
			medianBlur(threshold2, blur2, 15);

			if (trackObj)
			{
				tr = trackFilteredObject(x, y, blur, frame);
				tr2 = trackFilteredObject(x2, y2, blur2, frame);
			}

			if (recording == true)
				recordFrame(frame, vidWriter, recording);
			else
				recordFrame(frame, vidWriter, recording);
			
			moveWindow(windowBGR, POS_X, POS_Y); //position BGR windows
			imshow(windowBGR, frame); //show image in window

			moveWindow(windowBlur, POS_X, POS_Y + FRAME_HEIGHT + 33); //position threshold windows
			imshow(windowBlur, blur); //show image in window
			
			moveWindow(windowBlur2, POS_X * 2 + 60, POS_Y + FRAME_HEIGHT + 33); //position threshold  windows
			imshow(windowBlur2, blur2); //show image in window
			
			nX = cursorStabilisator(vecX, 20, x);
			nY = cursorStabilisator(vecY, 20, y);

			if (activateCursor == true)
				cursor(nX, nY, activateCursor);
			else
				cursor(nX, nY, activateCursor);

			if (activateCursor == true)
				cursor(x, y, activateCursor);
			else
				cursor(x, y, activateCursor);

			if (tr == true && tr2 == true && activateCursor == true)
				LeftClick();
		}

		else
		{
			
			if (useMorph)
			{
				morphologicalOperations(threshold);
				morphologicalOperations(threshold2);
			}
			if (trackObj)
			{
				tr = trackFilteredObject(x, y, threshold, frame);
				tr2 = trackFilteredObject(x2, y2, threshold2, frame);
			}

			if (recording == true)
				recordFrame(frame, vidWriter, recording);
			else
				recordFrame(frame, vidWriter, recording);

			moveWindow(windowBGR, POS_X, POS_Y); //position BGR windows
			imshow(windowBGR, frame); //show image in window

			imshow(windowHSV, hsv); //show image in window

			moveWindow(windowThreshold, POS_X, POS_Y + FRAME_HEIGHT + 33); //position threshold windows
			imshow(windowThreshold, threshold); //show image in window

			moveWindow(windowThreshold2, POS_X * 2 + 60, POS_Y + FRAME_HEIGHT + 33); //position threshold  windows
			imshow(windowThreshold2, threshold2); //show image in window
			nX = cursorStabilisator(vecX, 20, x);
			nY = cursorStabilisator(vecY, 20, y);

			//cout << x << "   " << nX << "   " << y << "   " << nY << endl;
			if (activateCursor == true)
				cursor(nX, nY,activateCursor);
			else
				cursor(nX, nY, activateCursor);

			if (tr == true && tr2 == true && activateCursor == true)
				LeftClick();
		}

		switch (char(waitKey(1)))
		{
			case 's': writeFile(1,"threshold1.txt"); writeFile(2, "threshold2.txt"); break; //if you hit 's' settings will be saved 
			case 'r': recording = !recording; break; //if you hit 'r' you'll start recording  
			case 'c': activateCursor = !activateCursor; break; //if you hit 'c' you'll activate cursor
			case  27: cout << "Program closed!"; return 0; //if you hit 'esc' you'll exit the program
		}

		waitKey(10); //wait 1ms

	}
	

	
	getchar();
	return 0;
}
