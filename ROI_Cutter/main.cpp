#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

///Global variables to keep track of the (clicked) positions.
Point startPoint, currentPoint, endPoint;
///Global variable to know in which state the program is. Recording or not-recording.
bool recordingState = false;

///Callback function for mouse events.
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if( event == EVENT_LBUTTONDOWN )
    {
        startPoint.x = x;       ///Get the x-pos
        startPoint.y = y;       ///Get the y-pos
        cout << "Start point: " << startPoint.x << ";" << startPoint.y << endl;
        recordingState = true;   ///Change the recording state to TRUE.

    }

    if( event == EVENT_MOUSEMOVE && recordingState == true)
    {
        currentPoint.x = x;
        currentPoint.y = y;
        cout << "Current point: " << currentPoint.x << ";" << currentPoint.y << endl;
    }

    if( event == EVENT_LBUTTONUP)
    {
        endPoint.x = x;
        endPoint.y = y;
        cout << "End point: " << endPoint.x << ";" << endPoint.y << endl;
        recordingState = false; ///Change the recording state to FALSE.
    }

}

int main(int argc, const char **argv)
{
    cout << "ROI cutter!" << endl;
    cout << "Draw a rectangle and save that ROI as a seperate image." << endl;

     ///Setup CommandLineParser
    CommandLineParser parser(argc, argv,
        "{ help h    |  | show this message         }"
        "{ image i   |  | (required) path to the image.  }"
    );

    if(parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    ///Collect CLP data
    string imgName = parser.get<string>("image");

    ///Check if CLP data is valid
    if(imgName.empty())
    {
        parser.printMessage();
        return -1;
    }

    ///Read image
    Mat inputImage;
    inputImage = imread(imgName);

    ///Check if image is empty
    if(inputImage.empty()){
        cerr << "Empty image!" << endl; return -1;
    }

    ///Show the image
    string windowName = "Input image";
    imshow(windowName, inputImage);


    ///Set the callback function for the mouse events.
    setMouseCallback(windowName, CallBackFunc, NULL);


    waitKey(0);

    return 0;
}
