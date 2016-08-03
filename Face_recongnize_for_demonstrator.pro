QT += core
#QT -= gui


QT += gui


QT += widgets
CONFIG += c++11

INSTALLS += target


TARGET = Face_recongnize_for_demonstrator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    face_detect_functions.cpp \
    crop_images_functions.cpp \
    face_recongnize_functions.cpp \
    main_funtions.cpp

INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_core -lopencv_face -lopencv_imgproc -lopencv_objdetect -lopencv_videoio

HEADERS += \
    face_detect_functions.h \
    crop_images_functions.h \
    face_recongnize_functions.h \
    main_funtions.h
