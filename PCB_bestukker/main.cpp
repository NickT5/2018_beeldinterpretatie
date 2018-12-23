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





    return 0;
}
