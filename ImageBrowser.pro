#-------------------------------------------------
#
# Project created by QtCreator 2014-10-01T11:58:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageBrowser
TEMPLATE = app
CONFIG += c++14


SOURCES += main.cpp\
        mainwindow.cpp \
    imagepainter.cpp \
    imageloader.cpp \
    asyncpixmaploader.cpp \
    iconloader.cpp \
    filelistwidgetitem.cpp \
    pixmaploadedobserver.cpp \
    canvas.cpp \
    drawable.cpp \
    ratingsystem.cpp \
    ratingpainter.cpp \
    imageloadingqueue.cpp \
    signalblocker.cpp

HEADERS  += mainwindow.h \
    imagepainter.h \
    imageloader.h \
    iteratorhelper.h \
    asyncpixmaploader.h \
    loadingqueue.h \
    iconloader.h \
    filelistwidgetitem.h \
    pixmaploadedobserver.h \
    canvas.h \
    drawable.h \
    eventdependanti.h \
    ratingsystem.h \
    ratingpainter.h \
    imageloadingqueue.h \
    signalblocker.h

FORMS    += mainwindow.ui

RESOURCES += \
    icons.qrc
