#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image i   |  | (required) image path to imageColorAdapted.jpg }"
    );

    if(parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    ///Collect CLP data
    string imgName = parser.get<string>("image");
    cout << imgName;

    ///Create Mat objet
    Mat inputImage;

    ///Read input image
    inputImage = imread(imgName);
    ///Check if input image
    if(inputImage.empty()){
        cerr << "Empty image!" << endl; return -1;
    }

    resize(inputImage,inputImage,Size(),0.50,0.50); /// Size halveren

    ///Show the input image
    imshow("Input image",inputImage);
    waitKey(0);

    ///Deel sessie1 opdracht 1.1 nodig voor een binair masker.
    Mat img = inputImage.clone();
    vector<Mat> bgr;

    ///Split channels
    split(img, bgr);
    Mat B = bgr[0]; Mat G = bgr[1]; Mat R = bgr[2];

    ///Create black mask
    Mat mask_1 = Mat::zeros(img.rows, img.cols, CV_8UC1);

    for(int row=0;row<img.rows;row++)
    {
        for(int col=0;col<img.cols;col++)
        {
            ///b, g en r pixelvalues op positie i,j
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


    ///Erosie en dilatie
    Mat erosion_dst, dilation_dst;
    Mat opening_dst, closing_dst;

    int morph_type = MORPH_RECT;
    int erosion_size = 1;
    int dilation_size = 3;  //Groter dan erosion_size om ledematen te verbinden

    //Returns a kernel of the specified size and shape for morph operations.
    //shape,size of element, width of element, height of element,
    Mat kernelErosion = getStructuringElement( morph_type,
                        Size(2*erosion_size + 1, 2*erosion_size+1),
                        Point(erosion_size, erosion_size) );
    Mat kernelDilation = getStructuringElement( morph_type,
                        Size(2*dilation_size + 1, 2*dilation_size+1),
                        Point(dilation_size, dilation_size) );

    ///The operations
    erode(mask_1, erosion_dst, kernelErosion);
    dilate(mask_1, dilation_dst, kernelDilation);

    ///Opening (= erosion + dilation)
    erode(mask_1, opening_dst, kernelErosion);
    dilate(opening_dst, opening_dst, kernelDilation);

    ///Closing (= dilation + dilation)
    dilate(mask_1, closing_dst, kernelDilation);
    erode(closing_dst, closing_dst, kernelErosion);

    ///Show the results
    imshow("Erosion", erosion_dst);
    imshow("Dilation", dilation_dst);
    imshow("Opening", opening_dst);
    imshow("closing", closing_dst);

    waitKey(0);

    ///Resulterende blob defecten wegwerken m.a.w. edge blobs smoothen

    ///Stap 1: connected components vinden en daarna labellen(verschillende kleuren)
    Mat contourMap = Mat::zeros(img.rows, img.cols, CV_8UC3);   //Gevonden countours hierop tekenen.
    vector<vector<Point> > contours;    //each detected contour is stored as a vector of points.
    vector<Vec4i> hierarchy;  //contains info about img toplogy. Aantal even groot als aantal countours.

    //8bit img, contours, hierarchy, mode, method, offset
    findContours(opening_dst, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    //iterate trough all the top-level contours,
    //draw each connected conponent with its own random color
    int idx;
    for(idx=0 ; idx >= 0; idx = hierarchy[idx][0])
    {
        Scalar color(rand()&255, rand()&255, rand()&255);
        drawContours(contourMap, contours, idx, color, FILLED, 8, hierarchy);
    }

    imshow("Components (contours)", contourMap);
    waitKey(0);


    ///Stap 2
    ///convexHull() function checks a curve for convexity defects and corrects it.
    //convexiteit = bolrondheid
    Mat hull_result = Mat::zeros(img.rows, img.cols, CV_8UC3);
    vector<vector<Point> >hull( contours.size() );

    //Find the convex hull for each contour.
    for( size_t i = 0; i < contours.size(); i++ )
    {
        convexHull( Mat(contours[i]), hull[i], false); //false staat voor clockwise orientatie vd hull.
    }
    //Draw the hull results.
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rand()&255, rand()&255, rand()&255 );
        drawContours( hull_result, hull, (int)i, color, 1, 8, vector<Vec4i>(), 0, Point() );
    }

    imshow("Convex hull", hull_result);
    waitKey(0);

    return 0;
}
