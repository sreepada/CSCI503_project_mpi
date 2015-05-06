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
 
vector<Mat> cropFaces(string path, int label)
{
    vector<Mat> cropped_faces;
    vector<Rect> faces;
		Mat frame = imread(path);
    Mat frame_gray;
    Mat crop;
    Mat res;
    Mat gray;

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
		return cropped_faces;
}

int recognizeSuspect(string model_path, vector<Mat> test_images){
    vector<Mat> test_images;

    for(int i=0; i < test_images.size(); i++) {
        resize(test_images[i],test_images[i],test_images[0].size(),0,0, INTER_NEAREST);
        printf("Image %d size %d\n", i, test_images[i].size().height * test_images[i].size().width);
    }   

    Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
    model->load(model_path);
		int flag = 0;
		for (vector<Mat>::iterator it = test_images.begin(); it != test_images.end(); it++) {
			int predictedLabel = -1;
			double confidence = 0.0;
			model->predict(*it, predictedLabel, confidence);
			cout << "Predicted class = " << predictedLabel ;
			if (predictedLabel == 1) {
				flag = 1;
			}
			cout << " Actual class = " << test_labels[it - test_images.begin()];
			cout <<	" Confidence = " << confidence << endl;
		}
		return flag;
}
