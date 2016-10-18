#ifndef OGLWindow_H
#define OGLWindow_H

#include "geometryengine.h"
#include "labyrinth.h"
#include <QAccelerometer>
#include <QAccelerometerReading>

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

namespace Ui {
class OGLWindow;
}

class OGLWindow : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit OGLWindow();
    ~OGLWindow();
    void startGame();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void touchEvent(QTouchEvent* event);
    void timerEvent(QTimerEvent *e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

    void initShaders();
    void initTextures();
    void initLabyrinth();

    void beginMoveEvent(const QPointF& first, const QPointF& last, int type);
    void updateMoveEvent(const QPointF& first, const QPointF& last, int type);

private:
    QBasicTimer timer;
    QOpenGLShaderProgram program;
    QOpenGLShaderProgram lightingProgram;

    QOpenGLTexture *texture;

    QMatrix4x4 projection;

    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;

    GLint rendTexture;

    int oldMoveType;
    qreal oldAtan;

    int framesPerSecond;
    int lastTimeForFPS;
    void fps();

    PlaneDrawObject* plane;
    BoxDrawObject* boxDraw;
    Labyrinth* labyrinth;

    int lastTimeForEnd;

    QAccelerometer accelerometer;

signals:
    void changeWindow();
    void endGame();
};

#endif // OGLWindow_H
