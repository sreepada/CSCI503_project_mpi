#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace cv;

// Function Headers
void detectAndDisplay(Mat frame, String output_folder);

// Global variables
// Copy this file from opencv/data/haarscascades to target folder
string face_cascade_name = "haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
string window_name = "Capture - Face detection";
int filenumber; // Number of file to be saved
string filename;

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
            images.push_back(imread(path));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

// Function main
int main(int argc, const char *argv[])
{

    // Check for valid command line arguments, print usage
    // if no arguments were given.
    if (argc < 2) {
    	cout << "usage: " << argv[0] << " <csv.ext> <output_folder> " << endl;
        exit(1);
    }
    string output_folder = ".";
    if (argc >= 3) {
    	output_folder = argv[2];
    }
    // Get the path to your CSV.
    string fn_csv = string(argv[1]);
    // These vectors hold the images and corresponding labels.
    vector<Mat> images;
    vector<int> labels;
    // Read in the data. This can fail if no valid
    // input filename is given.
    try {
    	read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
    	cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
    	// nothing more we can do
	exit(-1);
    }
    // Load the cascade
    if (!face_cascade.load(face_cascade_name))
    {
        printf("--(!)Error loading\n");
        return (-1);
    };

    // Read the image file
    //Mat frame = imread(argv[3]);

    //Mat frame = images[0];

    for (int i = 0; i < images.size(); i++) {
    	Mat frame = images[i];
    	// Apply the classifier to the frame
    	if (!frame.empty())
    	{
       		detectAndDisplay(frame, output_folder);
    	}
    	else
    	{
        	printf(" --(!) No captured frame from image %d -- Break!\n", i);
    	}
    }

    return 0;
}

// Function detectAndDisplay
void detectAndDisplay(Mat frame, String output_folder)
{
    std::vector<Rect> faces;
    Mat frame_gray;
    Mat crop;
    Mat res;
    Mat gray;
    string text;
    stringstream sstm;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    // Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    // Set Region of Interest
    cv::Rect roi_b;
    cv::Rect roi_c;

    size_t ic = 0; // ic is index of current element
    int ac = 0; // ac is area of current element

    size_t ib = 0; // ib is index of biggest element
    int ab = 0; // ab is area of biggest element

    for (ic = 0; ic < faces.size(); ic++) // Iterate through all current elements (detected faces)
    {
        roi_c.x = faces[ic].x;
        roi_c.y = faces[ic].y;
        roi_c.width = (faces[ic].width);
        roi_c.height = (faces[ic].height);

        ac = roi_c.width * roi_c.height; // Get the area of current element (detected face)

        roi_b.x = faces[ib].x;
        roi_b.y = faces[ib].y;
        roi_b.width = (faces[ib].width);
        roi_b.height = (faces[ib].height);

        ab = roi_b.width * roi_b.height; // Get the area of biggest element, at beginning it is same as "current" element

        /*if (ac > ab)
        {
            ib = ic;
            roi_b.x = faces[ib].x;
            roi_b.y = faces[ib].y;
            roi_b.width = (faces[ib].width);
            roi_b.height = (faces[ib].height);
        }*/

        crop = frame(roi_c);
        cout << "roi_c.x" << roi_c.x << endl;
        cout << "roi_b.x" << roi_b.x << endl;
        resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
        cvtColor(crop, gray, CV_BGR2GRAY); // Convert cropped image to Grayscale

        // Form a filename
        filename = "./faces/result";
        stringstream ssfn;
        ssfn << "/" << filenumber << ".png";
        filename = output_folder + ssfn.str();
        cout << output_folder << " " << filename << endl;
        
        filenumber++;

        imwrite(filename, gray);

        //Point pt1(faces[ic].x, faces[ic].y); // Display detected faces on main window - live stream from camera
        //Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
        //rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
    }

    // Show image
    //sstm << "Crop area size: " << roi_b.width << "x" << roi_b.height << " Filename: " << filename;
    //text = sstm.str();

    //putText(frame, text, cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 255), 1, CV_AA);
    //imshow("original", frame);

    //if (!crop.empty())
    //{
    //    imshow("detected", crop);
    //}
    //else
    //    destroyWindow("detected");
}
