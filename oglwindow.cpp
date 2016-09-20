#include "oglwindow.h"

OGLWindow::OGLWindow() :
    geometries(0),
    texture(0),
    angularSpeed(0)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    setFormat(format);

    rendTexture = 0;
    centralPixel = new GLubyte[4]; //RGBA

    framesPerSecond = 0;
    lastTime = 0;

    //hardcode maze
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, -0.05, -1.0), QVector3D(1.0, 0.0, 1.0), QColor(0, 0, 255)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, 0.3, -1.0), QVector3D(1.0, 0.35, 1.0), QColor(255, 0, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());

    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, 0.01, -1.0), QVector3D(-0.95, 0.29, -0.95), QColor(240, 0, 240)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(0.95, 0.01, -1.0), QVector3D(1, 0.29, -0.95), QColor(225, 0, 225)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(0.95, 0.01, 0.95), QVector3D(1.0, 0.29, 1), QColor(210, 0, 210)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1, 0.01, 0.95), QVector3D(-0.95, 0.29, 1), QColor(195, 0, 195)));
    ignoreBoxs.back().push_back(boxs.back().getColor());

    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-0.95, 0.01, -1), QVector3D(0.95, 0.29, -0.95), QColor(0, 255, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[2].getColor());
    ignoreBoxs.back().push_back(boxs[3].getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(0.95, 0.01, -0.95), QVector3D(1, 0.29, 0.95), QColor(0, 200, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[3].getColor());
    ignoreBoxs.back().push_back(boxs[4].getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-0.95, 0.01, 0.95), QVector3D(0.95, 0.29, 1), QColor(0, 145, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[4].getColor());
    ignoreBoxs.back().push_back(boxs[5].getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, 0.01, -0.95), QVector3D(-0.95, 0.29, 0.95), QColor(0, 90, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[5].getColor());
    ignoreBoxs.back().push_back(boxs[2].getColor());

    boxs.push_back(Box(QVector3D(-0.95, 0.01, -0.7), QVector3D(0.7, 0.29, -0.65), QColor(128, 128, 128)));
    boxs.push_back(Box(QVector3D(0.65, 0.01, -0.65), QVector3D(0.7, 0.29, 0.7), QColor(128, 128, 128)));
    boxs.push_back(Box(QVector3D(-0.35, 0.01, 0.65), QVector3D(0.65, 0.29, 0.7), QColor(128, 128, 128)));
    boxs.push_back(Box(QVector3D(-0.35, 0.01, 0.65), QVector3D(-0.3, 0.29, -0.30), QColor(128, 128, 128)));
}

OGLWindow::~OGLWindow()
{
    makeCurrent();
    delete texture;
    delete geometries;
    delete []centralPixel;
    doneCurrent();
}


