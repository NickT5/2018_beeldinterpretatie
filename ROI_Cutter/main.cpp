#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

///Global variables to keep track of the (clicked) positions.
Point startPoint, currentPoint, endPoint;
///Global variable to know in which state the program is. Recording or not-recording.
bool recordingState = false;
///Global variable to decide when to create a roi from the drawn rectangle.
bool stopProgram = false;

///Global variable to keep track on how many rectangles were drawn.
/// This will be used to make a unique name for each roi image.
unsigned int numRectangles = 0;

///Callback function for mouse events.
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if( event == EVENT_LBUTTONDOWN )
    {
        startPoint.x = x;       ///Get the x-pos
        startPoint.y = y;       ///Get the y-pos
        cout << "Start point: " << startPoint.x << ";" << startPoint.y << endl;
        recordingState = true;   ///Change the recording state to TRUE.
        stopProgram = false;

    }

    if( event == EVENT_MOUSEMOVE && recordingState == true)
    {
        currentPoint.x = x;
        currentPoint.y = y;
        //cout << "Current point: " << currentPoint.x << ";" << currentPoint.y << endl;
    }

    if( event == EVENT_LBUTTONUP)
    {
        endPoint.x = x;
        endPoint.y = y;
        cout << "End point: " << endPoint.x << ";" << endPoint.y << endl;
        recordingState = false; ///Change the recording state to FALSE.
        numRectangles++;

    }

    if ( event == EVENT_RBUTTONDOWN)
    {
        ///Reset all positions.
        startPoint.x = 0;   startPoint.y = 0;
        currentPoint.x = 0;   currentPoint.y = 0;
        endPoint.x = 0;   endPoint.y = 0;
    }

    ///Mss nog een defenitieve keuze bevestig knop?
    if (event == EVENT_MBUTTONDOWN)
    {
        stopProgram = true;
    }

}

int main(int argc, const char **argv)
{
    cout << "ROI cutter!" << endl;
    cout << "Draw a rectangle and save that ROI as a seperate image." << endl;
    cout << "Create a new image per drawn rectangle." << endl;

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

    ///Clone the input image.
    Mat canvas = inputImage.clone();
    imshow(windowName, canvas);

    ///Set the callback function for the mouse events.
    setMouseCallback(windowName, CallBackFunc, NULL);

    while(!stopProgram){
        if(recordingState)
        {
             rectangle(canvas, startPoint, currentPoint, Scalar(0,240,255), 2); ///Syntax: img, hoekpunt, opposite hoekpunt, color,thickness ( -1 voor filled)
        }
        else{
            rectangle(canvas, startPoint, endPoint, Scalar(0,0,255), 2);
        }

        ///Show canvas
        imshow(windowName, canvas);
        waitKey(30);

        ///Create a ROI.
        //todo

        ///Neem een nieuwe kloon zodat niet elke rechthoek ooit getekent wordt op de canvas.
        canvas = inputImage.clone();

    }

    if(stopProgram)
    {
        cout << "Closing program..." << endl;
        waitKey(500);
    }



    return 0;
}
