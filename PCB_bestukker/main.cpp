#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat rotate_image(Mat src, float angle)
{
    Mat dst;
    Point2f src_center(src.cols/2.0F, src.rows/2.0F);
    Mat rotationMatrix = getRotationMatrix2D(src_center, angle, 1.0);
    warpAffine(src, dst, rotationMatrix, src.size());
    return dst;
}

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
        GaussianBlur(inputImages[i], inputImages[i], Size(5,5),0);                              ///Gaussian blur.
        string windowTitles[] = {"input image", "template image"};                              ///Window title.
        imshow(windowTitles[i], inputImages[i]);                                                ///Show image.
    }
    waitKey(0);


    ///Clone input images.
    Mat pcb = inputImages[0].clone();
    Mat templ = inputImages[1].clone();

    ///Rotate image.
    for(int i=90; i<360; i=i+90)
    {
        Mat r = rotate_image(templ,i);
        imshow("r",r);
        waitKey(0);
    }

    ///Create Mat object for the result.
    Mat matchResult = Mat::zeros(pcb.rows, pcb.cols, CV_8UC1);

    ///Template matching.
    ///For SQDIFF is the min value the best match. For CCORR and CCOEFF is the max value the best match.
    int match_method[] = {CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED};
    matchTemplate(pcb, templ, matchResult, match_method[5]);
    imshow("matchResult", matchResult);
    waitKey(0);

    ///Met normalize() kan je ervoor zorgen dat de min waarde -> 0 en max -> 1.
    ///Dit zorgt ervoor dat het template match result duidelijker wordt.
    normalize( matchResult, matchResult, 0, 1, NORM_MINMAX, -1, Mat() );
    imshow("(normalized) matchResult", matchResult);
    waitKey(0);

    ///Find the location of the best match.
    double minVal; double maxVal;   //min en max value.
    Point minLoc; Point maxLoc;    //Locatie waar de min en max gevonden zijn.
    minMaxLoc(matchResult, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    ///Draw a bounding box around the best match.
    Mat result = inputImages[0].clone();
    Point oppositeCorner = Point(maxLoc.x+templ.cols, maxLoc.y+templ.rows);
    rectangle(result, maxLoc, oppositeCorner, Scalar(0,0,255));

    ///Show the result for the template match (for a single object).
    imshow("Result of template match (for a single object)", result);
    waitKey(0);



/**************************Find all objects***************************/

    ///*******/Erosie en dilatie variabelen
    int morph_type = MORPH_RECT;
    int erosion_size = 1;
    int dilation_size = 1;
    Mat kernelErosion = getStructuringElement( morph_type,
                        Size(2*erosion_size + 1, 2*erosion_size+1),
                        Point(erosion_size, erosion_size) );
    Mat kernelDilation = getStructuringElement( morph_type,
                        Size(2*dilation_size + 1, 2*dilation_size+1),
                        Point(dilation_size, dilation_size) );
    ///*******/

    ///Create a mask by applying a threshold on the template match.
    Mat mask = Mat::zeros(matchResult.rows, matchResult.cols, CV_8UC1);
    threshold(matchResult, mask, 0.75, 1, THRESH_BINARY);

    ///Clean the mask via opening (= erosion + dilation)
    erode(mask, mask, kernelErosion);
    dilate(mask, mask, kernelDilation);
    imshow("Threshold mask after erosion and dilation", mask);
    waitKey(0);

    ///Convert [0;1] scale to [0;255]. (minMaxLoc expects a grayscale image as input.
    mask.convertTo(mask,CV_8UC1);
    mask *= 255;

    ///Search blobs with FindContours.
    vector<vector<cv::Point> > contours;
    findContours(mask, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE);

    ///Create Mat for the result with multiple bounding boxes.
    Mat result_multi = inputImages[0].clone();

    ///Draw a bounding box for each found contour.
    for(unsigned int i=0;i<contours.size();i++)
    {
        ///Search max in the regions of the contours.
        Rect region = boundingRect(contours[i]);
        Mat temp = mask(region);

        Point maxLoc;
        minMaxLoc(temp, NULL, NULL, NULL, &maxLoc);

        ///Define the corners for the bounding box.
        Point corner = Point(maxLoc.x + region.tl().x, maxLoc.y + region.tl().y);
        Point oppositeCorner = Point(maxLoc.x+region.tl().x+templ.cols, maxLoc.y+region.tl().y+templ.rows);

        ///Draw the bounding box.
        rectangle(result_multi, corner, oppositeCorner, Scalar(0,0,255));

        ///Visualisatie van de punten voor DEBUG
        //circle(result_multi,maxLoc,5,Scalar(255,0,0),5);                //blauw
        //circle(result_multi,region.tl(),5,Scalar(0,255,0),3);           //groen
        //circle(result_multi,region.br(),5,Scalar(0,255,255),3);         //geel
        circle(result_multi,corner,3,Scalar(255,0,255),3);                //violet
        circle(result_multi,oppositeCorner,3,Scalar(255,255,0),3);        //cyan

    }

    ///Show the result with all the detected objects.
    imshow("Result with multiple bounding boxes", result_multi);
    waitKey(0);

    return 0;
}
