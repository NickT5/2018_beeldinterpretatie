#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char **argv)
{
 CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image1 i   |  | (required) image path to imageColor.jpg }"
        "{ image2 j   |  | (required) image path to imageBimodal.jpg }"
    );

    if(parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    ///Collect CLP data
    string imgName1 = parser.get<string>("image1");
    string imgName2 = parser.get<string>("image2");

    ///Create Mat objects
    Mat inputImage1, inputImage2;

    ///Read the images
    inputImage1 = imread(imgName1);
    inputImage2 = imread(imgName2);
    ///Check if images are empty
    if(inputImage1.empty() || inputImage2.empty()){
        cerr << "Empty image!" << endl; return -1;
    }
    resize(inputImage1,inputImage1,Size(),0.50,0.50); /// Size halveren
    resize(inputImage2,inputImage2,Size(),0.50,0.50); /// Size halveren

    ///Show the input images
    imshow("Input image1",inputImage1);
    imshow("Input image2",inputImage2);
    waitKey(0);


    ///Sessie 1 opdracht 1.1.1: Segmenteer skin pixels uit de image
    Mat img = inputImage1.clone();
    vector<Mat> bgr;

    ///Split channels
    split(img, bgr);
    Mat B = bgr[0]; Mat G = bgr[1]; Mat R = bgr[2];

    ///Create black mask
    Mat mask_1 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    //Mat mask_2 = mask_1.clone();

    cout << img.rows << ", " << img.cols << endl;
    for(int row=0;row<img.rows;row++)
    {
        for(int col=0;col<img.cols;col++)
        {
            ///b, g en r pixelvalues op positie row,col
            int b = B.at<uchar>(row,col);
            int g = G.at<uchar>(row,col);
            int r = R.at<uchar>(row,col);

            ///Als het voldoet, dan maak je het een witte pixel in de mask.
            ///Anders blijft het zwart.
            if( (r>95) && (g>40) && (b>20) &&
                ((max(r,max(g,b)) - min(r, min(g,b)))>15) &&
                (abs(r-g)>15) &&
                (r>g) && (r>b)){
                    mask_1.at<uchar>(row,col) = 255;
            }
        }
    }
    imshow("Skin threshold",mask_1);
    waitKey(0);

///////////////////////////////////////////////////////////////////////////
    ///Sessie 1 opdracht 1.1.2.  OTSU threshold
    ///Verbeteren van OTSU threshold met hist equalization of CLAHE

    Mat imgGray,imgThreshold;

    ///Convert img to grayscale
    cvtColor(inputImage2, imgGray, COLOR_BGR2GRAY );

    ///OTSU threshold
    //syntax: src,dest,threshold value,max value, type (THRESH_BINARY, THRESH_OTSU, ...)
    threshold(imgGray,imgThreshold,0,255,THRESH_BINARY|THRESH_OTSU); //OTSU overruled die 0

    ///Show threshold image result
    imshow("Threshold image", imgThreshold );
    waitKey(0);

    ///Resultaat niet ideaal door schaduw
    ///Originele grijs image verbeteren adhv 2 technieken:
    ///Techniek 1 = Histogram equalisation:
    Mat imgGray_equalized;
    equalizeHist(imgGray,imgGray_equalized);
    imshow("ticket na H.E.",imgGray_equalized);
    waitKey(0);

    threshold(imgGray_equalized,imgThreshold,0,255,THRESH_BINARY|THRESH_OTSU);
    imshow("Histogram equalisation + OTSU",imgThreshold);
    waitKey(0);

    ///Techniek 2 = CLAHE:
    Mat result_CLAHE;
    Ptr<CLAHE> clahe_pointer = createCLAHE();
    clahe_pointer->setTilesGridSize(Size(15,15));
    clahe_pointer->setClipLimit(1);
    clahe_pointer->apply(imgGray.clone(),result_CLAHE);
    imshow("ticket na CLAHE",result_CLAHE);
    waitKey(0);

    threshold(result_CLAHE,imgThreshold,0,255,THRESH_BINARY|THRESH_OTSU);
    imshow("CLAHE + OTSU",imgThreshold);

    waitKey(0);

    return 0;
}
