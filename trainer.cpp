/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;



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

int main(int argc, const char *argv[]) {
	// Get the path to your CSV.
	string fn_csv = string(argv[1]);
	string test_csv = string(argv[2]);
	string att_csv = string(argv[3]);
	// These vectors hold the images and corresponding labels.
	string mean_folder = "./train_data/mean_faces";
	vector<Mat> images;
	vector<int> labels;
	vector<Mat> att_images;
	vector<int> att_labels;
	vector<Mat> test_images;
	vector<int> test_labels;
	// Read in the data. This can fail if no valid
	// input filename is given.
	try {
		read_csv(fn_csv, images, labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
		// nothing more we can do
		exit(1);
	}
	try {
		read_csv(att_csv, att_images, att_labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << att_csv << "\". Reason: " << e.msg << endl;
		// nothing more we can do
		//         exit(1);
	}
	try {
		read_csv(test_csv, test_images, test_labels);
	} catch (cv::Exception& e) {
		cerr << "Error opening file \"" << test_csv << "\". Reason: " << e.msg << endl;
		// nothing more we can do
		//         exit(1);
	}

	// Make all images the same size
	for(int i=0; i < images.size(); i++) {
		resize(images[i],images[i],images[0].size(),0,0, INTER_NEAREST);
		//imshow("resized-image", images[i]);
		//printf("Image %d size %d\n", i, images[i].size().height * images[i].size().width);
	}
        for(int i=0; i < att_images.size(); i++) {
	        resize(att_images[i],att_images[i],images[0].size(),0,0, INTER_NEAREST);
	        //printf("Image %d size %d\n", i, test_images[i].size().height * test_images[i].size().width);
	}	
        for(int i=0; i < test_images.size(); i++) {
	        resize(test_images[i],test_images[i],images[0].size(),0,0, INTER_NEAREST);
	        //printf("Image %d size %d\n", i, test_images[i].size().height * test_images[i].size().width);
	}	

	cout << "done resizing " << images.size() << endl;
	// Get the height from the first image. We'll need this
	// later in code to reshape the images to their original
	// size:
	int height = images[0].rows;

	// The following lines create an Eigenfaces model for
	// face recognition and train it with the images and
	// labels read from the given CSV file.
	// This here is a full PCA, if you just want to keep
	// 10 principal components (read Eigenfaces), then call
	// the factory method like this:
	//
	//      cv::createEigenFaceRecognizer(10);
	//
	// If you want to create a FaceRecognizer with a
	// confidence threshold (e.g. 123.0), call it with:
	//
	//      cv::createEigenFaceRecognizer(10, 123.0);
	//
	// If you want to use _all_ Eigenfaces and have a threshold,
	// then call the method like this:
	//
	//      cv::createEigenFaceRecognizer(0, 123.0);
	//
	for (int i = 0; i < 10; i++) {
		vector<Mat> images_i;
		vector<int> labels_i;
		for (int j = i * 36; j < i*36 + 36; j++) {
			images_i.push_back(att_images[j]);
			printf("i:%d j:%d images success\n", i, j);
			labels_i.push_back(att_labels[j]);
			printf("i:%d j:%d labels success\n", i, j); 
		}
		Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
		model->train(images_i, labels_i);
		printf("i:%d create and train works!\n", i);
		Mat mean = model->getMat("mean");
		printf("i:%d mean works!\n", i);
		imwrite(format("%s/mean%d.png", mean_folder.c_str(), i), norm_0_255(mean.reshape(1, att_images[0].rows)));
		printf("i:%d save works!\n", i);
	}












        Ptr<FaceRecognizer> model2 = createEigenFaceRecognizer();
	model2->train(images, labels);
	model2->save("eigenfaces_at.yml");
	// The following line predicts the label of a given
	// test image:
	//int predictedLabel = model->predict(testSample);
	//
	// To get the confidence of a prediction call the model with:
	//
			for (vector<Mat>::iterator it = test_images.begin(); it != test_images.end(); it++) {
				int predictedLabel = 100;
				double confidence = 0.0;
				model2->predict(*it, predictedLabel, confidence);
				cout << "Predicted class = " << predictedLabel ;
				cout << " Actual class = " << test_labels[it - test_images.begin()];
				cout <<	" Confidence = " << confidence << endl;
			}
	// Here is how to get the eigenvalues of this Eigenfaces model:
	//    Mat eigenvalues = model->FaceRecognizer::getMat("eigenvalues");
	// And we can do the same to display the Eigenvectors (read Eigenfaces):
	//    Mat W = model->getMat("eigenvectors");
	// Get the sample mean from the training data
	//    Mat mean = model->getMat("mean");
	// Display or save:
	//    if(argc == 2) {
	//        imshow("mean", norm_0_255(mean.reshape(1, images[0].rows)));
	//    } else {
	//        imwrite(format("%s/mean.png", output_folder.c_str()), norm_0_255(mean.reshape(1, images[0].rows)));
	//    }
	// Display or save the Eigenfaces:
	//    for (int i = 0; i < min(10, W.cols); i++) {
	//        string msg = format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
	//        cout << msg << endl;
	// get eigenvector #i
	//        Mat ev = W.col(i).clone();
	// Reshape to original size & normalize to [0...255] for imshow.
	//        Mat grayscale = norm_0_255(ev.reshape(1, height));
	// Show the image & apply a Jet colormap for better sensing.
	//        Mat cgrayscale;
	//        applyColorMap(grayscale, cgrayscale, 2);
	// Display or save:
	//        if(argc == 2) {
	//            imshow(format("eigenface_%d", i), cgrayscale);
	//        } else {
	//            imwrite(format("%s/eigenface_%d.png", output_folder.c_str(), i), norm_0_255(cgrayscale));
	//        }
	//    }

	// Display or save the image reconstruction at some predefined steps:
	//    for(int num_components = min(W.cols, 10); num_components < min(W.cols, 300); num_components+=15) {
	// slice the eigenvectors from the model
	//        Mat evs = Mat(W, Range::all(), Range(0, num_components));
	//        Mat projection = subspaceProject(evs, mean, images[0].reshape(1,1));
	//        Mat reconstruction = subspaceReconstruct(evs, mean, projection);
	// Normalize the result:
	//        reconstruction = norm_0_255(reconstruction.reshape(1, images[0].rows));
	// Display or save:
	//        if(argc == 2) {
	//            imshow(format("eigenface_reconstruction_%d", num_components), reconstruction);
	//        } else {
	//            imwrite(format("%s/eigenface_reconstruction_%d.png", output_folder.c_str(), num_components), reconstruction);
	//        }
	//    }
	// Display if we are not writing to an output folder:
	//    if(argc == 2) {
	//        waitKey(0);
	//    }
	return 0;
}
