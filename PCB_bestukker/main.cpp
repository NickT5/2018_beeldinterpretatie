#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char **argv)
{
    cout << "Project: PCB bestukker." << endl;

    ///Setup CLP.
    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image0 i   |  | (required) image path to the pcb image }"
        "{ image1 j   |  | (required) image path to the template }"
    );

    if(parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    int aantImages = argc-1;

    ///Collect CLP data (image names).
    vector<string> imgNames;
    for(int i=0; i<aantImages; i++)
    {
        string s = "image" + std::to_string(i);
        imgNames.push_back(parser.get<string>(s));
    }

    ///Create Mat objects for the images.
    vector<Mat> inputImages;

    ///Read, empty check, (resize) and show the images.
    for(int i=0;i<aantImages; i++)
    {
        inputImages.push_back(imread(imgNames[i]));                                             ///Read image.
        if(inputImages[i].empty()){ cerr << "One or more empty images!" << endl;  return -1;}   ///Empty check.
        //resize(inputImages[i],inputImages[i],Size(), 0.75, 0.75);                             ///Resize.
        string windowTitles[] = {"input image", "template image"};                              ///Window title.
        imshow(windowTitles[i], inputImages[i]);                                                ///Show image.
    }
    waitKey(0);


    ///Clone input images.
    Mat pcb = inputImages[0].clone();
    Mat templ = inputImages[1].clone();

    ///Create Mat object for the result.
    Mat matchResult = Mat::zeros(pcb.rows, pcb.cols, CV_8UC1);

    ///Template matching
    int match_method[] = {CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED};

    matchTemplate(pcb, templ, matchResult, match_method[3]);
    imshow("Result (single) template match", matchResult);
    waitKey(0);

    ///Met normalize() kan je ervoor zorgen dat de min waarde -> 0 en max -> 1.
    ///Dit zorgt ervoor dat het template match result duidelijker wordt.
    normalize( matchResult, matchResult, 0, 1, NORM_MINMAX, -1, Mat() );
    imshow("TM Normalized", matchResult);
    waitKey(0);

    ///Bounding box:
    ///In matchResult is het meer wit waar een gevonden match is en anders is het zwart.
    ///We kunnen dus minMaxLoc() gebruiken.
    ///minMaxLoc() finds the global min and max in an array
    double minVal; double maxVal;   //min en max value.
    Point minLoc; Point maxLoc;    //Locatie waar de min en max gevonden zijn.
    minMaxLoc(matchResult, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    /// 1 bounding box tekenen:
    Point oppositeCorner = Point(maxLoc.x+templ.cols,maxLoc.y+templ.rows);
    Mat result = inputImages[0].clone();
    rectangle(result, maxLoc, oppositeCorner, Scalar(0,0,255));

    imshow("Result with bounding box",result);
    waitKey(0);




    return 0;
}
