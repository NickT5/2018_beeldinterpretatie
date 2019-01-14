#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace std;
using namespace cv;

/////////////////////////////////////////////////// FUNCTION DECLARATIONS /////////////////////////////////////////////////////////////////
double degree_to_rad(double angle);
Point rotate_point(Point p1, double angle, Point p0);
Point find_contour_point(Mat mask);
Mat create_mask(Mat src);
Rect search_region(Mat src, Point letterLocation, int w, int h, double angle);
void put_resistor(Mat src, Mat &dst, Point pointRegion, Point pointContour, double angle);
Mat rotate_image(Mat src, double angle);

/////////////////////////////////////////////////// GLOBAL VARIABLES //////////////////////////////////////////////////////////////////////

//int debugLevel; //Nice to have (to implement)
bool debugOn = true;            ///If true, print and show more values/images.
bool fullMaskThreshold = true;

//full mask variables:
int fm_threshold = 150;
int fm_threshold_slider = 150;
const int fm_threshold_slider_max = 255;
//

/////////////////////////////////////////////////// CALLBACK FUNCTIONS //////////////////////////////////////////////////////////////////////
static void on_trackbar_full_mask(int, void*){
    fm_threshold = fm_threshold_slider;
}

/////////////////////////////////////////////////// NON-CALLBACK FUNCTIONS //////////////////////////////////////////////////////////////////////
double degree_to_rad(double angle)
{
    return (angle * M_PI / 180.0);         //Convert angle in degrees to angle in radians.
}

Point rotate_point(Point p1, double angle, Point p0)
{
    //p1 is the point to rotate.
    //p0 is around which to be rotated.
    Point p2;   //Point after rotation.

    double a = degree_to_rad(angle);

    ///Execute the transformation (translatie + rotatie).
    p2.x = cos(a) * (p1.x - p0.x) - sin(a) * (p1.y - p0.y) + p0.x;
    p2.y = sin(a) * (p1.x - p0.x) + cos(a) * (p1.y - p0.y) + p0.y;

    ///Return the rotated point.
    return p2;
}

Point find_contour_point(Mat mask)
{
    cout << "\t\tFind top left point of the (2) largest contour(s)." << endl;

    Mat contourMap = Mat::zeros(mask.rows, mask.cols, CV_8UC3);   //Gevonden countours hierop tekenen.
    vector<vector<Point> > contours;                            //each detected contour is stored as a vector of points.
    vector<Vec4i> hierarchy;                                    //contains info about img toplogy. Aantal even groot als aantal countours.

    ///findContours in the mask.
    //8bit img, contours, hierarchy, mode, method
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
    if(debugOn)
    {
        imshow("Contours of component", contourMap);
        waitKey(0);
    }


    ///Find the two largest blobs.
    vector<Point> grootste_blob = contours[0];
    vector<Point> tweede_grootste_blob = contours[0];
    for(unsigned int i=0; i< contours.size(); i++)
    {
        if(contourArea(contours[i]) > contourArea(grootste_blob))
        {
            grootste_blob = contours[i];
        }
        if(contourArea(contours[i]) > contourArea(tweede_grootste_blob) && contourArea(contours[i]) < contourArea(grootste_blob))
        {
            tweede_grootste_blob = contours[i];
        }
    }

    ///Use both contours if their area is approximatly the same to get the most top left point.
    if(debugOn) cout << "\t\tArea largest blob: " << contourArea(grootste_blob) << endl;
    if(debugOn) cout << "\t\tArea 2nd largest blob: " << contourArea(tweede_grootste_blob) << endl;
    double drempel = 90;
    if( (contourArea(grootste_blob) - contourArea(tweede_grootste_blob)) < drempel )
    {
        if(debugOn) cout << "\t\tBlobs approximatly the same. Use the most topleft point." << endl;
        Rect br1 = boundingRect(grootste_blob);
        Rect br2 = boundingRect(tweede_grootste_blob);
        if(br1.tl().x < br2.tl().x ){
            return br1.tl();
        }
        else{
            return br2.tl();
        }
    }
    else{
        if(debugOn) cout << "\t\tBlobs not approximatly the same." << endl;
        ///Return the top left point of the largest blob.
        Rect br = boundingRect(grootste_blob);
        return br.tl();
    }

}

