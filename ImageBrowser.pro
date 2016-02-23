#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T11:58:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageBrowser
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        mainwindow.cpp \
    imagepainter.cpp \
    imageloader.cpp \
    asyncpixmaploader.cpp \
    iconloader.cpp \
    filelistwidgetitem.cpp \
    pixmaploadedobserver.cpp

HEADERS  += mainwindow.h \
    imagepainter.h \
    imageloader.h \
    iteratorhelper.h \
    asyncpixmaploader.h \
    loadingqueue.h \
    iconloader.h \
    filelistwidgetitem.h \
    pixmaploadedobserver.h

FORMS    += mainwindow.ui
