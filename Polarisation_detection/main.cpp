#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/////////////////////////////////////////////////// FUNCTION DECLARATIONS /////////////////////////////////////////////////////////////////
bool get_polarisation(Mat);

/////////////////////////////////////////////////// NON-CALLBACK FUNCTIONS //////////////////////////////////////////////////////////////////////
bool get_polarisation(Mat mask)
{
    ///Voting system on number of white pixels.
    ///Count pixels left and right from center.x of the image.

    bool polarisationLeft;

    ///Get center of Mat.
    int cx = mask.cols/2;

    vector<Point> leftPoints;   ///Points left from center.
    vector<Point> rightPoints;  ///Points right from center.

    ///Loop over image:
    int row, col;
    int px;     //pixel value
    for(row=0; row<mask.rows; row++)
    {
        for(col=0; col<mask.cols; col++)
        {
            ///Get pixel value.
            px = mask.at<uchar>(row,col);

            ///Check if pixel is white.
            if(px == 255)
            {
                ///Check wether pixel point is on the left or right side.
                if(col<cx) leftPoints.push_back(Point(row, col));
                else      rightPoints.push_back(Point(row, col));
            }
            else{
                ///Not a white pixel. Don't count.
            }
        }
    }


    ///Compare sizes of vector with white points.
    cout << "left white points size: " << leftPoints.size() << endl;
    cout << "right white points size: " << rightPoints.size() << endl;
    if(leftPoints.size() < rightPoints.size()){
        polarisationLeft = false;
    }
    else{
        polarisationLeft = true;
    }

    ///Opportunity for improvement -> text removal. Search location of letter, draw black rectangle on letters, recheck polarisation.

    return polarisationLeft;
}

/////////////////////////////////////////////////// MAIN //////////////////////////////////////////////////////////////////////
int main(int argc, const char **argv)
{
    ///Variables
    bool polarisation;
    Mat mask1, mask2, diode;
    vector<Mat> allMasks;

    ///Read images
    mask1 = imread("img/roi_1.png");
    mask2 = imread("img/roi_2.png");
    diode = imread("img/diode2.png");

    allMasks.push_back(mask1);
    allMasks.push_back(mask2);

    cout << mask1.cols << "; " << mask1.rows << endl;
    cout << mask2.cols << "; " << mask2.rows << endl;
    cout << diode.cols << "; " << diode.rows << endl;

    ///Show images
    imshow("mask1", mask1);
    imshow("mask2", mask2);
    imshow("diode", diode);
    waitKey(0);

    for(unsigned i=0; i<allMasks.size(); i++)
    {
        if(i == 0 ) cout << "Mask 1..." << endl;
        else        cout << "Mask 2..." << endl;

        polarisation  = get_polarisation(allMasks[i]);
        if(polarisation)
        {
            cout << "Kathode links" << endl;
        }
        else{
            cout << "Kathode rechts." << endl;
        }
    }

    waitKey(0);

    return 0;
}
