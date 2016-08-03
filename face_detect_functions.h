#ifndef FACE_DETECT_FUNCTIONS_H
#define FACE_DETECT_FUNCTIONS_H

#include "opencv2/objdetect.hpp"
#include "opencv2/opencv.hpp"

#include <iostream>
#include <QVector>
#include <QImage>


using namespace std;
using namespace cv;

class face
{
public:
    Point get_coord_face() { return coord_face;}
    QVector<Point> get_coord_eyes() {return coord_eyes;}
    void set_coord_face(Point set){coord_face = set;}
    void set_coord_eyes(QVector<Point> set){coord_eyes = set;}
    int number_eyes(){return coord_eyes.size();}
private:
    Point coord_face;
    QVector <Point> coord_eyes;
};


class cascades
{
public:
    cascades();
    CascadeClassifier cascade, nestedCascade;
};

QVector <face> center_faces(QImage, float);
QVector <face> detect_face_and_eyes(Mat img, double scale);

Mat qimage_to_mat(QImage&);
QImage convert_lpl_qimg(IplImage*); //for mainFunction
QImage Mat2QImage(cv::Mat const& src); //for mainFunction

#endif // FACE_DETECT_FUNCTIONS_H
