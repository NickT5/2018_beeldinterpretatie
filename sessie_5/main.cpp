#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace cv::ml;///nodig voor machine learning classifiers

#define H 0
#define S 1
#define V 2

bool voorgrond=1;
vector<Point> punten_voorgrond;
vector<Point> punten_achtergrond;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
     if( event == EVENT_LBUTTONDOWN )
     {
          if(voorgrond)
          {
                punten_voorgrond.push_back(Point(x,y));
                cout << "Voorgrond position (" << x << ", " << y << ")" << endl;

          }
          else
          {
                punten_achtergrond.push_back(Point(x,y));
                cout << "Achtergrond position (" << x << ", " << y << ")" << endl;
          }

     }
     ///Todo: met rechtsklik een 'slecht' punt uit de vector poppen.
}

int main(int argc, const char **argv)
{
    cout << "Sessie 5 - Machine learning (kNN, Normal Bayes, SVM)" << endl;

    CommandLineParser parser(argc, argv,
        "{ help h |  | show this message         }"
        "{ image0 i   |  | (required) image path to strawberry1.tif }"
        "{ image1 j   |  | (required) image path to strawberry2.tif }"
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

    ///Define window titles
    string windowTitles[] = {"strawberry1.tif", "strawberry2.tif"};

    ///Read, empty check, (resize and show) the images
    for(int i=0;i<aantImages; i++)
    {
        inputImages.push_back(imread(imgNames[i]));                                 ///Read
        if(inputImages[i].empty()){ cerr << "Empty image!" << endl;  return -1;}    ///Empty check
        //resize(inputImages[i],inputImages[i],Size(), 0.75, 0.75);                 ///Resize
        //imshow(windowTitles[i], inputImages[i]);                                  ///Show

        ///Het niet-rode in de aardbeien onderdrukken door te blurren.
        GaussianBlur(inputImages[i], inputImages[i], Size(5,5),0);
    }


    ///Clone input image
    Mat img = inputImages[0].clone();

    ///Show image
    imshow(windowTitles[0], img);

    ///set the callback function for any mouse event
    setMouseCallback(windowTitles[0], CallBackFunc, NULL);


    cout << "Geef alle voorgrond punten. Klik op ENTER wanneer je klaar bent." << endl;
    cout << "----------------------------------------------------------------" << endl;
    waitKey(0);

    ///Verander van voorgrond mode naar achtergrond mode.
    voorgrond = false;
    cout << "Geef alle achtergrond punten. Klik op ENTER wanneer je klaar bent." << endl;
    cout << "------------------------------------------------------------------" << endl;
    waitKey(0);

    ///Converteer naar HSV space.
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    ///Trainingsdata voor de voorgrond bouwen.
    Mat trainingDataVoorgrond(punten_voorgrond.size(),3,CV_32FC1);
    Mat labelsVoorgrond = Mat::ones(punten_voorgrond.size(),1,CV_32SC1);
    ///Voor elk geklikte voorgrond punt de HSV waarde ophalen vd afbeelding.
    for(unsigned int i=0; i<punten_voorgrond.size(); i++)
    {
        Vec3b descriptor = hsv.at<Vec3b>(punten_voorgrond[i].y, punten_voorgrond[i].x);
        trainingDataVoorgrond.at<float>(i,H) = descriptor[H];
        trainingDataVoorgrond.at<float>(i,S) = descriptor[S];
        trainingDataVoorgrond.at<float>(i,V) = descriptor[V];
    }

    ///Training Data voor de achtergrond opbouwen.
    Mat trainingDataAchtergrond(punten_achtergrond.size(), 3, CV_32FC1);
    Mat labelsAchtergrond = Mat::zeros(punten_achtergrond.size(), 1, CV_32SC1);
    ///Voor elk geklikte achtergrond punt de HSV waarde ophalen vd afbeelding.
    for(unsigned int i=0; i<punten_achtergrond.size(); i++)
    {
        Vec3b descriptor = hsv.at<Vec3b>(punten_achtergrond[i].y, punten_achtergrond[i].x);
        trainingDataAchtergrond.at<float>(i,H) = descriptor[H];
        trainingDataAchtergrond.at<float>(i,S) = descriptor[S];
        trainingDataAchtergrond.at<float>(i,V) = descriptor[V];
    }

    ///Groepeer voorgrond en achtergrond samen in 'trainingData'.
    Mat trainingData, labels;
    vconcat(trainingDataVoorgrond, trainingDataAchtergrond, trainingData);  //vertical concatination
    vconcat(labelsVoorgrond, labelsAchtergrond, labels);                    //vertical concatination

    ///Train de classifiers (kNN, Normal Bayes, SVM):
    cout << "Train a kNN Classifier ..." << endl;
    Ptr<KNearest> kNN = KNearest::create();
    Ptr<TrainData> trainingdataKNN = TrainData::create(trainingData, ROW_SAMPLE, labels);
    kNN->setIsClassifier(true);
    kNN->setAlgorithmType(KNearest::BRUTE_FORCE);
    kNN->setDefaultK(3);
    kNN->train(trainingdataKNN);

    cout << "Train a Normal Bayer Classifier ..." << endl;
    Ptr<NormalBayesClassifier> normalBayes = NormalBayesClassifier::create();
    normalBayes->train(trainingData, ROW_SAMPLE, labels);

    cout << "Train a Support Vector Machine Classifier ..." << endl;
    Ptr<SVM> svm = SVM::create();
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
    svm->train(trainingData, ROW_SAMPLE, labels);

    ///Creeër maskers voor elke CLF.
    Mat mask_kNN            = Mat::zeros(img.rows,img.cols,CV_8UC1);
    Mat mask_normalBayes    = Mat::zeros(img.rows,img.cols,CV_8UC1);
    Mat mask_SVM            = Mat::zeros(img.rows,img.cols,CV_8UC1);

    ///Variabele definiëren voor de pixel prediction voor elke CLF.
    Mat prediction_kNN, prediction_normalBayes, prediction_SVM;

    ///Itereer over elke pixel van de image.
    for(int i=0;i<img.rows;i++){
        for(int j=0;j<img.cols;j++){
            ///Haal HSV pixelwaarden op van de huidige pixel.
            Vec3b pixelvalue = hsv.at<Vec3b>(i,j);
            Mat data_test(1,3,CV_32FC1);
            data_test.at<float>(0,H) = pixelvalue[H];
            data_test.at<float>(0,S) = pixelvalue[S];
            data_test.at<float>(0,V) = pixelvalue[V];

            ///Predict pixel
            kNN->findNearest(data_test, kNN->getDefaultK(), prediction_kNN);
            normalBayes->predict(data_test, prediction_normalBayes);    //juiste predict?
            svm->predict(data_test, prediction_SVM);

            ///Maskers invullen op basis van de pixel prediction.
            mask_kNN.at<uchar>(i,j)         = prediction_kNN.at<float>(0,0);
            mask_normalBayes.at<uchar>(i,j) = prediction_normalBayes.at<float>(0,0);
            mask_SVM.at<uchar>(i,j)         = prediction_SVM.at<float>(0,0);
        }
    }

    ///Maskers weergeven
    imshow("Segmentation KNearest", mask_kNN*255);
    imshow("Segmentation Normal Bayes", mask_normalBayes*255);
    imshow("Segmentation Support Vector Machine", mask_SVM*255);

    ///Maskers toepassen op image.
    Mat result_kNN, result_normalBayes, result_SVM;
    bitwise_and(img, img, result_kNN, mask_kNN);
    bitwise_and(img, img, result_normalBayes, mask_normalBayes);
    bitwise_and(img, img, result_SVM, mask_SVM);

    imshow("Result with KNearest", result_kNN);
    imshow("Result with Normal Bayes", result_normalBayes);
    imshow("Result with Support Vector Machine", result_SVM);
    waitKey(0);


///TODO: resultaat verbeteren door het groene kanaal af te trekken.

    return 0;
}
