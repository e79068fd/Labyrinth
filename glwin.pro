#-------------------------------------------------
#
# Project created by QtCreator 2016-09-08T20:17:47
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = glwin
TEMPLATE = app


SOURCES += main.cpp \
    oglwindow.cpp \
    mainwindow.cpp \
    geometryengine.cpp \
    box.cpp

HEADERS  += \
    oglwindow.h \
    mainwindow.h \
    geometryengine.h \
    box.h

FORMS    += \
    mainwindow.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    vshader.vsh \
    fshader.fsh \
    lightingshader.fsh \
    lightingshader.vsh

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

RESOURCES += \
    shaders.qrc \
    textures.qrc

