#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int threshold1;
int alpha_slider_1 = 255;               //Stored value of trackbar.
const int alpha_slider_max_1 = 255;     //Max value of trackbar.

int threshold2;
int alpha_slider_2 = 255;               //Stored value of trackbar.
const int alpha_slider_max_2 = 255;     //Max value of trackbar.

Mat canny;
Mat pcb_gray;
Mat pcb_houghlines;
Mat pcb_origneel;

//Hough line variables:
int th;
int threshold_slider = 80;
const int threshold_slider_max = 255;

int minLineLength;
int minLineLength_slider = 5;
const int minLineLength_slider_max = 255;

int maxLineGap;
int maxLineGap_slider = 0;
const int maxLineGap_slider_max = 255;
//

///Callback functions for the trackbars:
static void on_trackbar1(int, void*){
    threshold1 = alpha_slider_1;
    Canny(pcb_gray, canny, threshold1, threshold2, 3, true);
}

static void on_trackbar2(int, void*){
    threshold2 = alpha_slider_2;
    Canny(pcb_gray, canny, threshold1, threshold2, 3, true);
}

static void on_trackbar3(int, void*){
    pcb_houghlines = pcb_origneel.clone();   //'Refresh' the output.

    ///Get values from the sliders.
    vector<Vec4i> lines;
    th = threshold_slider;
    minLineLength = minLineLength_slider;
    maxLineGap = maxLineGap_slider;

    ///Apply HoughLinesP.
    HoughLinesP( canny, lines, 0.5, CV_PI/360, th, minLineLength, maxLineGap );

    ///Draw the lines.
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( pcb_houghlines, Point(lines[i][0], lines[i][1]), Point( lines[i][2], lines[i][3]), Scalar(0,0,255), 2);
    }

}


Point find_countour_center(Mat src, Mat mask)
{
    cout << "find_coutour_center()..." << endl;

    Mat contourMap = Mat::zeros(src.rows, src.cols, CV_8UC3);   //Gevonden countours hierop tekenen.
    vector<vector<Point> > contours;                            //each detected contour is stored as a vector of points.
    vector<Vec4i> hierarchy;                                    //contains info about img toplogy. Aantal even groot als aantal countours.

    ///findContours in the mask.
    //8bit img, contours, hierarchy, mode, method, offset
    //mode: CV_RETR_EXTERNAL, CV_RETR_LIST, CV_RETR_CCOMP, CV_RETR_TREE
    //method: CV_CHAIN_APPROX_NONE, CV_CHAIN_APPROX_SIMPLE, ...
    findContours(mask.clone(), contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    ///Draw each contour with a random color.
    int idx;
    for(idx=0 ; idx >= 0; idx = hierarchy[idx][0])
    {
        Scalar color(rand()&255, rand()&255, rand()&255);
        drawContours(contourMap, contours, idx, color, FILLED, 8, hierarchy);
    }

    ///Show the contours.
    imshow("Components (contours)", contourMap);
    waitKey(0);


    ///Find the largest blob.
    vector<Point> grootste_blob = contours[0];
    for(unsigned int i=0; i< contours.size(); i++)
    {
        if(contourArea(contours[i]) > contourArea(grootste_blob))
        {
            grootste_blob = contours[i];
        }
    }

    ///Find center of the largest blob.
    Rect br = boundingRect(grootste_blob);
    Point centerPoint = Point(br.x+br.width/2, br.y+br.height/2);

    return centerPoint;
}

Mat create_mask(Mat src)
{
    cout << "create_mask()..." << endl;

    ///Create a black mask.
    Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);

    ///Create a gray image of the PCB.
    Mat gray;
    cvtColor( src.clone(), gray, COLOR_BGR2GRAY );
    imshow("gray", gray);
    waitKey(0);

    ///Apply binary threshold.
    threshold(gray, mask, 150, 255, THRESH_BINARY);

    ///Show the mask.
    imshow("mask", mask);
    waitKey(0);

    return mask;
}

