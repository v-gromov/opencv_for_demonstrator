#include "face_detect_functions.h"

cascades cascades_obj;


QVector <face> center_faces(QImage image_for_detect, float scale)
{
    QVector <face> null_face;
    int number_of_true_face = -1;

    Mat mat_image_for_detect = qimage_to_mat(image_for_detect);
    if(mat_image_for_detect.empty())
    {
        cout << "Could not read image" << endl;
        return null_face;
    }
    QVector <face> find_faces = detect_face_and_eyes(mat_image_for_detect, scale);

    for(int i = 0; i < find_faces.size(); i++)//Фильтр смотрит тупо - есть ли хотя бы два глаза на картинке
    {
        if(find_faces[i].number_eyes()>1)
        {
            number_of_true_face = i;
        }
    }
    if(number_of_true_face!=-1)
        return find_faces;
    return null_face;
}


QVector <face> detect_face_and_eyes(Mat full_image_with_faces, double scale) //Возвращает координаты лица
{
    vector<Rect> result_recrangle_faces;
    QVector <face> find_faces;
    Mat small_img_with_faces;

    resize(full_image_with_faces, small_img_with_faces, Size(), 1./scale, 1./scale, INTER_LINEAR );
    cvtColor(small_img_with_faces, small_img_with_faces, COLOR_BGR2GRAY);        //cvtColor(&img, gray, COLOR_BGR2GRAY);

    equalizeHist(small_img_with_faces, small_img_with_faces);

    //Поиск лица на изображении
    cascades_obj.cascade.detectMultiScale(small_img_with_faces, result_recrangle_faces,
                                          1.1, 2, 0
                                          |CASCADE_SCALE_IMAGE,
                                          Size(30, 30) );

    for (size_t i = 0; i < result_recrangle_faces.size(); i++ ) //Обработка лиц
    {
        Rect rect_face = result_recrangle_faces[i];
        Mat small_image_with_face;
        vector<Rect> nestedObjects;
        Point center;
        face face_for_write;


        double aspect_ratio = (double)rect_face.width/rect_face.height; //Соотношение сторон лица
        if((0.75 < aspect_ratio && aspect_ratio < 1.3)&&(rect_face.width>20))
        {
            center.x = cvRound((rect_face.x + rect_face.width*0.5)*scale);
            center.y = cvRound((rect_face.y + rect_face.height*0.5)*scale);
        }

        //поиск глаз на лице
        small_image_with_face = small_img_with_faces(rect_face); //Кадрируем лицо
        cascades_obj.nestedCascade.detectMultiScale(small_image_with_face, nestedObjects,
                                                    1.1,2,
                                                    0|CASCADE_SCALE_IMAGE,
                                                    Size(30, 30) );

        face_for_write.set_coord_face(center);
        QVector <Point> write_eyes_array;

        for ( size_t j = 0; j < nestedObjects.size(); j++ )
        {
            Rect nr = nestedObjects[j];
            center.x = cvRound((rect_face.x + nr.x + nr.width*0.5)*scale);
            center.y = cvRound((rect_face.y + nr.y + nr.height*0.5)*scale);

            int radius_eye = cvRound((nr.width + nr.height)*0.25*scale);

            if((radius_eye>=20)&&((center.x>10)&&(center.x<full_image_with_faces.size().width-10))&&((center.y>10)&&(center.x<full_image_with_faces.size().height-10)))
            {
                write_eyes_array.push_back(center);
            }
        }
        if(write_eyes_array.size()==2)
        {
            if(write_eyes_array.at(0).x<write_eyes_array.at(1).x)
            {
                face_for_write.set_coord_eyes(write_eyes_array);
                find_faces.push_back(face_for_write);
            }
        }
    }
    return find_faces;
}


cascades::cascades()
{
    string cascadeName = "/home/vgromov/tmp/opencv/opencv/data/haarcascades/haarcascade_frontalface_alt.xml";
    string nestedCascadeName = "/home/vgromov/tmp/opencv/opencv/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml";

    if ( !nestedCascade.load( nestedCascadeName ) )
        cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
    if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
    }
}

Mat qimage_to_mat(QImage& qimage) {
    cv::Mat tmp(qimage.height(),qimage.width(),CV_8UC3,(uchar*)qimage.bits(),qimage.bytesPerLine());
    cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
    cvtColor(tmp, result,CV_BGR2RGB);
    return result;
}

QImage convert_lpl_qimg(IplImage* frame)
{
    Mat img = cvarrToMat(frame);
    cv::cvtColor(img,img,CV_BGR2RGB); //Qt reads in RGB whereas CV in BGR
    QImage imdisplay((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    img.release();
    return imdisplay;
}


QImage Mat2QImage(const Mat &src) {
    cv::Mat temp;
    cvtColor(src, temp,CV_BGR2RGB);
    QImage dest((uchar*) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    QImage dest2(dest);
    dest2.detach();
    return dest2;
}
