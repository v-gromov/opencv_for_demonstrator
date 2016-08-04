#ifndef RECONGNIZE_FACE_FUNCTIONS_H
#define RECONGNIZE_FACE_FUNCTIONS_H

#include "opencv2/opencv.hpp"

#include <fstream>
#include <sstream>

#include <string.h>
#include <vector>

using namespace cv;
using namespace std;


class face_recog    //Включает в себя модели для распознавания лиц и метод, который выдает имя пользователя
{
public:
    void train_model(int, string);  //1 - Eigenfaces, 2 - Fisherfaces, 3 - Local Binary Patterns Histograms
    int get_number_people(Mat);

    void set_threshold(int val){recognition_threshold = val;}
    int get_threshold(){return recognition_threshold;}

private:
    void read_csv(const string&, vector<Mat>&, vector <int>&);
    int recognition_threshold=800;
};

Mat img_for_database(Mat);

#endif // RECONGNIZE_FACE_FUNCTIONS_H
