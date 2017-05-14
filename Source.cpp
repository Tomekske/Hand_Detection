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

using namespace cv;
using namespace std;

//trackbar
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
#define	TRACKBAR_MAX_VALUE 256

//windows
#define FRAME_WIDTH  640
#define FRAME_HEIGHT  480
#define POS_X 600
#define POS_Y 0
const string file = "threshold_settings.txt";
const string windowBGR = "Original Image";
const string windowHSV = "HSV Image";
const string windowThreshold = "Thresholded Image";
const string windowThreshold2 = "Thresholded2 Image";
const string windowTrackbars = "Trackbars";
const string windowBlur = "Blured Thresholded Image";
const string windowBlur2 = "Blured2 Thresholded Image";

//settings
#define STORE_ARR_SIZE 32
char strArr[STORE_ARR_SIZE][STORE_ARR_SIZE];

//tracking
const int MAX_NUM_OBJECTS = 50;
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;

//trackbar
void on_trackbar(int position, void *userdata);
void createTrackbars();

//settings
void readFile(int object, string filename);
void writeFile(int object, char *filename);

//convert
string intToString(int number);

//instructions
void instructions();

//detection
void morphologicalOperations(Mat &threshold);
bool trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed);
void drawObject(int x, int y, Mat &frame);
void splitString(char token[], char filter[]);

//record
void recordFrame(Mat &frame, VideoWriter vidWriter, bool recording);

//cursor
void cursor(int x, int y, bool activateCursor);
void LeftClick();
int cursorStabilisator(vector<int>&vec, int amountSamples, int data, bool activateStabilisator);
int summation(int value);
int weightedMean(vector<int>&vec);
int distanceCoordinates(int coord_1, int coord_2);

