///Sessie 2: kleurruimtes
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

///Globale variabelen voor drie trackbars:
int alpha_slider_1 = 13;               //Stored value of trackbar.
const int alpha_slider_max_1 = 180;     //Max value of trackbar.

int alpha_slider_2 = 159;                //Stored value of trackbar.
const int alpha_slider_max_2 = 180;     //Max value of trackbar.

int alpha_slider_3 = 50;               //Stored value of trackbar.
const int alpha_slider_max_3 = 255;     //Max value of trackbar.
//Opmerking: Hue is tot 180. Niet 255, omdat het niet voorgestelt kan worden met 1byte.
int hueUp;   //7
int hueLow;  //155
int sat;     //154

///Callback functions voor de drie trackbars:
static void on_trackbar1(int, void*){
    hueUp = alpha_slider_1;
}

static void on_trackbar2(int, void*){
    hueLow = alpha_slider_2;
}

static void on_trackbar3(int, void*){
    sat = alpha_slider_3;
}


int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image1 i   |  | (required) img path to sign.jpg }"
        "{ image2 j   |  | (required) img path to sign2.jpg }"
        "{ image3 k   |  | (required) img path to sign3.jpg }"
        "{ image4 l   |  | (required) img path to sign4.jpg }"
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
    string imgName4 = parser.get<string>("image4");

    ///Declare Mat objects
    Mat inputImg1, inputImg2, inputImg3, inputImg4;

    ///Read the input images
    inputImg1 = imread(imgName1);
    inputImg2 = imread(imgName2);
    inputImg3 = imread(imgName3);
    inputImg4 = imread(imgName4);

    ///Check if input images are empty
    if(inputImg1.empty() || inputImg2.empty() || inputImg3.empty() || inputImg4.empty() ){
        cerr << "Empty image(s)!" << endl; return -1;
    }
    resize(inputImg1,inputImg1,Size(),0.75,0.75); /// Size verkleinen
    resize(inputImg2,inputImg2,Size(),0.75,0.75); /// Size verkleinen
    resize(inputImg3,inputImg3,Size(),0.50,0.50); /// Size verkleinen
    resize(inputImg4,inputImg4,Size(),0.50,0.50); /// Size verkleinen

    ///Show the input images
    imshow("Input image 1",inputImg1);
    imshow("Input image 2",inputImg2);
    imshow("Input image 3",inputImg3);
    imshow("Input image 4",inputImg4);
    waitKey(0);


