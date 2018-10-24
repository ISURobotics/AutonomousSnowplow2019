#include "stopsign_detector.h"

using namespace std;

stopsign_detector::stopsign_detector(bool vis, atomic<bool> * stopsign_detected) {
	cout << "Initializing stopsign detector" << endl;
	stopsign_detected_ref = stopsign_detected;
	*stopsign_detected_ref = false;
	visualize = vis;
	if (!cap.open(0))
	{
		cout << "Failed to initialize camera" << endl;
	}
	else
	{
		cap.set(3, 640);
		cap.set(4, 480);
	}
	ifstream fin("C:/ISU Robotics/AutonomousSnowplow2019/x64/Release/object_detector.svm", ios::binary);
	if (!fin)
	{
		cout << "Couldn't find object_detector.svm, please ensure it's located in /x64/Release/." << endl;
	}
	deserialize(detector, fin); //Load trained detector into our detector object.
	detected = false;
}

bool stopsign_detector::detect() {
	cv::Mat frame;
	bool ret = false;
	/*-------------------
	Capture Image, convert to dlib format
	--------------------*/
	cap >> frame;
	array2d<bgr_pixel> dlibImage;
	assign_image(dlibImage, dlib::cv_image<bgr_pixel>(frame));

	/*-------------------
	Detect rectangles (supports multiple stop signs)
	--------------------*/
	const std::vector<rectangle> rects = detector(dlibImage);
	if (rects.size() > 0) {
		ret = true;
	}

	/*-------------------
	Visualize detected stop signs
	--------------------*/
	if (visualize)
	{
		if (ret) {
			cv::Rect rectToDraw(rects.at(0).left(), rects.at(0).top(), rects.at(0).width(), rects.at(0).height());
			cv::rectangle(frame, rectToDraw, cv::Scalar(0, 255, 0));
		}
		cv::imshow("Stop Sign Detector", frame);
	}
	cv::waitKey(1);
	return ret;
}

void stopsign_detector::run()
{
	while (true)
	{
		//cout << "Stop sign detector running..." << endl;
		*stopsign_detected_ref = detect();
	}
}