#pragma once

//standard includes
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

//opencv includes
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//dlib includes
#include <dlib/svm_threaded.h>
#include <dlib/string.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing.h>
#include <dlib/array2d.h>
#include <dlib/opencv.h>
#include <dlib/opencv/cv_image.h>
#include <dlib/data_io.h>
#include <dlib/cmd_line_parser.h>

//namespaces if desired
using namespace dlib;
using namespace std;

class stopsign_detector {

public:
	stopsign_detector(bool vis, atomic<bool> * stopsign_detected);
	bool detect();
	typedef scan_fhog_pyramid<pyramid_down<6> > image_scanner_type;
	object_detector<image_scanner_type> detector;
	void run();
	bool detected;
	atomic<bool> * stopsign_detected_ref;

private:
	cv::VideoCapture cap;
	bool visualize;
};