///Opdracht 1: Segmenteer de verkeersborden in de BGR kleurenruimte
    vector<Mat>bgr, bgr2, bgr3, bgr4;

    ///Split
    split(inputImg1,bgr);
    split(inputImg2,bgr2);
    split(inputImg3,bgr3);
    split(inputImg4,bgr4);

    ///Leeg masker maken voor elk verkeersbord.
    Mat mask_sign = Mat::zeros(inputImg1.rows, inputImg1.cols, CV_8UC1);
    Mat mask_sign2 = Mat::zeros(inputImg2.rows, inputImg2.cols, CV_8UC1);
    Mat mask_sign3 = Mat::zeros(inputImg3.rows, inputImg3.cols, CV_8UC1);
    Mat mask_sign4 = Mat::zeros(inputImg4.rows, inputImg4.cols, CV_8UC1);

    ///Rode kanaal nemen (=grayscale) en een threshold waarde kiezen bv: 150
    threshold(bgr[2],mask_sign,150,255,THRESH_BINARY);
    threshold(bgr2[2],mask_sign2,150,255,THRESH_BINARY);
    threshold(bgr3[2],mask_sign3,150,255,THRESH_BINARY);
    threshold(bgr4[2],mask_sign4,150,255,THRESH_BINARY);

    ///Toon de threshold maskers
    imshow("Mask 1",mask_sign);
    imshow("Mask 2",mask_sign2);
    imshow("Mask 3",mask_sign3);
    imshow("Mask 4",mask_sign4);
    waitKey(0);

    ///Nu het gecreeerde masker gebruiken om geen zwart wit afbeelding te hebben,
    ///maar wel een img met enkel het rood van het stopbord.
    ///Dus elk gesplitst kanaal vermenigvuldigen met het masker en terug samenvoegen:
    ///Dit doen voor elk masker:
    Mat tmp_1  = bgr[0] & mask_sign;
    Mat tmp2_1 = bgr[1] & mask_sign;
    Mat tmp3_1 = bgr[2] & mask_sign;

    Mat tmp_2  = bgr2[0] & mask_sign2;
    Mat tmp2_2 = bgr2[1] & mask_sign2;
    Mat tmp3_2 = bgr2[2] & mask_sign2;

    Mat tmp_3  = bgr3[0] & mask_sign3;
    Mat tmp2_3 = bgr3[1] & mask_sign3;
    Mat tmp3_3 = bgr3[2] & mask_sign3;

    Mat tmp_4  = bgr4[0] & mask_sign4;
    Mat tmp2_4 = bgr4[1] & mask_sign4;
    Mat tmp3_4 = bgr4[2] & mask_sign4;

    ///Kanalen samenvoegen m.b.v. mixChannels():
    Mat segmented_sign = Mat::zeros(inputImg1.rows, inputImg1.cols, CV_8UC3); //Mat::zeros is nodig.
    Mat segmented_sign2 = Mat::zeros(inputImg2.rows, inputImg2.cols, CV_8UC3); //Mat::zeros is nodig.
    Mat segmented_sign3 = Mat::zeros(inputImg3.rows, inputImg3.cols, CV_8UC3); //Mat::zeros is nodig.
    Mat segmented_sign4 = Mat::zeros(inputImg4.rows, inputImg4.cols, CV_8UC3); //Mat::zeros is nodig.

    Mat in[] = { tmp_1, tmp2_1,tmp3_1};
    Mat in2[] = { tmp_2, tmp2_2,tmp3_2};
    Mat in3[] = { tmp_3, tmp2_3,tmp3_3};
    Mat in4[] = { tmp_4, tmp2_4,tmp3_4};

    int from_to[] = {0,0, 1,1, 2,2};
    int from_to2[] = {0,0, 1,1, 2,2};
    int from_to3[] = {0,0, 1,1, 2,2};
    int from_to4[] = {0,0, 1,1, 2,2};
    //inputarray, number of inputmatrices,output array,number of output matrices. Pairs specifying which channels are copied and where, number of pairs.
    mixChannels(in,  3, &segmented_sign,  1, from_to, 3);
    mixChannels(in2, 3, &segmented_sign2, 1, from_to2, 3);
    mixChannels(in3, 3, &segmented_sign3, 1, from_to3, 3);
    mixChannels(in4, 3, &segmented_sign4, 1, from_to4, 3);

    ///Toon de segmenteerde verkeerdsborden
    imshow("Segmented sign 1 ", segmented_sign);
    imshow("Segmented sign 2 ", segmented_sign2);
    imshow("Segmented sign 3 ", segmented_sign3);
    imshow("Segmented sign 4 ", segmented_sign4);
    waitKey(0);

    //Nadeel, segmenteren in RGB space: moeilijk om 1 tint te segmenteren en moeilijk om een universele te hebben.

