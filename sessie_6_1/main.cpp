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

    ///Load the HAAR and LBP classifier.
    detectorHAAR.load(HAAR_location);
    detectorLBP.load(LBP_location);

    ///Check if classifiers are successfully loaded.
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


    ///Skip the intro of the video.
    video.set(CAP_PROP_POS_FRAMES, 52);

    Mat frame;
    ///Take a frame from the video capture each iteration until ESC is pressed.
    while( true ){
        ///Take a frame from the videocapture.
        video >> frame;

        ///Check wether the frame is empty.
        ///Possible when there's a problem with the frame or the end of the video is reached.
        if(frame.empty()){
            cerr << "EMPTY FRAME!" << endl;
            cout << "Possible when there's a problem with the frame or the end of the video is reached." << endl;
            return 0;
        }

        ///Clone the frame.
        Mat tempHAAR = frame.clone();
        Mat tempLBP = frame.clone();

        ///Vector of rectangles where each rectangle contains the detected object,
        /// the rectangles may be partially outside the original image.
        vector<Rect> objectsHAAR, objectsLBP;

        ///Vector of detection numbers for the corresponding objects.
        ///An object’s number of detections is the number of neighboring positively
        /// classified rectangles that were joined together to form the object.
        vector<int> scoreHAAR, scoreLBP;

        ///Extra parameters voor de .detectMultiScale() functie.
        double scaleFactor=1.1;
        int minNeighbors=3;

        ///Detects objects of different sizes in the input image.
        ///The detected objects are returned as a list of rectangles.
        detectorHAAR.detectMultiScale(tempHAAR, objectsHAAR, scoreHAAR, scaleFactor, minNeighbors);
        detectorLBP.detectMultiScale(tempLBP, objectsLBP, scoreLBP, scaleFactor , minNeighbors);

        Mat canvas = frame.clone();

        ///Teken de resultaten van de HAAR detector.
        for(unsigned int i = 0; i < objectsHAAR.size(); i++){
            circle(canvas, Point(objectsHAAR[i].x+objectsHAAR[i].width/2, objectsHAAR[i].y+objectsHAAR[i].height/2), objectsHAAR[i].width/2, Scalar(0, 0, 255));
            stringstream temp;
            temp << (int)scoreHAAR[i];
            putText(canvas, temp.str(), Point(objectsHAAR[i].x, objectsHAAR[i].y), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255));
        }

        ///Teken de resultaten van de LBP detector.
        for(unsigned i = 0; i < objectsLBP.size(); i++){
            rectangle(canvas, objectsLBP[i], Scalar(0, 255, 0), 2);
            stringstream temp;
            temp << (int)scoreLBP[i];
            putText(canvas, temp.str(), Point(objectsLBP[i].x, objectsLBP[i].y), FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));
        }

        ///Opm: De CLF's zijn niet rotatie-invariant. Bij scheve poses is er geen detectie.
       // imshow("Frame", frame);
        imshow("Canvas", canvas);

        ///Press ESC on keyboard to exit.
        char c = (char)waitKey(30);
        if(c==27){
            break;
        }
    }


    /// When everything done, release the video capture object
    video.release();

    return 0;
}
