#include "main_funtions.h"
#include <QDebug>

int main_function()
{
    QString folder = "/home/vgromov/Projects/build-Face_recongnize_for_demonstrator-Desktop-Debug/";
    QList <QImage> crop_images_list;
    CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
    assert(capture);
    if(capture == NULL)
    {
        cout<<"Camera capture is bussy or not found. Exit"<<endl;
        return -1;
    }
    IplImage* frame;
    for(int counter = 0; counter < 100; counter++)//Создадим и обработаем 10 снимков. Этап нахождения и кадрирования лица
    {
        cout<<"Create photo ["<<counter<<"/100]"<<endl;
        frame = cvQueryFrame(capture);

        QImage full_image = convert_lpl_qimg(frame);
        Mat mat_frame = cvarrToMat(frame);

        QVector <face> faces = detect_face_and_eyes(mat_frame, 1);

        for(int i = 0; i < faces.size(); i++)
        {
            if(faces[i].number_eyes()>=2)
            {
                cout<<"One face is found!"<<endl;
                QImage faceImg = CropFace(full_image, faces[i].get_coord_eyes().at(0).x,faces[i].get_coord_eyes().at(0).y, faces[i].get_coord_eyes().at(1).x, faces[i].get_coord_eyes().at(1).y, 0.3, 0.3, 200, 200);
                crop_images_list.push_back(faceImg);
                i = faces.size();
            }
        }
    }
    if(crop_images_list.size()<5)
    {
        cout<<"Faces not found in image ("<<crop_images_list.size()<<"|100) find. Need not more 5. Exit"<<endl;
        return -2;
    }
    cout<<"System find ("<<crop_images_list.size()<<"|100) faces! Now run the process recognize face..."<<endl;

    if (!QFile::exists(folder+"database.csv") )
    {
        cout<<"Database not found. Create new database!";
        create_database(folder, crop_images_list);
    }
    else
    {

        //Распознаем лица на снимке
        face_recog face_recognation_obj;    //тренируем модель
        cout<<whats_numb_people(folder);
        if(whats_numb_people(folder)>1)
            face_recognation_obj.train_model(2, (folder+"database.csv").toStdString());
        else
            face_recognation_obj.train_model(1, (folder+"database.csv").toStdString());
        QList <int> numb_ident;
        QList <int> numb_ident2;
        for(int i = 0; i < crop_images_list.size(); i++)
        {
            QImage tmp = crop_images_list.at(i);
            Mat mat_frame = qimage_to_mat(tmp);
            int number_people = face_recognation_obj.get_number_people(mat_frame);
            if(number_people!=-1)
                numb_ident.push_back(number_people);
        }
        int error_flag = 0;
        int number_people = 0;
        int all_numb_photo = numb_ident.size();
        cout<<numb_ident.size()<<endl;
        if(numb_ident.size()==0)
        {
            cout<<"face not found. Need create proffile"<<endl;
            create_database(folder, crop_images_list);
            return 0;
        }
        else
        {
            numb_ident2 = numb_ident;
            int number_people2;
            if(!numb_ident.empty())
            {
                number_people = numb_ident.takeFirst();
                number_people2 = number_people;
            }
            while(!numb_ident.empty())
            {
                number_people = numb_ident.takeFirst();
                if(number_people!= number_people2)
                {
                    number_people2 = number_people;
                    error_flag++;
                }
            }
        }
        if(error_flag>0)
        {
            cout<<"errors:"<<error_flag<<endl;
            cout<<"The system not recognize face. Please restart system."<<endl;
            return -3;
        }
        else//Будем считать, что человек найден
        {
            cout<<"Face errors:"<<error_flag<<endl;
            QList <int> listValue;
            QList <int> counterValue;
            listValue.push_front(numb_ident2.takeFirst());
            counterValue.push_front(1);

            while(numb_ident2.size()>0)
            {
                int val = numb_ident2.takeFirst();

                int size_counterValue = counterValue.size();
                for(int i = 0; i < size_counterValue; i++)
                {
                    if(val == listValue[i])
                    {
                        counterValue[i]++;
                    }
                    else
                    {
                        listValue.push_front(val);
                        counterValue.push_front(1);
                    }
                }
            }
            int counter = 0;
            int real_name_person = -1;

            int temp_count = 0;
            int tempVal = 0;
            while(counterValue.size()!=0)
            {
                temp_count = counterValue.takeFirst();
                tempVal = listValue.takeFirst();
                if(temp_count>counter)
                {
                    real_name_person = tempVal;
                }
            }
            if(real_name_person<0)
                cout<<"Error in program"<<endl;
            else
                cout<<"People was found. The number of people = "<<real_name_person<<endl;
        }
    }
    return 0;
}


void create_database(QString name_base, QList <QImage> images)
{
    QFile fileWithName(name_base + "database.csv");
    fileWithName.open(QIODevice::ReadOnly | QIODevice::Text);

    int end_numb = 0;
    while(!fileWithName.atEnd())
    {
        QString str;
        str = fileWithName.readLine();
        QStringList list1 = str.split(';');
        QString value = list1.at(1);
        if(value.toInt()>end_numb)
            end_numb = value.toInt();
    }
    end_numb++;
    QString tmp;
    tmp.setNum(end_numb);
    QString folder_for_people = "user_folder" + tmp;
    fileWithName.close();
    fileWithName.setFileName(name_base + "database.csv");
    if (!fileWithName.open(QIODevice::Append | QIODevice::Text))
    {
        cout<<"Create database: can't open file with database: ";
        cout<<(name_base + "database.csv").toStdString();
    }
    else
    {
        QDir().mkdir(folder_for_people);
        int numb_photo = 0;
        while(images.size()!=0)
        {
            QImage tmp_img = images.takeFirst();

            Mat fordb = img_for_database(qimage_to_mat(tmp_img));
            cvtColor(fordb, fordb,  CV_RGB2BGR);//Выставляем нужный колор

            QImage save_crop_image = Mat2QImage(fordb);
            save_crop_image = save_crop_image.scaled(save_crop_image.width(),save_crop_image.height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            QString tmp2;
            tmp2.setNum(numb_photo);
            QString path_to_file = name_base+folder_for_people+"/image_crop"+tmp2+".jpg";
            //сохраним файл
            QFile file(path_to_file);
            file.open(QIODevice::WriteOnly);
            save_crop_image.save(&file, "jpg",100);
            file.close();

            //добавим сведения с CSV файл
            QFile fileCSV(name_base + "database.csv");
            fileCSV.open(QIODevice::Append | QIODevice::Text);
            QTextStream writeStream(&fileCSV); // Создаем объект класса QTextStream
            writeStream <<path_to_file + ";"+tmp+"\n"; // Посылаем строку в поток для записи
            fileCSV.close(); // Закрываем файл
            numb_photo++;
        }
    }
    fileWithName.close();
}

int whats_numb_people(QString name_base)
{
    QFile fileWithName(name_base + "database.csv");
    fileWithName.open(QIODevice::ReadOnly | QIODevice::Text);
    int whats_numb =0;
    int old_value = 0;
    while(!fileWithName.atEnd())
    {
        QString str;
        str = fileWithName.readLine();
        QStringList list1 = str.split(';');
        QString value = list1.at(1);
        int now_value = value.toInt();

        if(now_value!=old_value)
        {
            old_value = now_value;
            whats_numb++;
        }
    }
    fileWithName.close();
    return whats_numb;
}
