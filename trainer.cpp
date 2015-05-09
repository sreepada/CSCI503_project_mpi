#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

int someMain(string fn_csv, string test_csv, string att_csv) {
	string mean_folder = "./train_data/mean_faces";
	vector<Mat> images;
	vector<int> labels;
	vector<Mat> att_images;
	vector<int> att_labels;
	vector<Mat> test_images;
	vector<int> test_labels;
	try {
		read_csv(fn_csv, images, labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		exit(1);
	}
	try {
		read_csv(att_csv, att_images, att_labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << att_csv << "\". Reason: " << e.msg << endl;
	}
	try {
		read_csv(test_csv, test_images, test_labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << test_csv << "\". Reason: " << e.msg << endl;
	}

	// Make all images the same size
	for(int i=0; i < images.size(); i++) {
		resize(images[i],images[i],images[0].size(),0,0, INTER_NEAREST);
	}
	for(int i=0; i < att_images.size(); i++) {
		resize(att_images[i],att_images[i],images[0].size(),0,0, INTER_NEAREST);
	}	
	for(int i=0; i < test_images.size(); i++) {
		resize(test_images[i],test_images[i],images[0].size(),0,0, INTER_NEAREST);
	}	

	int height = images[0].rows;

	for (int i = 0; i < 10; i++) {
		vector<Mat> images_i;
		vector<int> labels_i;
		for (int j = i * 36; j < i*36 + 36; j++) {
			images_i.push_back(att_images[j]);
			labels_i.push_back(att_labels[j]);
		}
		Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
		model->train(images_i, labels_i);
		Mat mean = model->getMat("mean");
		imwrite(format("%s/mean%d.png", mean_folder.c_str(), i), norm_0_255(mean.reshape(1, att_images[0].rows)));
	}

	Ptr<FaceRecognizer> model2 = createEigenFaceRecognizer();
	model2->train(images, labels);
	model2->save("eigenfaces_at.yml");
/*	for (vector<Mat>::iterator it = test_images.begin(); it != test_images.end(); it++) {
		int predictedLabel = 100;
		double confidence = 0.0;
		model2->predict(*it, predictedLabel, confidence);
		cout << "Predicted class = " << predictedLabel ;
		cout << " Actual class = " << test_labels[it - test_images.begin()];
		cout <<	" Confidence = " << confidence << endl;
	}*/
	return 0;
}