int main(int argc, char* argv[])
{
	bool activateCursor = false;
	bool recording = false;
	bool activateStabilisator = false;
	bool useBlur = true;
	bool qt = false;
	bool enableClick = false;
	int distanceCoord = 0;
	vector<int> vecX;
	vector<int> vecY;
	int g = 0;
	int gem = 0;
	int wMean = 0;

	bool useMorph = true;
	bool trackObj = true;

	bool tr = false;
	bool tr2 = false;

	char *word, *arr[3], conv[32][32];
	int c = 0;

	char *vidLoc = "Recorded.avi";
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

	int test = 0;

	//if application got called from QT we set a bool to true
	if (argc > 1)
		qt = true;

	//we copy commend line arguments to associated variables
	if (qt == true)
	{
		activateCursor = atoi(argv[1]);
		activateStabilisator = atoi(argv[2]);
		recording = atoi(argv[3]);
		useBlur = atoi(argv[4]);
	}

	capture.open(0); //select default webcam

					 //if program isnt called from qt we're going to create trackbars
	if (qt == false)
		createTrackbars(); //create trackbarsq

	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH); //set width of capture frame
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT); //set height of capture frame

														 //If webcam cannot be opened we display an error
	if (!vidWriter.isOpened())
		cout << "error";

	//continuously fetch webcam feed
	while (1)
	{
		capture.read(frame); //store image to matrix
		flip(frame, frame, 1); //mirror image, 1 = horizontal flip

		cvtColor(frame, hsv, COLOR_BGR2HSV); //convert BGR to HSV
		cvtColor(frame, hsv2, COLOR_BGR2HSV); //convert BGR to HSV

		inRange(hsv, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold); //convert HSV to binary 
		inRange(hsv2, Scalar(HMIN, SMIN, VMIN), Scalar(HMAX, SMAX, VMAX), threshold2); //convert HSV to binary image

		//if useMorph == true then we're going to use morphological operations to smoothen the image
		if (useMorph)
		{
			morphologicalOperations(threshold);
			morphologicalOperations(threshold2);
		}

		//if useBlur == true then we're going to blur the image after the morphological operations to smoothen the image
		if (useBlur == true)
		{
			medianBlur(threshold, blur, 15);
			medianBlur(threshold2, blur2, 15);
		}

		//if trackObje == true then we're going to enable mouse tracking
		if (trackObj)
		{
			//if useblur == true we're going to pass blured image coord to track trackFilteredObject
			if (useBlur == true)
			{
				tr = trackFilteredObject(x, y, blur, frame);
				tr2 = trackFilteredObject(x2, y2, blur2, frame);
			}

			//if useblur == false we're going to pass threshold image coord to track trackFilteredObject
			else
			{
				tr = trackFilteredObject(x, y, threshold,frame);
				tr2 = trackFilteredObject(x2, y2, threshold2, frame);
			}
		}

		//if recording == true then we're going to start recording and print status to concolse
		if (recording == true)
			recordFrame(frame, vidWriter, recording);

		//if recording == false then we're going to print status to concolse
		else
			recordFrame(frame, vidWriter, recording);

		//if program isnt called by QT then we're going to display all windowses such as BGR feed, HSV feed, threshold image
		if (qt == false)
		{
			moveWindow(windowBGR, POS_X, POS_Y); //position BGR windows
			imshow(windowBGR, frame); //show image in window

			moveWindow(windowHSV, POS_X * 2 + 60, POS_Y); //position BGR windows
			imshow(windowHSV, hsv); //show image in window

			//if useBlur == true then we're going to display blured images
			if (useBlur == true)
			{
				moveWindow(windowBlur, POS_X, POS_Y + FRAME_HEIGHT + 33); //position threshold windows
				imshow(windowBlur, blur); //show image in window

				moveWindow(windowBlur2, POS_X * 2 + 60, POS_Y + FRAME_HEIGHT + 33); //position threshold  windows
				imshow(windowBlur2, blur2); //show image in window
			}

			//if useBlur == false then we're going to display threshold images
			else
			{
				moveWindow(windowThreshold, POS_X, POS_Y + FRAME_HEIGHT + 33); //position threshold windows
				imshow(windowThreshold, threshold); //show image in window

				moveWindow(windowThreshold2, POS_X * 2 + 60, POS_Y + FRAME_HEIGHT + 33); //position threshold  windows
				imshow(windowThreshold2, threshold2); //show image in window
			}
		}

		nX = cursorStabilisator(vecX, 10, x2, activateStabilisator); //get new x - coord
		nY = cursorStabilisator(vecY, 10, y2, activateStabilisator); //get new y - coord
		distanceCoord = distanceCoordinates(x2, x); //calculate distance between two objects

		//if activateStabilisator == cursor stabilisator will be enabled
		if (activateStabilisator == true)
		{
			//if activateCurso == true then the cursor will be activated and the status text will be displayed once
			if (activateCursor == true)
				cursor(nX, nY, activateCursor);

			//if activateCurso == false then the status text will be displayed once
			else
				cursor(nX, nY, activateCursor);
		}

		//if activateStabilisator == false stabilisator wont be enabled
		else
		{
			//if activateCurso == true then the cursor will be activated and the status text will be displayed once
			if (activateCursor == true)
				cursor(x2, y2, activateCursor);

			//if activateCurso == false then the status text will be displayed once
			else
				cursor(x2, y2, activateCursor);
		}

		//if the distance between the x - coordinates is between a certain distance then you're able to simulate a left click, just once
		if (((distanceCoord <= 70) && (distanceCoord >= -70)) && activateCursor == true && enableClick == false)
		{
			cout << test++ << " clicked" << endl;
			enableClick = true;
			LeftClick();

		}

		//if the distance between the x - coordinates is between a certain distance then we're going to reset the left click to its default state.
		else if (((distanceCoord >= 70) || (distanceCoord <= -70)) && activateCursor == true && enableClick == true)
		{
			enableClick = false;
		}
		
		//this switch case makes it possible to adjust some settings
		switch (char(waitKey(1)))
		{
			case 's': writeFile(1, "threshold1.txt"); writeFile(2, "threshold2.txt"); break; //if you hit 's' settings will be saved 
			case 'r': recording = !recording; break; //if you hit 'r' you'll start recording  
			case 't': activateCursor = !activateCursor; break; //if you hit 'c' you'll activate cursor
			case 'c': activateStabilisator = !activateStabilisator; break; //if you hit 'c' you'll activate cursor
			case  27: cout << "Program closed!"; return 0; //if you hit 'esc' you'll exit the program
		}

		waitKey(10); //wait 1ms
	}
	getchar();
	return 0;
}



//--------------------------------------------------------------------------------------------------------------//
// 											   Trackbar			           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
* @brief  Callback function,If the callback is the NULL pointer, no callbacks are called, but only value is updated
* @param  position: trackbar position
* @param  userdata: User data that is passed as is to the callback. It can be used to handle trackbar events without using global variables.
* @retval None
*/
void on_trackbar(int position, void *userdata)
{
	//This function gets called whenever a trackbar position is changed
}

