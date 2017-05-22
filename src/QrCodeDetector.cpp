#include "QrCodeDetector.h"
#include <iterator>

#pragma comment (lib, "libiconv.lib")
using namespace cv;
using namespace std;
using namespace zbar;

QrCodeDetector::QrCodeDetector()
{
	cout << "default" << endl;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
}

QrCodeDetector::QrCodeDetector(Mat & cameraMatrix_, double & landmarkSize_) : cameraMatrix(cameraMatrix_), landmarkSize(landmarkSize_)
{
	pts_src.push_back(Point2f(0,0));
	pts_src.push_back(Point2f(640-1, 0));
	pts_src.push_back(Point2f(640-1, 480));
	pts_src.push_back(Point2f(0, 480));
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
}

int QrCodeDetector::detect(Mat & frame) {


	Mat grey;
	cvtColor(frame, grey, CV_BGR2GRAY);
	int width = frame.cols;
	int height = frame.rows;
	cout << width << " " << height << std::endl;
	uchar *raw = (uchar *)grey.data;
	// wrap image data   
	Image image(width, height, "Y800", raw, width * height);
	// scan the image for barcodes   
	int n = scanner.scan(image);
	
	// extract results
	cout << "qr codes detected: " << n << endl;

	Point2f pts[4];
	vector<Point> pts_dst;

	for (Image::SymbolIterator symbol = image.symbol_begin();
		symbol != image.symbol_end();
		++symbol) {
		// do something useful with results  
		vector<Point> pts_temp;

		cout << "decoded " << symbol->get_type_name() << " symbol " << symbol->get_data() << '"' <<" "<< endl;   
			int n = symbol->get_location_size();
		for (int i = 0; i<n; i++) {
			pts_temp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
		}
		RotatedRect r = minAreaRect(pts_temp);
		
		r.points(pts);
		for (int i = 0; i<4; i++) {
			cout << "point " << i << " " << pts[i] << endl;
			pts_dst.push_back(pts[i]);
			line(frame, pts[i], pts[(i + 1) % 4], Scalar(255, 0, 0), 3);
			circle(frame, pts[i], 2, Scalar(0, 255, 0), 3);
		}
		//cout<<"Angle: "<<r.angle<<endl;   
	}
	imshow("MyVideo", frame); //show the frame in "MyVideo" window  
	if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop  
	{
		cout << "esc key is pressed by user" << endl;
		return -1;
	}//*/

	// Calculate Homography
	Mat h = findHomography(pts_src, pts_dst);
	h.convertTo(h, CV_32F);
	// Output image
	//Mat im_out;
	// Warp source image to destination based on homography
	//warpPerspective(im_src, im_out, h, im_dst.size());
	Mat pose;
	cameraPoseFromHomography(h, pose);
	cout << pose << endl;

	double l = 5;
	std::vector<cv::Point2f> pose_points2d;
	pose_points2d.push_back((cv::Point2f(320,240)));    // axis center
	pose_points2d.push_back((cv::Point2f(340.0,230.0)));    // axis x
	pose_points2d.push_back((cv::Point2f(0,l)));    // axis y
	pose_points2d.push_back((cv::Point2f(0.5,0.5)));    // axis z
	draw3DCoordinateAxes(frame, pose_points2d);

	imshow("MyVideo", frame); //show the frame in "MyVideo" window  
	if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop  
	{
		cout << "esc key is pressed by user" << endl;
		return 1;
	}
}
void QrCodeDetector::cameraPoseFromHomography(const Mat& H, Mat& pose)
{
	pose = Mat::eye(3, 4, CV_32FC1);      // 3x4 matrix, the camera pose
	float norm1 = (float)norm(H.col(0));
	float norm2 = (float)norm(H.col(1));
	float tnorm = (norm1 + norm2) / 2.0f; // Normalization value

	Mat p1 = H.col(0);       // Pointer to first column of H
	Mat p2 = pose.col(0);    // Pointer to first column of pose (empty)

	cv::normalize(p1, p2);   // Normalize the rotation, and copies the column to pose

	p1 = H.col(1);           // Pointer to second column of H
	p2 = pose.col(1);        // Pointer to second column of pose (empty)

	cv::normalize(p1, p2);   // Normalize the rotation and copies the column to pose

	p1 = pose.col(0);
	p2 = pose.col(1);

	Mat p3 = p1.cross(p2);   // Computes the cross-product of p1 and p2
	Mat c2 = pose.col(2);    // Pointer to third column of pose
	p3.copyTo(c2);       // Third column is the crossproduct of columns one and two

	pose.col(3) = H.col(2) / tnorm;  //vector t [R|t] is the last column of pose
}
// Draw the 3D coordinate axes
void QrCodeDetector::draw3DCoordinateAxes(cv::Mat image, const std::vector<cv::Point2f> &list_points2d)
{
	cv::Scalar red(0, 0, 255);
	cv::Scalar green(0, 255, 0);
	cv::Scalar blue(255, 0, 0);
	cv::Scalar black(0, 0, 0);

	cv::Point2i origin = list_points2d[0];
	cv::Point2i pointX = list_points2d[1];
	cv::Point2i pointY = list_points2d[2];
	cv::Point2i pointZ = list_points2d[3];

	drawArrow(image, origin, pointX, red, 9, 2, lineType, 1);
	//drawArrow(image, origin, pointY, blue, 9, 2, lineType, 1);
	//drawArrow(image, origin, pointZ, green, 9, 2, lineType, 1);
	//cv::circle(image, origin, radius / 2, black, -1, lineType);

}
// Draw an arrow into the image
void QrCodeDetector::drawArrow(cv::Mat image, cv::Point2i p, cv::Point2i q, cv::Scalar color, int arrowMagnitude, int thickness, int line_type, int shift)
{
	//Draw the principle line
	cv::line(image, p, q, color, thickness, line_type, shift);
	const double PI = CV_PI;
	//compute the angle alpha
	double angle = atan2((double)p.y - q.y, (double)p.x - q.x);
	cout << angle << endl;
	//compute the coordinates of the first segment
	p.x = (int)(q.x + arrowMagnitude * cos(angle + PI / 4));
	p.y = (int)(q.y + arrowMagnitude * sin(angle + PI / 4));
	//Draw the first segment
	cout << p << endl;
	cv::line(image, p, q, color, thickness, line_type, shift);
	//compute the coordinates of the second segment
	p.x = (int)(q.x + arrowMagnitude * cos(angle - PI / 4));
	p.y = (int)(q.y + arrowMagnitude * sin(angle - PI / 4));
	//Draw the second segment
	cv::line(image, p, q, color, thickness, line_type, shift);
}