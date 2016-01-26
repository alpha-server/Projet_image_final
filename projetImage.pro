#-------------------------------------------------
#
# Project created by QtCreator 2016-01-11T15:35:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = projetImage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
   # traiterImage.cpp \
    formRedimensionnement.cpp \
traiterImage.cpp\


HEADERS  += mainwindow.h \
   # traiterImage.h \
    formRedimensionnement.h

FORMS    += mainwindow.ui

LIBS += -lopencv_core\
        -lopencv_imgproc\
        -lopencv_highgui\
        -lopencv_video\
        -lopencv_features2d\
        -lopencv_calib3d
        -lopencv_objdetect\
