#pragma once
#pragma comment (lib, "libiconv.lib")
#include <zbar.h>
#include "opencv2/opencv.hpp" 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iconv.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"




using namespace cv;
using namespace std;
using namespace zbar;

class QrCodeDetector
{
	// For text
	const int fontFace = cv::FONT_ITALIC;
	const double fontScale = 0.75;
	const int thickness_font = 2;

	// For circles
	const int lineType = 8;
	const int radius = 4;
	const double thickness_circ = -1;

	void cameraPoseFromHomography(const Mat& H, Mat& pose);

	// auxiliar reference frame rendering methods
	void draw3DCoordinateAxes(cv::Mat image, const std::vector<cv::Point2f> &list_points2d);
	void drawArrow(cv::Mat image, cv::Point2i p, cv::Point2i q, cv::Scalar color, int arrowMagnitude, int thickness, int line_type, int shift);
public:
	QrCodeDetector(); //class default constructor 
	QrCodeDetector(Mat & cameraMatrix_, double & landmarkSize_); //specific construtor
	int detect(Mat & frame); //method of the class
	
private:
	ImageScanner scanner;
	Mat cameraMatrix;
	double landmarkSize;
	vector<Point2f> pts_src;

};
