
#include "QrCodeDetector.h"



using namespace cv;
using namespace std;
using namespace zbar;

int main(int argc, char* argv[])
{
	string cameraid = "out.avi";
	//int cameraid = (int)atoi(argv[1]);
	double landmarkSize = (double) atof( argv[2]);
	cout << landmarkSize << endl;
	float data[3][3] = { {1,0,1},{0,1,1},{0,0,1} };
	Mat cameraMatrix(3,3,CV_32FC1, & data);
	QrCodeDetector firstqr(cameraMatrix, landmarkSize);
	
	
	VideoCapture cap(cameraid); // viodeocapture object
	cout << "camera" << endl;
	while (1) {
		if (!cap.isOpened())  // if not success, exit program
		{
			cout << "Cannot open the video cam" << endl;
			continue;
		}
		Mat frame;
		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			return -1;
		}

		int i=firstqr.detect(frame);
		if (i < 0) {
			return -1;

		}

	}

	
}
/*
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

int main() {

	VideoCapture vcap(2);
	if (!vcap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	int frame_width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
	VideoWriter video("out.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10, Size(frame_width, frame_height), true);

	for (;;) {

		Mat frame;
		vcap >> frame;
		video.write(frame);
		imshow("Frame", frame);
		char c = (char)waitKey(33);
		if (c == 27) break;
	}
	return 0;
}*/