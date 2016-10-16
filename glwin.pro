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
    box.cpp \
    ../lib/bullet/LinearMath/*.cpp\
    ../lib/bullet/BulletCollision/BroadphaseCollision/*.cpp\
    ../lib/bullet/BulletCollision/CollisionDispatch/*.cpp\
    ../lib/bullet/BulletCollision/CollisionShapes/*.cpp\
    ../lib/bullet/BulletCollision/Gimpact/*.cpp\
    ../lib/bullet/BulletCollision/NarrowPhaseCollision/*.cpp\
    ../lib/bullet/BulletDynamics/Character/*.cpp\
    ../lib/bullet/BulletDynamics/ConstraintSolver/*.cpp\
    ../lib/bullet/BulletDynamics/Dynamics/*.cpp\
    ../lib/bullet/BulletDynamics/Vehicle/*.cpp \
    labyrinth.cpp

HEADERS  += \
    oglwindow.h \
    mainwindow.h \
    geometryengine.h \
    box.h \
    ../lib/bullet/LinearMath/*.h\
    ../lib/bullet/BulletCollision/BroadphaseCollision/*.h\
    ../lib/bullet/BulletCollision/CollisionDispatch/*.h\
    ../lib/bullet/BulletCollision/CollisionShapes/*.h\
    ../lib/bullet/BulletCollision/Gimpact/*.h\
    ../lib/bullet/BulletCollision/NarrowPhaseCollision/*.h\
    ../lib/bullet/BulletDynamics/Character/*.h\
    ../lib/bullet/BulletDynamics/ConstraintSolver/*.h\
    ../lib/bullet/BulletDynamics/Dynamics/*.h\
    ../lib/bullet/BulletDynamics/Vehicle/*.h \
    labyrinth.h

INCLUDEPATH += ../lib/bullet

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