Mat create_mask(Mat src)
{
    cout << "Creating a mask of the PCB." << endl;

    ///Create a black mask.
    Mat mask = Mat::zeros(src.rows, src.cols, CV_8UC1);

    ///Create a gray image of the PCB.
    Mat gray;
    cvtColor(src.clone(), gray, COLOR_BGR2GRAY);

    ///Use the trackbar to choose the threshold value if bool = true. Otherwise, use the default value.
    if(fullMaskThreshold)
    {
        namedWindow("Full mask", WINDOW_AUTOSIZE);
        createTrackbar("Threshold", "Full mask", &fm_threshold_slider, fm_threshold_slider_max, on_trackbar_full_mask );
        while(1)
        {
            ///Apply binary threshold with value from trackbar.
            threshold(gray, mask, fm_threshold, 255, THRESH_BINARY);

            ///Show the mask.
            imshow("Full mask", mask);

            ///Exit loop via "Esc" button on keyboard.
            if (waitKey(1) == 27)   break;
        }
    }
    else
    {
        cout << "Not using the trackbar to setup full mask threshold. (boolean is FALSE)" << endl;
        cout << "Using the default value." << endl;
        ///Apply binary threshold with value from trackbar.
        threshold(gray, mask, fm_threshold, 255, THRESH_BINARY);

        ///Show the mask.
        imshow("Full mask", mask);
        waitKey(0);
    }

    return mask;
}

Rect search_region(Mat src, Point letterLocation, int w, int h, double angle)
{
    cout << "\t\tDefining a search region around the letter." << endl;

    Point tlPoint, brPoint;         ///Hoekpunten die een rechthoek (van de zoekregio) definiëren. (topleft en bottomright)

    ///Coördinaten instellen o.b.v. de locatie van de letter en de width en height van de component.
    double ratio = 0.5;             //Width en height vergroten met een factor 'ratio' om de zoekregio te vergroten.
    int w2 = w + (ratio * w);
    int h2 = h + (ratio * h);
    tlPoint.x = letterLocation.x - (w2/2);
    tlPoint.y = letterLocation.y - (h2/2);
    brPoint.x = letterLocation.x + (w2/2);
    brPoint.y = letterLocation.y + (h2/2);

    ///Voer een rotatie bewerking uit op de punten indien nodig.
    if(angle != 0){
        tlPoint = rotate_point(tlPoint, angle, letterLocation);
        brPoint = rotate_point(brPoint, angle, letterLocation);
    }

    ///Return the region.
    return Rect(tlPoint, brPoint);
}

void put_resistor(Mat src, Mat &dst, Point pointRegion, Point pointContour, double angle)
{
    cout << "\t\tPlacing a resistor on the PCB." << endl << endl;

    int x0 = pointRegion.x;       ///x-coord t.o.v. globale assenstelsel.
    int y0 = pointRegion.y;       ///y-coord t.o.v. globale assenstelsel.

    int x1 = pointContour.x;       ///x-coord t.o.v. lokale assenstelsel.
    int y1 = pointContour.y;       ///y-coord t.o.v. lokale assenstelsel.

    ///Define the desired point coordinates (=top left point) where to place the component.
    int x = x0 + x1;
    int y = y0 + y1;

    ///Read resistor component.
    Mat resistor = imread("../../img/weerstand_10k.png");

    ///Rotate image if nessecary.
    if(angle != 0) resistor = rotate_image(resistor, angle);

    ///Place resistor on the PCB.
    //x and y is the coordinate of the top left corner.
    resistor.copyTo(dst(Rect(x, y, resistor.cols, resistor.rows)));

}

