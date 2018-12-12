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

    ///Create a list of points to store the middlepoints of the bounding box from the detection.
    vector<Point> middle;

    ///Create HOG people detector.
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

    Mat frame;
    Mat canvas;

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

        ///Double the size.
        resize(frame, frame, Size(), 2, 2);

        ///Use the detector to detect the person in the frame.
        vector<Rect> people;
        vector<double> weights;
        hog.detectMultiScale(frame, people, weights);

        canvas = frame.clone();

        ///Draw the rectangle and output the scoretext.
        for(unsigned i = 0; i < people.size(); i++){
            ///Haal het middelpunt op. Straks gebruiken voor het tracken.
            middle.push_back(Point(people[i].x+people[i].width/2, people[i].y+people[i].height/2));

            ///Draw a bounding box on the canvas.
            rectangle(canvas, people[i], Scalar(255, 255, 0));

            ///Draw text (=score) on the canvas.
            stringstream temp;
            temp << (double)weights[i];
            putText(canvas, temp.str(), Point(people[i].x, people[i].y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 0));
        }

        ///Draw the tracking lines.
        for(unsigned j = 1; j < middle.size(); j++){
                ///Lijn tekenen tussen de vorige en de huidige punt in een rode kleur.
                line(canvas, middle[j-1], middle[j], Scalar(0, 0, 255));
        }

        ///Show the canvas.
        //imshow("Frame", frame);
        imshow("Canvas", canvas);

        ///Press ESC on the keyboard to exit.
        char c = (char)waitKey(30);
        if(c==27){
            break;
        }
    }

    /// When everything done, release the video capture object
    video.release();

    return 0;
}