Mat search_region(Mat src, Point letter_location, int w, int h)
{
    cout << "search_region()..." << endl;

    Mat region;                     ///Regio waar lijnen in gedetecteerd moeten worden.
    Point tlPoint, brPoint;         ///Hoekpunten die een rechthoek definiëren. (topleft en bottomright)

    ///Coördinaten instellen o.b.v. de locatie van de letter en de width en height van de component.
    double ratio = 0.5;             //Width en height vergroten met een factor 'ratio' om de zoek regio te vergroten.
    int w2 = w + (ratio * w);
    int h2 = h + (ratio * h);
    tlPoint.x = letter_location.x - (w2/2);
    tlPoint.y = letter_location.y - (h2/2);
    brPoint.x = letter_location.x + (w2/2);
    brPoint.y = letter_location.y + (h2/2);

    ///Zoek regio creeëren.
    region = Mat(src, Rect(tlPoint, brPoint));

    return region;
}

void put_resistor(Mat src, Mat &dst, int x, int y)
{
    ///Read resistor component.
    Mat resistor = imread("../../img/weerstand_10k.png");

    //dst = src.clone();

    ///Place resistor on the PCB.
    resistor.copyTo(dst(Rect(x, y, resistor.cols, resistor.rows)));

}



Mat get_line_mask(Mat src)
{
    Mat dst = src.clone();
    Mat tmp = src.clone();

    cvtColor(src, tmp, COLOR_BGR2GRAY);

    Canny(tmp, dst, 50.0, 150.0, 3, true);

    Mat color_dst;
    cvtColor( dst, color_dst, COLOR_GRAY2BGR );

    /*vector<Vec4i> lines;
    HoughLinesP( dst, lines, 1, CV_PI/180, 80, 30, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( color_dst, Point(lines[i][0], lines[i][1]),
        Point( lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }*/

    return dst;
}

