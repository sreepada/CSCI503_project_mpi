#include "timer.cc"
#include "detectAndRecog.cpp"
#include "trainer.cpp"

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

int main(int argc, const char *argv[]) {
	// Check for valid command line arguments, print usage
	// if no arguments were given.
	if (argc < 1) {
		cout << "usage: " << argv[0] << " <csv.ext> " << endl;
		exit(1);
	}
	
	// Get the path to your CSV.
	string filename = string(argv[1]);
	double seconds = read_timer();
	// These vectors hold the images and corresponding labels.

	someMain("train.csv", "test.csv", "att.csv");
	cout << "training took: " << read_timer() - seconds << " seconds"<< endl;

	ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, ';');
		getline(liness, classlabel);
		if(!path.empty()) {
				vector<Mat> croppedFaces = cropFaces(path);
				vector<int> croppedLabel;
				for(int i=0; i < croppedFaces.size(); i++) {
					croppedLabel.push_back(atoi(classlabel.c_str()));
				}   
				int result = recognizeSuspect(croppedFaces, croppedLabel, "eigenfaces_at.yml");
		}
	}
	cout << "time elasped: " << read_timer() - seconds << endl;
	return 0;
}
