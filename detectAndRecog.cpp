#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;

// Global variables
// Copy this file from opencv/data/haarscascades to target folder
string face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";

static Mat norm_0_255(InputArray _src) {
	Mat src = _src.getMat();
	// Create and return normalized image:
	Mat dst;
	switch(src.channels()) {
		case 1:
			cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
			break;
		case 3:
			cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
			break;
		default:
			src.copyTo(dst);
			break;
	}
	return dst;
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if(!path.empty() && !classlabel.empty()) {
			//printf("%s\n", path.c_str());
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

vector<Mat> cropFaces(string path)
{
	// Load the cascade
	if (!face_cascade.load(face_cascade_name))
	{
		printf("--(!)Error loading\n");
		vector<Mat> temp;
		return temp;
	}

	vector<Mat> cropped_faces;
	vector<Rect> faces;
	Mat frame = imread(path);
	Mat gray;
	if (path.find("att_faces") != string::npos) {
		resize(frame, frame, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
		cvtColor(frame, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale
		cropped_faces.push_back(gray);
	}
	else {
		Mat frame_gray;
		Mat crop;
		Mat res;

		cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);

		// Detect faces
		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

		// Set Region of Interest
		cv::Rect roi_c;

		size_t ic = 0; // ic is index of current element

		for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
		{
			roi_c.x = faces[ic].x;
			roi_c.y = faces[ic].y;
			roi_c.width = (faces[ic].width);
			roi_c.height = (faces[ic].height);

			crop = frame(roi_c);
			//        cout << "roi_c.x" << roi_c.x << endl;
			//        cout << "roi_b.x" << roi_b.x << endl;
			resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
			cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

			cropped_faces.push_back(gray);
		}
	}
	return cropped_faces;
}

int recognizeSuspect(vector<Mat> test_images, vector<int> test_labels, string model_path){
	for(int i=0; i < test_images.size(); i++) {
		resize(test_images[i],test_images[i],Size(128,128),0,0, INTER_NEAREST);
	}   

	Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
	/*vector<Mat> images;
		vector<int> labels;
		read_csv("train.csv",images,labels);
		for(int i=0; i < images.size(); i++) {
		resize(images[i],images[i],Size(128,128),0,0, INTER_NEAREST);
		}   
		model->train(images, labels);*/
	model->load(model_path);
	int flag = 0;
	for (vector<Mat>::iterator it = test_images.begin(); it != test_images.end(); it++) {
		int predictedLabel = -1;
		double confidence = 0.0;
		model->predict(*it, predictedLabel, confidence);
		cout << "Actual label = " << test_labels[it - test_images.begin()];
		cout << " Predicted class = " << predictedLabel ;
		if (predictedLabel == 1) {
			flag = 1;
		}
		cout <<	" Confidence = " << confidence << endl;
	}
	return flag;
}
