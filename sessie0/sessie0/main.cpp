#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char **argv)
{
    cout << "Sessie0" << endl;

    ///Setup CommandLineParser
    CommandLineParser parser(argc, argv,
        "{ help h    |  | show this message         }"
        "{ image i   |  | (required) input image path  }"
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
    imshow("Input image",inputImage);
    waitKey(0);

    ///Split and show the color channels
    vector<Mat> bgr;
    split(inputImage,bgr);

    imshow("blue channel",bgr[0]);
    waitKey(0);
    imshow("green channel",bgr[1]);
    waitKey(0);
    imshow("red channel",bgr[2]);
    waitKey(0);

    ///Convert BGR image to GRAYSCALE
    Mat imgGray;
    cvtColor(inputImage,imgGray,COLOR_BGR2GRAY);
    imshow("Image in grayscale",imgGray);
    waitKey(0);

    ///Print the pixel values
    cout << "Print the pixel values of the image:" << endl;
    cout << "Dimensions of the image: " << imgGray.rows << " x " << imgGray.cols << endl;
    int row, col;
    int pixelValue;
    for(row=0;row<imgGray.rows;row++)
    {
        for(col=0;col<imgGray.cols;col++)
        {
            pixelValue = imgGray.at<uchar>(row,col);
            cout << pixelValue << " ";
        }
        cout << endl;
    }

    ///Make a canvas (lines, circles, rectangels, etc)
    ///Mat canvas = Mat(400,400,CV_8UC3,Scalar(255,255,255);    ///white background
    Mat canvas = Mat::zeros(400,400,CV_8UC3);                   ///black background
    line(canvas, Point(10,50), Point(100,150), Scalar(0,255,0),2);                          ///Syntax: img, startpoint, endpoint, color, thickness, linetype
    circle(canvas, Point(200,200), 25, Scalar(0,0,255), 2);                                 ///Syntax: img, center, radius, color, thickness (-1 voor filled)
    ellipse(canvas, Point(100,200), Size(150,50.), 90, 0, 360, Scalar(255,255,0), 3, 8 );   ///Syntax: img, center, rotated angle, arc(0-360), color, thickness
    rectangle(canvas, Point(300,300), Point(400,400), Scalar(0,240,255), -1);               ///Syntax: img, hoekpunt, opposite hoekpunt, color,thickness ( -1 voor filled)

    imshow("Picasso", canvas);
    waitKey(0);

    return 0;
}
