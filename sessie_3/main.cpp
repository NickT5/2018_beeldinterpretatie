#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char **argv)
{
    cout << "Sessie 3 - template based matching" << endl;

    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image1 i   |  | (required) image path to recht.jpg }"
        "{ image2 j   |  | (required) image path to template.jpg }"
        "{ image3 k   |  | (required) image path to rot.jpg }"
    );

    if(parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    ///Collect CLP data
    string imgName1 = parser.get<string>("image1");
    string imgName2 = parser.get<string>("image2");
    string imgName3 = parser.get<string>("image3");

    ///Create Mat objects
    Mat inputImage, templateImage, rotatedImage;

    ///Read the images
    inputImage = imread(imgName1);
    templateImage = imread(imgName2);
    rotatedImage = imread(imgName3);

    ///Check if images are empty
    if(inputImage.empty() || templateImage.empty() || rotatedImage.empty()){
        cerr << "Empty imagee!" << endl; return -1;
    }

    resize(inputImage,inputImage,Size(),0.75,0.75); /// Size halveren van recht.jpg
    resize(templateImage,templateImage,Size(),0.75,0.75); /// Size halveren van template.jpg
    resize(rotatedImage,rotatedImage,Size(),0.75,0.75); /// Size halveren van rot.jpg

    ///Show the input images
    imshow("Input image",inputImage);                   //recht.jpg
    imshow("Template image",templateImage);             //template.jpg
    imshow("Rotated input image",rotatedImage);         //rot.jpg
    waitKey(0);

    ///Opdracht 1: Gebruik template matching om een object te vinden in een inputbeeld

    ///Copy
    Mat tmp1 = inputImage.clone();
    Mat tmp2 = templateImage.clone();

    ///Create Mat object for the result.
    Mat matchResult = Mat::zeros(tmp1.rows, tmp1.cols, CV_8UC1);

    ///Template matching
    int match_method = CV_TM_CCOEFF_NORMED; ////CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED
    matchTemplate(tmp1, tmp2, matchResult, match_method);
    imshow("Result (single) template match", matchResult);

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
    Point oppositeCorner = Point(maxLoc.x+tmp2.cols,maxLoc.y+tmp2.rows);
    Mat result = inputImage.clone();
    rectangle(result, maxLoc, oppositeCorner, Scalar(0,0,255));

    imshow("Result with bounding box",result);
    waitKey(0);


    ///Opdracht 2: Pas de template matching aan om lokaal naar maxima te zoeken, zodanig dat je alle matches vind

    ///*******/Erosie en dilatie variabelen
    int morph_type = MORPH_RECT;
    int erosion_size = 2;
    int dilation_size = 2;
    Mat kernelErosion = getStructuringElement( morph_type,
                        Size(2*erosion_size + 1, 2*erosion_size+1),
                        Point(erosion_size, erosion_size) );
    Mat kernelDilation = getStructuringElement( morph_type,
                        Size(2*dilation_size + 1, 2*dilation_size+1),
                        Point(dilation_size, dilation_size) );
    ///*******/

    ///Maak een masker door te thresholden op matchResult.
    Mat mask = Mat::zeros(matchResult.rows,matchResult.cols,CV_8UC1);
    threshold(matchResult, mask,0.75,1,THRESH_BINARY);

    ///Masker beetje properde maken via opening (= erosion + dilation)
    erode(mask, mask, kernelErosion);
    dilate(mask, mask, kernelDilation);
    imshow("Threshold mask (met erosie en dilatie)",mask);
    waitKey(0);

    ///[0;1] schaal converteren naar [0;255] schaal. Nodig omdat minMaxLoc een 1-kanaal img als input neemt.
    mask.convertTo(mask,CV_8UC1);
    mask *= 255;

    ///FindContours
    vector<vector<cv::Point> > contours;
    findContours(mask, contours, CV_RETR_EXTERNAL,CHAIN_APPROX_NONE);

    ///Create Mat for the result with multiple bounding boxes.
    Mat result_multi = inputImage.clone();

    for(unsigned int i=0;i<contours.size();i++)
    {
        ///Search max in the regions of the contours.
        Rect region = boundingRect(contours[i]);
        Mat temp = mask(region);
        //imshow("temp",temp);  //debug
        //waitKey(0);           //debug
        Point maxLoc;
        minMaxLoc(temp, NULL, NULL, NULL, &maxLoc);

        ///Hoekpunten definieren voor een bounding box.
        Point corner = Point(maxLoc.x + region.tl().x, maxLoc.y + region.tl().y);
        Point oppositeCorner = Point(maxLoc.x+region.tl().x+tmp2.cols, maxLoc.y+region.tl().y+tmp2.rows);

        ///Bounding box tekenen.
        rectangle(result_multi, corner, oppositeCorner, Scalar(0,0,255));

        ///Visualisatie van de punten voor DEBUG
        //circle(result_multi,maxLoc,5,Scalar(255,0,0),5);                //blauw
        //circle(result_multi,region.tl(),5,Scalar(0,255,0),3);           //groen
        //circle(result_multi,region.br(),5,Scalar(0,255,255),3);         //geel
        circle(result_multi,corner,3,Scalar(255,0,255),3);                //violet
        circle(result_multi,oppositeCorner,3,Scalar(255,255,0),3);        //cyan

        ///Voor DEBUG
        //imshow("result with multiple bounding boxes", result_multi);
        //waitKey(0);
    }

    ///Toon het resultaat met de gedecteerde objecten.
    imshow("result with multiple bounding boxes", result_multi);
    waitKey(0);


    ///Extra: Pas de template matching aan geroteerde objecten te vinden (roteren van beeld, rotatedRect, warpPerspective op hoekpunten)
    ///Rotation invariant matching:
    // Ipv template te rotateren, ga je de input image rotereren voor elke graad[°] en je houd dit bij in 3D vector. (voor elke graad een 2D matrix).
    //Vervolgens op elk van die afbeelding template matching toepassen. Vervolgens, rond te match tekenen met lijnen, want met rechthoek gaat het niet. (staat recht).
    //Voor de lijnen is een inverse transformatie matrix nodig.
    //Ergens voorbeeld code te vinden

    return 0;
}
