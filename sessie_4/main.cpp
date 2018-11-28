#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, const char **argv)
{
    cout << "Sessie 4 - Keypoint detection and matching" << endl;

    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image0 i   |  | (required) image path to fitness_image.png }"
        "{ image1 j   |  | (required) image path to fitness_object.png }"
        "{ image2 k   |  | (required) image path to kinderbueno_image.png }"
        "{ image3 l   |  | (required) image path to kinderbueno_object.png }"
    );

    if(parser.has("help"))
    {
        parser.printMessage();
        return -1;
    }

    int aantImages = argc-1;

    ///Collect CLP data (image names)
    vector<string> imgNames;
    for(int i=0; i<aantImages; i++)
    {
        string s = "image" + std::to_string(i);
        imgNames.push_back(parser.get<string>(s));
    }

    ///Create Mat objects
    vector<Mat> inputImages;

    ///Read, empty check, resize and show the images
    for(int i=0;i<aantImages; i++)
    {
        inputImages.push_back(imread(imgNames[i]));                                                             ///Read
        if(inputImages[i].empty()){ cerr << "Empty image!" << endl;  return -1;}                                ///Empty check
        //resize(inputImages[i],inputImages[i],Size(), 0.75, 0.75);                                               ///Resize
        string windowTitles[] = {"fitness image", "fitness object", "kinderbueno image", "kinderbueno object"}; ///Window title
        imshow(windowTitles[i], inputImages[i]);                                                                ///Show
    }
    waitKey(0);


    ///Loop for both sets of photos (fitness and kinderbueno)
    unsigned int index;
    for(unsigned int i=0;i<2;i++)
    {
        cout << endl << "Set nr." << i << ":" << endl;

        ///Create keypoints for each type of detector (ORB, BRISK and AKAZE) for both _image and _obj.
        vector<KeyPoint> orbKeypoints_1;            //Voor _img
        vector<KeyPoint> orbKeypoints_2;            //Voor _obj
        vector<KeyPoint> briskKeypoints_1;
        vector<KeyPoint> briskKeypoints_2;
        vector<KeyPoint> akazeKeypoints_1;
        vector<KeyPoint> akazeKeypoints_2;

        ///index om de juiste set van images te clonen uit inputImages[]
        index = i * 2;

        ///Clone the _image and _object from the sets
        Mat _img = inputImages[index].clone();
        Mat _obj = inputImages[index+1].clone();

        ///Create detectors (ORB, BRISK and AKAZE)
        Ptr<ORB> ORBDetector = ORB::create();
        Ptr<BRISK> BRISKDetector = BRISK::create();
        Ptr<AKAZE> AKAZEDetector = AKAZE::create();

        ///Detect keypoints on the image and object with an ORB detector
        ORBDetector->detect(_img, orbKeypoints_1);
        ORBDetector->detect(_obj, orbKeypoints_2);

        ///Detect keypoints on the image and object with an BRISK detector
        BRISKDetector->detect(_img, briskKeypoints_1);
        BRISKDetector->detect(_obj, briskKeypoints_2);

        ///Detect keypoints on the image and object with an AKAZE detector
        AKAZEDetector->detect(_img, akazeKeypoints_1);
        AKAZEDetector->detect(_obj, akazeKeypoints_2);

        ///Print the number of detected keypoints for each detector.
        cout << "ORB detector found "   << orbKeypoints_1.size()   << " keypoints on _img." << endl;
        cout << "ORB detector found "   << orbKeypoints_2.size()   << " keypoints on _obj." << endl;
        cout << "BRISK detector found " << briskKeypoints_1.size() << " keypoints on _img." << endl;
        cout << "BRISK detector found " << briskKeypoints_2.size() << " keypoints on _obj." << endl;
        cout << "AKAZE detector found " << akazeKeypoints_1.size() << " keypoints on _img." << endl;
        cout << "AKAZE detector found " << akazeKeypoints_2.size() << " keypoints on _obj." << endl;

        ///Create Mat objects (output images) to show the keypoint detections:
        Mat ORB_out_1 = _img.clone();
        Mat ORB_out_2 = _obj.clone();
        Mat BRISK_out_1 = _img.clone();
        Mat BRISK_out_2 = _obj.clone();
        Mat AKAZE_out_1 = _img.clone();
        Mat AKAZE_out_2 = _obj.clone();

        ///Draw the keypoints from the ORB detector.
        drawKeypoints(_img, orbKeypoints_1, ORB_out_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
        drawKeypoints(_obj, orbKeypoints_2, ORB_out_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

        ///Draw the keypoints from the BRISK detector.
        drawKeypoints(_img, briskKeypoints_1, BRISK_out_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
        drawKeypoints(_obj, briskKeypoints_2, BRISK_out_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

        ///Draw the keypoints from the AKAZE detector.
        drawKeypoints(_img, akazeKeypoints_1, AKAZE_out_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
        drawKeypoints(_obj, akazeKeypoints_2, AKAZE_out_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );

        ///Create a canvas for the images.
        Mat canvasImage(_img.rows, _img.cols*3, CV_8UC3);
        ///Copy the Mat images from each detector to the canvas.
        ORB_out_1.copyTo(canvasImage(Rect(0,0, _img.cols,_img.rows)));
        BRISK_out_1.copyTo(canvasImage(Rect(_img.cols,0, _img.cols,_img.rows)));
        AKAZE_out_1.copyTo(canvasImage(Rect(_img.cols*2,0, _img.cols,_img.rows)));

        ///Create a canvas for the objects
        Mat canvasObject(_obj.rows, _obj.cols*3, CV_8UC3);
        ///Copy the Mat objects from each detector to the canvas.
        ORB_out_2.copyTo(canvasObject(Rect(0,0, _obj.cols,_obj.rows)));
        BRISK_out_2.copyTo(canvasObject(Rect(_obj.cols,0, _obj.cols,_obj.rows)));
        AKAZE_out_2.copyTo(canvasObject(Rect(_obj.cols*2,0, _obj.cols,_obj.rows)));

        imshow("Canvas images (ORB, BRISK, AKAZE)", canvasImage);
        imshow("Canvas objects (templates) (ORB, BRISK, AKAZE)", canvasObject);
        waitKey(0);

        ///Opdracht 2
        cout << "Descriptoren van de keypoints bepalen en matchen.." << endl << "(enkel voor de keypoints van de ORB detector)" << endl;
        Mat descriptor_image;
        Mat descriptor_object;

        ///Bereken de descriptoren voor de keypoints van _img en _obj
        ORBDetector->compute(_img, orbKeypoints_1, descriptor_image);
        ORBDetector->compute(_obj, orbKeypoints_2, descriptor_object);

        ///Brute force matcher (met Eucl. dist.)
        BFMatcher matcher(NORM_L2);
        vector<DMatch> matches;
        matcher.match(descriptor_image, descriptor_object, matches);

        ///Matches tekenen.
        Mat img_matches;
        drawMatches(_img.clone(), orbKeypoints_1, _obj.clone(), orbKeypoints_2, matches, img_matches);

        ///Matches visualiseren.
        imshow("Matches", img_matches);
        waitKey(0);

        ///Opdracht 3 (RANSAC)
        ///In de matches zitten nog wat false positives. Dit verbeteren we met RANSAC.
        cout << "RANSAC toepassen om het resultaat te verbeteren.." << endl;

        ///Min en max distance tussen keypoints berekenen.
        double max_dist = 0.0; double min_dist = 100.0;
        for( int i = 0; i < descriptor_object.rows; i++ )
        {
            double dist = matches[i].distance;
            if( dist == 0) continue;                //als match te goed is (=0), skip het. Anders geeft dit later een probleem (nul good_matches)
            if( dist < min_dist ) min_dist = dist;
            if( dist > max_dist ) max_dist = dist;
        }

        cout << "Aantal matches: " << matches.size() << endl;
        cout << "Max distance: " << max_dist << endl;
        cout << "Min distance " << min_dist << endl;

        ///Houd enkel de goede matches bij.
        vector<DMatch> good_matches;
        for( int i = 0; i < descriptor_object.rows; i++ )
        {
            if(matches[i].distance < (3*min_dist))    good_matches.push_back(matches[i]);
        }

        cout << "Aantal goede matches: " << good_matches.size() << endl;

        ///Goede matches tekenen.
        Mat img_good_matches;
        drawMatches(_img.clone(), orbKeypoints_1, _obj.clone(), orbKeypoints_2, good_matches, img_good_matches);

        ///Goede matches visualiseren.
        imshow( "Goede matches", img_good_matches );
        waitKey(0);

        ///Localize the object
        vector<Point2f> obj;
        vector<Point2f> scene;
        for( unsigned int i = 0; i < good_matches.size(); i++ )
        {
            ///Get the keypoints from the good matches
            scene.push_back( orbKeypoints_1[good_matches[i].trainIdx].pt );
            obj.push_back( orbKeypoints_2[good_matches[i].queryIdx].pt );
        }

        ///Vind de beste homography matrix tussen obj en scene.
        Mat H = findHomography( obj, scene, CV_RANSAC );

        Mat detection_img = inputImages[index].clone();
        Mat detection_obj = inputImages[index+1].clone();

        ///Get the corners from the object to be "detected".
        vector<Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0);
        obj_corners[1] = cvPoint(detection_obj.cols, 0 );
        obj_corners[2] = cvPoint(detection_obj.cols, detection_obj.rows);
        obj_corners[3] = cvPoint(0, detection_obj.rows );

        /*///Toon obj corners
        circle(detection_obj,obj_corners[0], 5, Scalar(255,0,255),5);
        circle(detection_obj,obj_corners[1], 5, Scalar(0,0,255),5);
        circle(detection_obj,obj_corners[2], 5, Scalar(255,0,0),5);
        circle(detection_obj,obj_corners[3], 5, Scalar(255,255,0),5);
        imshow( "detection_obj", detection_obj );
        waitKey(0);
        */

        ///Gebruik de transformatie matrix om de 4 hoekpunten te vinden van het object in de scene.
        vector<Point2f> scene_corners(4);
        perspectiveTransform( obj_corners, scene_corners, H);

        /*///Toon scene corners
        circle(detection_img,scene_corners[0], 5, Scalar(255,0,255),5);
        circle(detection_img,scene_corners[1], 5, Scalar(0,0,255),5);
        circle(detection_img,scene_corners[2], 5, Scalar(255,0,0),5);
        circle(detection_img,scene_corners[3], 5, Scalar(255,255,0),5);
        */

        ///Draw lines between the corners (the mapped object in the scene - image_2 )
        line( detection_img, scene_corners[0] + Point2f( detection_obj.cols, 0), scene_corners[1] + cv::Point2f( detection_obj.cols, 0), cv::Scalar(0, 255, 0), 4 );
        line( detection_img, scene_corners[1] + Point2f( detection_obj.cols, 0), scene_corners[2] + cv::Point2f( detection_obj.cols, 0), cv::Scalar(0, 255, 255), 4 );
        line( detection_img, scene_corners[2] + Point2f( detection_obj.cols, 0), scene_corners[3] + cv::Point2f( detection_obj.cols, 0), cv::Scalar(255, 0, 0), 4 );
        line( detection_img, scene_corners[3] + Point2f( detection_obj.cols, 0), scene_corners[0] + cv::Point2f( detection_obj.cols, 0), cv::Scalar(0, 0, 255), 4 );

        ///Show object detection
        imshow( "Object detection", detection_img);
        waitKey(0);

    }

    return 0;
}
