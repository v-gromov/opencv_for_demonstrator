#include "face_recongnize_functions.h"
#include "opencv2/face.hpp"

using namespace cv;
using namespace cv::face;

Ptr <FaceRecognizer> model;

int face_recog::get_number_people(Mat Img)
{
    if((Img.size().width>10)&&(Img.size().height>10))
    {
        Mat img2 = img_for_database(Img);
        cvtColor(img2, img2,  CV_RGB2GRAY);//Выставляем нужный колор
        int predicted_label = -1;
        double predicted_confidence = 0.0;
        model->predict(img2, predicted_label, predicted_confidence);
        if(predicted_confidence< recognition_threshold)
            return predicted_label;
    }
    return -1;
}

void face_recog::train_model(int set_model, string fn_csv)
{
    vector<Mat> images;
    vector<int> labels;
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cout<< "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        return;
    }
    if(images.size() <= 1) {
        cout<< "Error in data. No image for work!";
    }
    switch(set_model)
    {
    case 1:
        model = createEigenFaceRecognizer();
        model->train(images, labels);
        break;
    case 2:
        model =  createFisherFaceRecognizer();
       model->train(images, labels);
        break;
    case 3:
        model = createLBPHFaceRecognizer();
        model->train(images, labels);
        break;
    default:
        cout<< "The method for detection face not selected!";
        break;
    }
}

Mat img_for_database(Mat Img)
{
    if((Img.size().width>10)&&(Img.size().height>10))
    {
        Size ImgSize = Img.size();
        Mat src = Img;//src image
        Mat dst;

        double sclareOX = 92./ImgSize.width;
        double sclareOY = 112./ImgSize.height;

        if(sclareOX<sclareOY)//значит больше ширина и мы уменьшаем до нужной ширины пропорционально
        {
            resize(src,dst,Size(0, 0), sclareOX, sclareOX, INTER_LINEAR);//resize image
        }
        else//значит больше высота и мы уменьшаем до нужной высоты пропорционально
        {
            resize(src,dst,Size(0, 0), sclareOY, sclareOY, INTER_LINEAR);//resize image
        }
        Mat dst2(112, 92, CV_8UC3);
        Vec3b col;
        col[0] = 0; col[1] = 0; col[2] = 0;
        int detOX = (112 - dst.size().height) / 2;
        int detOY = (92 - dst.size().width) / 2;
        for(int h = 0; h < 112; h++)
        {
            for(int w = 0; w < 92; w++)
                dst2.at<Vec3b>(Point(w,h)) = col;
        }
        for(int h = 0; h < dst.size().height; h++)
        {
            for(int w = 0; w<dst.size().width; w++)
            {
                Vec3b color = dst.at<Vec3b>(Point(w,h));
                dst2.at<Vec3b>(Point(w+detOY,h+detOX)) = color;
            }
        }
        return dst2;
    }
    else
    {
        cout<<"Incorrect image";
        return Mat(10, 10, DataType<int>::type);
    }
}

void face_recog::read_csv(const string& filename, vector<Mat>& images, vector<int>& labels)
{

    char separator = ';';
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        cout<<"No valid input file was given, please check the given filename.";
        return;
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}