Mat rotate_image(Mat src, float angle)
{
  /*  Mat dst;
    Point2f src_center(src.cols/2.0F, src.rows/2.0F);
    Mat rotationMatrix = getRotationMatrix2D(src_center, angle, 1.0);
    warpAffine(src, dst, rotationMatrix, src.size());
    return dst;
    */
    Mat dst;
    Point2f center(src.cols/2.0, src.rows/2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    Rect bbox = RotatedRect(center,src.size(), angle).boundingRect();

    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    warpAffine(src, dst, rot, bbox.size());
    return dst;
}

int main(int argc, const char **argv)
{
    cout << "Project: PCB bestukker." << endl;


    bool setupCannyValues = false;
    bool setupHoughLines = false;


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


    ///Erosion and dilation variables:
    int morph_type = MORPH_RECT;
    int erosion_size = 1;
    int dilation_size = 1;
    Mat kernelErosion = getStructuringElement( morph_type,
                        Size(2*erosion_size + 1, 2*erosion_size+1),
                        Point(erosion_size, erosion_size) );
    Mat kernelDilation = getStructuringElement( morph_type,
                        Size(2*dilation_size + 1, 2*dilation_size+1),
                        Point(dilation_size, dilation_size) );

    ///Clone input images.
    Mat pcb = inputImages[0].clone();
    Mat original_templ = inputImages[1].clone();

    canny = pcb.clone();
    cvtColor(pcb.clone(), pcb_gray, COLOR_BGR2GRAY);

    pcb_houghlines = pcb.clone();
    pcb_origneel = pcb.clone();


    ///Setup canny threshold values:
    if(setupCannyValues)
    {
        namedWindow("canny", WINDOW_AUTOSIZE);
        createTrackbar("Threshold 1", "canny", &alpha_slider_1, alpha_slider_max_1, on_trackbar1 );
        createTrackbar("Threshold 2", "canny", &alpha_slider_2, alpha_slider_max_2, on_trackbar2 );
        while(1)
        {
            ///Show canny edge detection result.
            imshow("canny", canny);

            ///Exit loop via "Esc" button on keyboard.
            if (waitKey(1) == 27)   break;

        }
    }
    else
    {
        cout << "Not using the trackbars to setup the canny edge detection threshold values. (boolean is FALSE)" << endl;
        cout << "Using the default values for Canny." << endl;
        Canny(pcb_gray, canny, threshold1, threshold2, 3, true);
    }


    if(setupHoughLines)
    {
        namedWindow("houghlines", WINDOW_AUTOSIZE);

        createTrackbar("threshold", "houghlines", &threshold_slider, threshold_slider_max, on_trackbar3 );
        createTrackbar("minLineLength", "houghlines", &minLineLength_slider, minLineLength_slider_max, on_trackbar3 );
        createTrackbar("maxLineGap", "houghlines", &maxLineGap_slider, maxLineGap_slider_max, on_trackbar3 );
        while(1)
        {
            imshow("houghlines", pcb_houghlines);

            ///Exit loop via "Esc" button on keyboard.
            if (waitKey(1) == 27)   break;
        }

    }
    else
    {
        cout << "Not using the trackbars to setup the houghlinesp parameters. (boolean is FALSE)" << endl;
    }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for(int i=0; i<180; i=i+90)
    {
        ///Rotate template image.
        Mat templ = rotate_image(original_templ.clone(),i);
        imshow("r",templ);

        ///Create Mat object for the result.
        Mat matchResult = Mat::zeros(pcb.rows, pcb.cols, CV_8UC1);

        ///Template matching.
        ///For SQDIFF is the min value the best match. For CCORR and CCOEFF is the max value the best match.
        int match_method[] = {CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED};
        matchTemplate(pcb, templ, matchResult, match_method[5]);
        imshow("matchResult", matchResult);
        waitKey(0);

        ///Normalize
        normalize( matchResult, matchResult, 0, 1, NORM_MINMAX, -1, Mat() );
        imshow("(normalized) matchResult", matchResult);
        waitKey(0);

        ///Threshold
        Mat mask = Mat::zeros(matchResult.rows, matchResult.cols, CV_8UC1);
        threshold(matchResult, mask, 0.75, 1, THRESH_BINARY);
        imshow("Threshold mask", mask);
        waitKey(0);

        ///Erosion + dilation)
        erode(mask, mask, kernelErosion);
        dilate(mask, mask, kernelDilation);
        imshow("Threshold mask after erosion and dilation", mask);
        waitKey(0);

        ///Convert [0;1] scale to [0;255]. (minMaxLoc expects a grayscale image as input.)
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
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    cout << "test" << endl;

/*  Mat canny = get_line_mask(pcb);
    imshow("canny", canny);
    waitKey(0);
  */


/*  Mat pcb_bestukked;
    put_resistor(pcb, pcb_bestukked, 0 ,0);
    imshow("r", pcb_bestukked);
    waitKey(0);
*/

    ///Create mask of the full pcb.
    Mat mask_full = create_mask(pcb);

    ///Get a region mask from the full mask based on the template matching.
    ///TODO: Change these hardcoded values. These should be based on the template matching locations and the size of the component.
    Point letter_location = Point(100,100);
    //int comp_size = 150;
    Mat resistor = imread("../../img/weerstand_10k.png");
    imshow("resistor", resistor);
    waitKey(0);
    int comp_width = resistor.cols;
    int comp_height = resistor.rows;

    Mat mask_region = search_region(mask_full, letter_location, comp_width, comp_height);


    cout << "hi" << endl;

    //debug
    imshow("mask_region", mask_region);
    waitKey(0);

    ///Detect countours and find the center of the biggest contour.
    Point centerPoint = find_countour_center(pcb, mask_region);

    //debug
    ///Visualize centerpoint
    Mat mask_region_color = search_region(pcb, letter_location, comp_width, comp_height);
    circle(mask_region_color, centerPoint, 5, Scalar(0,0,255), 5);
    imshow("centerpoint", mask_region_color);
    waitKey(0);


    return 0;
}
