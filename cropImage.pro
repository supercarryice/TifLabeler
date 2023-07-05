#-------------------------------------------------
#
# Project created by QtCreator 2023-07-03T12:39:38
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cropImage
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += _USE_MATH_DEFINES
#DEFINES += ENABLE_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        custominputdialog.cpp \
        main.cpp \
        mainwindow.cpp \
        mapcanvas.cpp

HEADERS += \
        custominputdialog.h \
        mainwindow.h \
        mapcanvas.h \
        zipfile.h

#FORMS += \
#        mainwindow.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
LIBS += -L"E:\qgisnew\OSGeo4W64\apps\qgis-ltr\lib" -lqgis_core -lqgis_gui -lqgis_app

INCLUDEPATH += "E:\qgisnew\OSGeo4W64\apps\qgis-ltr\include"
INCLUDEPATH += "E:\qgisnew\OSGeo4W64\include"
INCLUDEPATH += "E:\qgisnew\OSGeo4W64\apps\Qt5\include\QtGui\5.12.3"
INCLUDEPATH += "E:\qgisnew\OSGeo4W64\apps\Qt5\include\QtCore\5.12.3"


LIBS += -L"E:\qgisnew\OSGeo4W64\lib"
LIBS += -L"E:\qgisnew\OSGeo4W64\apps\gdal-dev\lib"
LIBS += -L"E:\qgisnew\OSGeo4W64\apps\qgis-ltr\lib"
LIBS += -L"E:\qgisnew\OSGeo4W64\apps\Qt5\lib"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../qgisnew/OSGeo4W64/apps/gdal-dev/lib/ -lgdal_i
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../qgisnew/OSGeo4W64/apps/gdal-dev/lib/ -lgdal_i

INCLUDEPATH += $$PWD/../../qgisnew/OSGeo4W64/apps/gdal-dev/include
DEPENDPATH += $$PWD/../../qgisnew/OSGeo4W64/apps/gdal-dev/include