/**
* @brief  This functions shows the trackbars and update the values
* @param  None
* @retval None
*/
void createTrackbars()
{
	char TrackbarName[50]; //create memory to store trackbar name on window
	namedWindow(windowTrackbars, 0); //create window for trackbars

									 //convert int to string
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
	createTrackbar("H_MIN", windowTrackbars, &H_MIN, TRACKBAR_MAX_VALUE, on_trackbar); //lbl value,
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



//--------------------------------------------------------------------------------------------------------------//
// 											   Settings			           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
* @brief  This functions write trackbar values to a file
* @param  object: which object you want to write to a file
* @param  filename: which file you want to save those settings to
* @retval None
*/
void writeFile(int object, char *filename)
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

/**
* @brief  This functions reads trackbar values from a file and stocks them in a array of strings
* @param  object: which object you want to read from a file
* @param  filename: which file you want to read settings from
* @retval None
*/
void readFile(int object, string filename)
{
	ifstream readFile(filename);
	string line;
	char filter[3] = ": ";
	int counter = 0;
	char settings[STORE_ARR_SIZE][STORE_ARR_SIZE];

	//if file is succesfully opened, save settings in array of strings
	if (readFile.is_open())
	{
		cout << "File openend!" << endl;

		//read settings from file and stock them in an array of strings
		while (getline(readFile, line))
		{
			strcpy(settings[counter], line.c_str());
			splitString(settings[counter], filter);
			counter++;
		}

		//save settings from 1st object in variables
		if (object == 1)
		{
			H_MIN = atoi(strArr[1]);
			H_MAX = stoi(strArr[3]);
			S_MIN = stoi(strArr[5]);
			S_MAX = stoi(strArr[7]);
			V_MIN = stoi(strArr[9]);
			V_MAX = stoi(strArr[11]);
		}

		//save settings from 2nd object in variables
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

	//if file couldnt be opened, the default settings are applied on the trackbars
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

/**
* @brief  This functions splits string into tokens and put them in a string array
* @param  token: string by reference where token is stored
* @param  filter: specify which on characters you want to split
* @retval None
*/
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



//--------------------------------------------------------------------------------------------------------------//
// 											   Convert			           				        				//
//--------------------------------------------------------------------------------------------------------------//


/**
* @brief  Convert integers to a string object
* @param  number: int you want to convert into a string
* @retval integer value as a string object
*/
string intToString(int number)
{
	stringstream ss;
	ss << number;
	return ss.str(); //stringstream to string
}



//--------------------------------------------------------------------------------------------------------------//
// 											   Instructions 	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
* @brief  This function displays the navigation instructions which helps you adjust different settings
* @param  None
* @retval None
*/
void instructions()
{
	cout << "#=================================#" << endl;
	cout << "# Instructions:                   #" << endl;
	cout << "# Enable cursor                 t #" << endl;
	cout << "# Cursor Stabilisator           c #" << endl;
	cout << "# Record stream                 r #" << endl;
	cout << "# Save settings                 s #" << endl;
	cout << "# Exit program                ESC #" << endl;
	cout << "#=================================#" << endl << endl << endl;
}



//--------------------------------------------------------------------------------------------------------------//
// 											   Detection    	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
* @brief  Function to perform morphological operations
* @param  threshold: pass binary feed as parameter
* @retval None
*/
void morphologicalOperations(Mat &threshold)
{
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3)); //erode with a 3px by 3px radius
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8)); //dilate with a 8px by 8px radius

																	   //erode twice for better results
	erode(threshold, threshold, erodeElement);
	erode(threshold, threshold, erodeElement);

	//dilate twice for better results
	dilate(threshold, threshold, dilateElement);
	dilate(threshold, threshold, dilateElement);
}

