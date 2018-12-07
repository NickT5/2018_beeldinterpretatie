#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    cout << "Sessie 6 (gezichten herkennen met Viola & Jones)" << endl;

    CommandLineParser parser(argc, argv,
        "{ help h  | | show this message }"
        "{ video v | | (required) path to video }"
        "{ cascadeHAAR a | | (required) path to HAARclassifier }"
        "{ cascadeLBP l | | (required) path to LBPclassifier }"
    );

    if( parser.has("help") ){
        parser.printMessage();
        return -1;
    }

    ///Read CLP input.
    string video_location = parser.get<string>("video");
    string HAAR_location = parser.get<string>("cascadeHAAR");
    string LBP_location = parser.get<string>("cascadeLBP");

    ///Check if strings (variable locations) are empty.
    if  (video_location.empty() || HAAR_location.empty() || LBP_location.empty()){
        cerr << "Empty input strings!" << endl;
        return -1;
    }

    ///Create and open a video capture.
    VideoCapture video;
    video.open(video_location);

    ///Check if video capture is successfully opened.
    if( !video.isOpened() ){
        cerr << "Could not open or find the video." << endl;
        return -1;
    }

    ///Create the HAAR and LBP classifier.
    CascadeClassifier detectorHAAR(HAAR_location);
    CascadeClassifier detectorLBP(LBP_location);

    ///Load the HAAR and LBP classifier using the
    detectorHAAR.load(HAAR_location);
    detectorLBP.load(LBP_location);

    if( detectorHAAR.empty() ){
        cerr << "Failed to load the HAAR classifier!" << endl;
        return -1;
    }
    else{ cout << "Successfully loaded the HAAR CLF." << endl; }

    if( detectorLBP.empty() ){
        cerr << "Failed to load the LBP classifier!" << endl;
        return -1;
    }
    else{ cout << "Successfully loaded the LBP CLF." << endl; }


    /// When everything done, release the video capture object
    video.release();

    return 0;
}
