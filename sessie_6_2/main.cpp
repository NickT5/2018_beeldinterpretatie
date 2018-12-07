#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    cout << "Sessie 6_2 (personen detector)" << endl;

    CommandLineParser parser(argc, argv,
        "{ help h  | | show this message }"
        "{ video v | | (required) path to people.mp4 }"
    );

    if( parser.has("help") ){
        parser.printMessage();
        return -1;
    }

    ///Read CLP input.
    string video_location = parser.get<string>("video");

    ///Check if strings (variable locations) are empty.
    if  ( video_location.empty() ){
        cerr << "Empty input string!" << endl;
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

    //video.set(CAP_PROP_POS_FRAMES, 10);       ///Skip x-frames of the video.

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


        imshow("Frame", frame);

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