Mat rotate_image(Mat src, double angle)
{
    /*
    //rotating a matrix with cropping.
    Mat dst;
    Point2f src_center(src.cols/2.0F, src.rows/2.0F);
    Mat rotationMatrix = getRotationMatrix2D(src_center, angle, 1.0);
    warpAffine(src, dst, rotationMatrix, src.size());
    return dst;
    */

    //rotating a matrix without cropping.
    //source: https://github.com/milq/cvrotate2D/blob/master/cvrotate2D.cpp
    Mat dst;
    Point2f center(src.cols/2.0, src.rows/2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    Rect bbox = RotatedRect(center,src.size(), angle).boundingRect();

    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    Size outputSize = bbox.size();
    warpAffine(src, dst, rot, outputSize);
    return dst;
}

/////////////////////////////////////////////////// MAIN //////////////////////////////////////////////////////////////////////
int main(int argc, const char **argv)
{
    cout << "Project: PCB bestukker!" << endl;


    ///Setup CLP.
    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image0 i   |  | (required) image path to the pcb image }"
        "{ image1 j   |  | (required) image path to the resistor template }"
        "{ image2 k   |  | (required) image path to the diode template }"
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

    ///Read, empty check, (resize), blur and show the images.
    for(int i=0;i<aantImages; i++)
    {
        inputImages.push_back(imread(imgNames[i]));                                             ///Read image.
        if(inputImages[i].empty()){ cerr << "One or more empty images!" << endl;  return -1;}   ///Empty check.
        //resize(inputImages[i],inputImages[i],Size(), 0.75, 0.75);                             ///Resize.
        GaussianBlur(inputImages[i], inputImages[i], Size(5,5),0);                              ///Gaussian blur.
        string windowTitles[] = {"input image", "template resistor", "template diode"};         ///Window title.
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
    //Mat original_templ = inputImages[1].clone();
    Mat template_resistor = inputImages[1].clone();
    Mat template_diode = inputImages[2].clone();

    ///Add all the templates to a vector of Mat's.
    vector<Mat> allTemplates;
    allTemplates.push_back(template_resistor);
    //allTemplates.push_back(template_diode);

    Mat result_multi = pcb.clone();    ///Create Mat for the result with multiple bounding boxes (around the letter).
    Mat pcb_bestukked = pcb.clone();   ///Mat for PCB with the components (=end result).


    ///Read component(s)
    Mat resistor = imread("../../img/weerstand_10k.png");
    int resistor_width = resistor.cols;
    int resistor_height = resistor.rows;

    ///Create mask of the full pcb.
    Mat mask_full = create_mask(pcb);

    Mat original_templ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(unsigned int t=0; t<allTemplates.size(); t++)
    {
        original_templ = allTemplates[t];
        if(t == 0) cout << "Detecting and placing resistors..." << endl;


        for(double angle=0; angle<180; angle=angle+90)
        {
            ///Rotate template image.
            cout << "\tRotate template. (" << angle << "°)" << endl;
            Mat templ = rotate_image(original_templ.clone(), angle);
            if(debugOn) imshow("Template", templ);

            ///Create Mat object for the result.
            Mat matchResult = Mat::zeros(pcb.rows, pcb.cols, CV_8UC1);

            ///Template matching.
            //For SQDIFF is the min value the best match. For CCORR and CCOEFF is the max value the best match.
            cout << "\tTemplate matching." << endl;
            int match_method[] = {CV_TM_SQDIFF, CV_TM_SQDIFF_NORMED, CV_TM_CCORR, CV_TM_CCORR_NORMED, CV_TM_CCOEFF, CV_TM_CCOEFF_NORMED};
            matchTemplate(pcb, templ, matchResult, match_method[5]);
            if(debugOn)
            {
                imshow("matchResult", matchResult);
                waitKey(0);
            }

            ///Normalize
            normalize( matchResult, matchResult, 0, 1, NORM_MINMAX, -1, Mat() );
            if(debugOn)
            {
                imshow("(normalized) matchResult", matchResult);
                waitKey(0);
            }

            ///Threshold
            cout << "\tThreshold the template match result." << endl;
            Mat mask = Mat::zeros(matchResult.rows, matchResult.cols, CV_8UC1);
            threshold(matchResult, mask, 0.75, 1, THRESH_BINARY);
            if(debugOn)
            {
                imshow("Threshold mask", mask);
                waitKey(0);
            }

            ///Apply opening (erosion + dilation).
            cout << "\tApply erosion and dilation." << endl;
            erode(mask, mask, kernelErosion);
            dilate(mask, mask, kernelDilation);
            if(debugOn)
            {
                imshow("Threshold mask after erosion and dilation", mask);
                waitKey(0);
            }

            ///Convert [0;1] scale to [0;255].
            mask.convertTo(mask,CV_8UC1);
            mask *= 255;

            ///Search blobs with findContours.
            cout << "\tSearch blobs (location of the letters)." << endl;
            vector<vector<cv::Point> > contours;
            findContours(mask, contours, CV_RETR_EXTERNAL, CHAIN_APPROX_NONE);
            cout << "\tFound " << contours.size() << " blobs." << endl;

            ///Find the location of the letter,
            /// define a search region around that letter,
            /// search for the two biggests blob in that region,
            /// get coordinates from the blob(s),
            /// place a component on the correct location.
            for(unsigned int i=0;i<contours.size();i++)
            {
                ///Get the bounding box rectangle around the contour.
                Rect region = boundingRect(contours[i]);

                ///Define the corner coordinates for the bounding box. (Only for visualisation)
                Point corner = Point(region.tl().x, region.tl().y);
                Point oppositeCorner = Point(region.tl().x+templ.cols, region.tl().y+templ.rows);

                ///Get the (center)point of the letter. (center of the bounding box)
                Point letterLocation = Point(region.tl().x+templ.cols/2, region.tl().y+templ.rows/2);

                ///Draw the bounding box. (Only for visualisation)
                rectangle(result_multi, corner, oppositeCorner, Scalar(0,0,255));

                ///Draw the corners. (Only for visualisation)
                circle(result_multi,corner,2,Scalar(255,0,255),2);                //violet
                circle(result_multi,oppositeCorner,2,Scalar(255,255,0),2);        //cyan
                circle(result_multi,letterLocation,2,Scalar(0,0,255),2);          //rood

                ///Show the bounding box around the letter.
                imshow("Result with multiple bounding boxes", result_multi);
                waitKey(0);

                ///Nu weten we de locatie v/d letter. Dit gebruiken we om een regio te maken waar we moeten zoeken naar contouren.
                //if(debugOn) cout << "\t\tletterLocation: x = " << letterLocation.x << " ; y = " << letterLocation.y << endl;

                ///Get a region of the full_mask based on the location of the letter and the width and height of the component.
                Rect rectRegion = search_region(mask_full, letterLocation, resistor_width, resistor_height, angle);

                ///Debug print.
                //if(debugOn) cout << "\t\trectRegion top left: x = " << rectRegion.tl().x << " ; y = " << rectRegion.tl().y << endl;
                //if(debugOn) cout << "\t\trectRegion bottom right: x = " << rectRegion.br().x << " ; y = " << rectRegion.br().y << endl;

                ///Cut a part from the full mask based on the rectangle region. (region around the letter).
                cout << "\t\tCut a part from the full mask." << endl;
                Mat mask_region = Mat(mask_full, rectRegion);

                ///Show the mask region around the letter.
                if(debugOn)
                {
                    imshow("mask region", mask_region);
                    waitKey(0);
                }

                ///Detect contours and find the center of the biggest contour.
                Point tlContour = find_contour_point(mask_region);        //a local point

                ///Only for visualisation. (show the point returned from find_contour_center() )
                if(debugOn)
                {
                    Mat mask_region_color = Mat(pcb, rectRegion);
                    circle(mask_region_color, tlContour, 2, Scalar(255,255,0), 2);  //cyan
                    imshow("mask region_color", mask_region_color);
                    waitKey(0);
                }

                ///Place a resistor on the correct location.
                put_resistor(pcb, pcb_bestukked, rectRegion.tl(), tlContour, angle);
                imshow("PCB bestukked", pcb_bestukked);
                waitKey(0);
            }

        }

    }

    return 0;
}