/**
* @brief  Function to track filtered objects
* @param  x: pass x - coordinate by reference
* @param  y: pass y - coordinate by reference
* @param  threshold: pass binary feed as parameter
* @param  cameraFeed: pass webcamfeed as parameter
* @retval boolean value if object is or isnt found
*/
bool trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed)
{
	Mat temp;
	threshold.copyTo(temp); //copy theshold values to a temporary Matrix object

							//these two vectors needed for output of findContours
	vector< vector<Point> > contours; //2D array of points
	vector<Vec4i> hierarchy; //creates a vector of 4 signed integers

	double refArea = 0;
	bool objectFound = false;

	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); //find counters of filtered objects of camfeed


																					//if hierarchy has more than zero areas stocked in vector
	if (hierarchy.size() > 0)
	{
		int numObjects = hierarchy.size(); //get number of found areas

										   //if number of objects lesser than MAX_NUM_OBJECTS then we dont have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS)
		{

			for (int i = 0; i >= 0; i = hierarchy[i][0])
			{
				Moments moment = moments((cv::Mat)contours[i]); //moments are set of scalars that provide an aggregated measure of a set of vectors.
				double area = moment.m00;

				//we only want the object with the largest area so we safe a reference area each. iteration and compare it to the area in the next iteration.
				if ((area > MIN_OBJECT_AREA) && (area < MAX_OBJECT_AREA) && (area > refArea))
				{
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}

				//if the area is the same as the 3/2 of the image size, probably just a bad filter
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

		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		else
		{
			putText(cameraFeed, "TOO MUCH NOISE!", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
			return false;
		}
		return false;
	}
	return false;
}

/**
* @brief  Function to draw a circle with a cross to mark tracked object in webcamfeed, also print coordinates on screen
* @param  x: pass x - coordinate by value
* @param  y: pass y - coordinate by value
* @param  frame: pass webcamfeed as parameter
* @retval None
*/
void drawObject(int x, int y, Mat &frame)
{
	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2); //draw circle on frame, @20 = radius ciricle

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



//--------------------------------------------------------------------------------------------------------------//
// 											     Record     	           				        				//
//--------------------------------------------------------------------------------------------------------------//



/**
* @brief  Function which allows us to record the RGB frame from our webcam
* @param  frame: pass webcamfeed as parameter
* @param  vidWriter: pass VideoWriter objec as value to write feed to the object
* @param  recording: pass a bool to indicate if the recording should start/stop recording
* @retval None
*/
void recordFrame(Mat &frame, VideoWriter vidWriter, bool recording)
{
	static bool flag = false;

	//start recording
	if (recording == true)
	{
		vidWriter.write(frame);
		putText(frame, "REC", Point(FRAME_WIDTH - 80, 40), 2, 1, Scalar(0, 0, 255), 2); //put text on recording frame

																						//print only once text if recording has started
		if (flag == true)
		{
			cout << "recording!" << endl;
			flag = !flag;
		}
	}

	//stop recording
	else
	{
		//print only once text if recording has stopped
		if (flag == false)
		{
			cout << "not recording!" << endl;
			flag = !flag;
		}
	}
}



//--------------------------------------------------------------------------------------------------------------//
// 											     Cursor     	           				        				//
//--------------------------------------------------------------------------------------------------------------//


/**
* @brief  Functions which allows us to pass coordinates into a mouseevent and simulate to cursor by motiondetection
* @param  x: pass x - coordinate by value
* @param  y: pass y - coordinate by value
* @param  activateCursor: pass a bool to indicate if the cursor is enabled/disabled
* @retval None
*/
void cursor(int x, int y, bool activateCursor)
{
	static bool flag = false;

	//enable cursor
	if (activateCursor == true)
	{
		SetCursorPos(x * 3, y * 3); //windows API function, we multiply coordinates by a constant to make sure we can move cursor acros the whole screen

									//print only once text if cursor got enabled
		if (flag == true)
		{
			cout << "Cursor enabled!" << endl;
			flag = !flag;
		}
	}

	//disable cursor
	else
	{
		//print only once text if cursor got disabled
		if (flag == false)
		{
			cout << "Cursor disabled!" << endl;
			flag = !flag;
		}
	}
}

/**
* @brief  This functions allows use to simulate a left click
* @param  None
* @retval None
*/
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

/**
* @brief  This function calculates the distance between coordinates
* @param  coord_1: x - coord of first object
* @param  coord_2: x - coord of second object
* @retval Calculated distance betweenc coordinates
*/
int distanceCoordinates(int coord_1, int coord_2)
{
	return coord_2 - coord_1;
}

/**
* @brief  This functions allows us to stabilize the cursors motion
* @param  vec: pass vector which holds the coordinates by reference
* @param  amountSamples: how many coordinates you want in your calcultion
* @param  data: are the new coordinates passed by value
* @param  activateStabilisator: indicates if cursor stabilisation must be turned on/off
* @retval new calculated coordinates
*/
int cursorStabilisator(vector<int>&vec, int amountSamples, int data, bool activateStabilisator)
{
	int vectorSize = vec.size(); //calculates how many elements there are in the vector
	static int cFlag = false;
	static int flag = false;

	//enables cursor stabilisation
	if (activateStabilisator == true)
	{
		//print text once is cursor stabilisator has turned on
		if (flag == true)
		{
			cout << "Stabilisator enabled!" << endl;
			flag = !flag;
		}
	}

	//disables cursor stabilisation
	else
	{
		//print text once is cursor stabilisator has turned off
		if (flag == false)
		{
			cout << "Stabilisator disabled!" << endl;
			flag = !flag;
		}
	}

	//returns new weighted coordinate value
	if (vectorSize > amountSamples)
	{
		vec.erase(vec.begin()); //erase oldest element in vector
		cFlag = true;
		return weightedMean(vec); //calculates and returns new coord value
	}

	//if vector hasnt all samples then we keep adding samples to vector
	else
	{
		vec.push_back(data);
		if (cFlag == false)
			return data;
	}
}

/**
* @brief  This functions calculates the sum of all coordinate values
* @param  value: amount of elements in vector
* @retval summation of all coordinates
*/
int summation(int value)
{
	int sum = 0;

	for (int i = 1; i <= value; i++)
		sum += i;
	return sum;
}

/**
* @brief  This functions calculates the weighted mean of the elements in a vector
* @param  vec: pass vector which holds the coordinates by reference
* @retval new calculated coordinates
*/
int weightedMean(vector<int>&vec)
{
	int wMean = 0;
	int sum = summation(vec.size());
	for (int i = 0; i < vec.size(); i++)
		wMean += (i + 1)*vec[i];
	return wMean /= sum;
}


