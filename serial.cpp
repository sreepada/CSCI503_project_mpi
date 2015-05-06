#include "timer.cc"
#include "detectAndRecog.cpp"

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
	// These vectors hold the images and corresponding labels.

	ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path;
	double seconds = read_timer();
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path);
		if(!path.empty()) {
				vector<Mat> croppedFaces = cropFaces(path);
				int result = recognizeSuspect(croppedFaces, "trained.ysm");
		}
	}
	cout << "time elasped: " << read_timer() - seconds << endl;
	return 0;
}