void OGLWindow::touchEvent(QTouchEvent *event) {
    if(event->type() == QTouchEvent::TouchEnd) {
        oldTouchCount = 0;
        return;
    }

    QList<QTouchEvent::TouchPoint> listTouch = event->touchPoints();

    int myType = (oldTouchCount == listTouch.size()) ? QTouchEvent::TouchUpdate : QTouchEvent::TouchBegin;
    oldTouchCount = listTouch.size();

    switch(myType) {
        case QTouchEvent::TouchBegin: {
            if(oldTouchCount == 1) {
                QTouchEvent::TouchPoint point = *listTouch.begin();
                if(point.pos().x() < 100 && point.pos().y() < 100) {
                    hide();
                    emit changeWindow();
                }
                mousePressPosition = QVector2D(point.pos());
            } else if (oldTouchCount == 2) {
                QPointF diff = listTouch.first().pos() - listTouch.last().pos();
                oldAtan = qAtan(diff.y() / diff.x());
            }
            break;
        }
        case QTouchEvent::TouchUpdate: {
            qreal x, y, z, length;
            if(listTouch.size() == 1) {
                QTouchEvent::TouchPoint point = *listTouch.begin();
                QVector2D diff = QVector2D(point.pos()) - mousePressPosition;
                mousePressPosition = QVector2D(point.pos());
                x = diff.y(); y = diff.x(); z = 0;
                length = diff.length() / 5.0;
            } else if (oldTouchCount == 2) {
                QPointF diff = listTouch.first().pos() - listTouch.last().pos();
                qreal atan = qAtan(diff.y() / diff.x());
                x = 0; y = 0; z = oldAtan - atan;
                if(qAbs(z) > 2) { // 2 is 114.592 Degrees in Radians
                    qreal sign = atan < 0 ? -1 : 1;
                    oldAtan = sign * (2 * 1.5708 - qAbs(oldAtan)); // 1.5708 is 90 Degrees in Radians
                    z = oldAtan - atan;
                }
                length = qAbs(z) * 100;
                oldAtan = atan;
            } else {
                break;
            }
            // Rotation axis is perpendicular to the mouse position difference
            // vector
            QVector3D n = QVector3D(x, y, z).normalized();

            // Accelerate angular speed relative to the length of the mouse sweep
            qreal acc = length;

            // Calculate new rotation axis as weighted sum
            rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

            // Increase angular speed
            angularSpeed = acc;
            break;
        }
        default: {
            break;
        }
    }
}

void OGLWindow::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(1.0f, 1.0f, 0.94f, 1);

    initShaders();
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Enable back face culling
    glEnable(GL_CULL_FACE);



    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    timer.start(1000/60, this);
}

void OGLWindow::initShaders() {
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.vsh"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.fsh"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void OGLWindow::initTextures() {
    // Load cube.png image
    texture = new QOpenGLTexture(QImage(":/cube.png").mirrored());

    // Set nearest filtering mode for texture minification
    texture->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    texture->setWrapMode(QOpenGLTexture::ClampToEdge);
}

void OGLWindow::resizeGL(int w, int h) {
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void OGLWindow::paintGL() {
    fps();

    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program.setUniformValue("rendTexture", rendTexture);
    texture->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;



    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);

    //---Search nearest box---
    program.setUniformValue("rendTexture", 0);
    for(int i = 0; i < 10; i++) {
        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * matrix * boxs[i].getMatrix());
        program.setUniformValue("color", boxs[i].getColor());

        // Draw cube geometry
        geometries->drawCubeGeometry(&program);
    }

    glReadPixels(geometry().width() / 2, geometry().height() / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, centralPixel);
    ignoreColorId.setRgb(centralPixel[0], centralPixel[1], centralPixel[2], centralPixel[3]);
    program.setUniformValue("rendTexture", rendTexture);
    //--- ---

    //---render box---
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(int i = 0; i < boxs.size(); i++) {
        if(i >= ignoreBoxs.size() || ignoreBoxs[i].indexOf(ignoreColorId) == -1) {
            // Set modelview-projection matrix
            program.setUniformValue("mvp_matrix", projection * matrix * boxs[i].getMatrix());
            program.setUniformValue("color", boxs[i].getColor());

            // Draw cube geometry
            geometries->drawCubeGeometry(&program);
        }
    }
    //--- ---
}

void OGLWindow::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

        // Request an update
        update();
    }
}

void OGLWindow::fps() {
    int currentTime = QTime::currentTime().msecsSinceStartOfDay();

    //Увеличиваем счетчик кадров
    framesPerSecond++;

    //Теперь вычтем из текущего времени последнее запомненное время. Если результат больше единицы,
    //это значит, что секунда прошла и нужно вывести новый FPS.
    if(currentTime - lastTime > 1000)
    {
        //Устанавливаем lastTime в текущее время. Теперь оно будет использоватся как предидущее время
        //для след. секунды.
        lastTime = currentTime;

        // Установим FPS для вывода:
        qDebug() << "FPS: " << framesPerSecond;

        //Сбросим FPS
        framesPerSecond = 0;
    }
}
