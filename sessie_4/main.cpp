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
        resize(inputImages[i],inputImages[i],Size(), 0.75, 0.75);                                               ///Resize
        string windowTitles[] = {"fitness image", "fitness object", "kinderbueno image", "kinderbueno object"}; ///Window title
        imshow(windowTitles[i], inputImages[i]);                                                                ///Show
    }
    waitKey(0);

    return 0;
}