///Opdracht 2: Segmenteer de verkeersborden in de HSV kleurenruimte
    ///Opmerking: eerste foto van CLP wordt gebruikt tijdens HSV segmentatie. De rest niet.

    ///Convert to HSV space.
    Mat hsvImage = Mat::zeros(inputImg1.rows, inputImg1.cols, CV_8UC3);
    cvtColor(inputImg1, hsvImage, CV_BGR2HSV);

    ///Split in hsv channels.
    vector<Mat> hsv;
    split(hsvImage,hsv);
    Mat H = hsv[0];
    Mat S = hsv[1];
    Mat V = hsv[2];

    ///Trackbars aanmaken op een window.
    namedWindow("My window", WINDOW_AUTOSIZE);
    char TrackbarName[50];
    sprintf(TrackbarName, "Upper H");
    createTrackbar(TrackbarName, "My window", &alpha_slider_1, alpha_slider_max_1, on_trackbar1 );
    createTrackbar("Lower H", "My window", &alpha_slider_2, alpha_slider_max_2, on_trackbar2 );
    createTrackbar("SAT", "My window", &alpha_slider_3, alpha_slider_max_3, on_trackbar3 );

    ///Maskers aanmaken
    Mat mask_hLow   = Mat::zeros(hsvImage.rows, hsvImage.cols, CV_8UC1);
    Mat mask_hUpper = Mat::zeros(hsvImage.rows, hsvImage.cols, CV_8UC1);
    Mat mask_sat    = Mat::zeros(hsvImage.rows, hsvImage.cols, CV_8UC1);

    ///Erosie en dilatie variabelen declareren:
    /******/
    int morph_type = MORPH_RECT;
    int erosion_size = 1;
    int dilation_size = 2;
    Mat kernelErosion = getStructuringElement( morph_type,
                        Size(2*erosion_size + 1, 2*erosion_size+1),
                        Point(erosion_size, erosion_size) );
    Mat kernelDilation = getStructuringElement( morph_type,
                        Size(2*dilation_size + 1, 2*dilation_size+1),
                        Point(dilation_size, dilation_size) );
    /*******/

    while(1){
        ///Slider waarden toepassen op de maskers
        //inRange() alternatief voor threshold()
        inRange(H, hueLow, 180, mask_hLow);
        inRange(H, 0, hueUp, mask_hUpper);
        inRange(S, sat, 255, mask_sat);

        ///De drie maskers combineren tot 1 masker:
        Mat mask_combined = Mat::zeros(hsvImage.rows, hsvImage.cols, CV_8UC1);
        mask_combined = (mask_hLow | mask_hUpper) & mask_sat;

        //imshow("Voor opening", mask_combined);        //DEBUG
        ///Opening (= erosion + dilation) om ruis te verminderen
        erode(mask_combined, mask_combined, kernelErosion);
        dilate(mask_combined, mask_combined, kernelDilation);
        //imshow("Na opening", mask_combined);          //DEBUG

        ///FindContours
        Mat contourMap = Mat::zeros(mask_combined.rows, mask_combined.cols, CV_8UC3);
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;  //contains info about img toplogy. Aantal evengroot als aantal countours.

        findContours(mask_combined,contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

        ///Zoek grootste blob (verkeersbord)
        vector<Point> grootste_blob = contours[0];
        for(unsigned int i=0; i< contours.size(); i++)
        {
            if(contourArea(contours[i]) > contourArea(grootste_blob))
            {
                grootste_blob = contours[i];
            }
        }
        Rect roi = boundingRect(grootste_blob);
        Mat blob = inputImg1(roi);
        imshow("Grootste blob", blob);

        int idx;
        for(idx=0 ; idx >= 0; idx = hierarchy[idx][0])
        {
            Scalar color(rand()&255, rand()&255, rand()&255);
            drawContours(contourMap, contours, idx, color, 8, FILLED, hierarchy);
        }
        ///Toon resultaat van findContours
        imshow("Contours", contourMap);

        ///convexHull() function checks a curve for convexity defects and corrects it.
        Mat hull_result = Mat::zeros(mask_combined.rows, mask_combined.cols, CV_8UC3);
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


        ///Combined masker toepassen op de BGR kanalen.
        Mat B_masked = bgr[0] & mask_combined;
        Mat G_masked = bgr[1] & mask_combined;
        Mat R_masked = bgr[2] & mask_combined;

        ///Samenbrengen tot Mat object.
        Mat in_2[] = {B_masked, G_masked, R_masked};
        Mat result = Mat::zeros(inputImg1.rows,inputImg1.cols,CV_8UC3);
        int from_to_2[] = {0,0, 1,1, 2,2};
        mixChannels( in_2, 3, &result, 1, from_to_2, 3);

        ///Toon het resultaat
        imshow("My window", result);

        ///Uit de lus via "Esc"
        if (waitKey(1) == 27)   break;

    }

    return 0;
}
