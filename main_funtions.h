#ifndef MAIN_FUNTIONS_H
#define MAIN_FUNTIONS_H

#include "crop_images_functions.h"
#include "face_detect_functions.h"
#include "face_recongnize_functions.h"
#include <QQueue>
#include <QTextStream>
#include <QFileDialog>

int main_function();
void create_database(QString, QList <QImage>);
int whats_numb_people(QString);
#endif // MAIN_FUNTIONS_H
