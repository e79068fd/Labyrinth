#ifndef OGLWindow_H
#define OGLWindow_H

#include "geometryengine.h"
#include "box.h"

#include <QWindow>
#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QWidget>
#include <QTouchEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QVector>
#include <qmath.h>
#include <QTime>

class GeometryEngine;

namespace Ui {
class OGLWindow;
}

class OGLWindow : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OGLWindow();
    ~OGLWindow();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void touchEvent(QTouchEvent* event);
    void timerEvent(QTimerEvent *e);

    void initShaders();
    void initTextures();

private:
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    GeometryEngine *geometries;

    QOpenGLTexture *texture;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;

    QVector<Box> boxs;
    QVector<QVector<QColor>> ignoreBoxs;
    GLubyte* centralPixel;
    QColor ignoreColorId;

    GLint rendTexture;

    int oldTouchCount;
    qreal oldAtan;

    int framesPerSecond;
    int lastTime;
    void fps();

signals:
    void changeWindow();
};

#endif // OGLWindow_H
