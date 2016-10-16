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

    angularSpeed = 1;
    rotationAxis.setX(1);
    rotationAxis.setY(1);
    rotationAxis.setZ(1);

    //hardcode maze
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, -0.05, -1.0), QVector3D(1.0, 0.0, 1.0), QColor(0, 0, 255)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, 0.3, -1.0), QVector3D(1.0, 0.35, 1.0), QColor(255, 0, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());

    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, 0.0, -1.0), QVector3D(-0.95, 0.3, -0.95), QColor(240, 0, 240)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(0.95, 0.0, -1.0), QVector3D(1, 0.3, -0.95), QColor(225, 0, 225)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(0.95, 0.0, 0.95), QVector3D(1.0, 0.3, 1), QColor(210, 0, 210)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1, 0.0, 0.95), QVector3D(-0.95, 0.3, 1), QColor(195, 0, 195)));
    ignoreBoxs.back().push_back(boxs.back().getColor());

    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-0.95, 0.0, -1), QVector3D(0.95, 0.3, -0.95), QColor(0, 255, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[2].getColor());
    ignoreBoxs.back().push_back(boxs[3].getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(0.95, 0.0, -0.95), QVector3D(1, 0.3, 0.95), QColor(0, 200, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[3].getColor());
    ignoreBoxs.back().push_back(boxs[4].getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-0.95, 0.0, 0.95), QVector3D(0.95, 0.3, 1), QColor(0, 145, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[4].getColor());
    ignoreBoxs.back().push_back(boxs[5].getColor());
    ignoreBoxs.push_back(QVector<QColor>());
    boxs.push_back(Box(QVector3D(-1.0, 0.0, -0.95), QVector3D(-0.95, 0.3, 0.95), QColor(0, 90, 0)));
    ignoreBoxs.back().push_back(boxs.back().getColor());
    ignoreBoxs.back().push_back(boxs[5].getColor());
    ignoreBoxs.back().push_back(boxs[2].getColor());

    boxs.push_back(Box(QVector3D(-0.95, 0.0, -0.7), QVector3D(0.7, 0.3, -0.65), QColor(128, 128, 128)));
    boxs.push_back(Box(QVector3D(0.65, 0.0, -0.65), QVector3D(0.7, 0.3, 0.7), QColor(128, 128, 128)));
    boxs.push_back(Box(QVector3D(-0.35, 0.0, 0.65), QVector3D(0.65, 0.3, 0.7), QColor(128, 128, 128)));
    boxs.push_back(Box(QVector3D(-0.35, 0.0 + 1e-3, 0.65), QVector3D(-0.3, 0.3 - 1e-3, -0.3), QColor(128, 128, 128)));

    initLabyrinth();
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
    glEnable(GL_FRONT_AND_BACK);

    geometries = new GeometryEngine;

    QVector2D texcoords[] = {
        QVector2D(0, 0),
        QVector2D(0.33, 0),
        QVector2D(0, 0.5),
        QVector2D(0.33, 0.5)
    };
    plane = new PlaneDrawObject;
    plane->setTexture(texcoords);

    boxDraw = new BoxDrawObject;

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

    // Compile vertex shader
    if (!lightingProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/lightingshader.vsh"))
        close();

    // Compile fragment shader
    if (!lightingProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/lightingshader.fsh"))
        close();

    // Link shader pipeline
    if (!lightingProgram.link())
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

void OGLWindow::initLabyrinth() {
    labyrinth = new Labyrinth();

    labyrinth->addWall(QVector3D(40, 1, 40), QVector3D(-20, -1, -20));
    labyrinth->addWall(QVector3D(40, 1, 40), QVector3D(-20,  6, -20));

    labyrinth->addWall(QVector3D(1, 6, 1), QVector3D(-20, 0, -20));
    labyrinth->addWall(QVector3D(1, 6, 1), QVector3D( 19, 0, -20));
    labyrinth->addWall(QVector3D(1, 6, 1), QVector3D( 19, 0,  19));
    labyrinth->addWall(QVector3D(1, 6, 1), QVector3D(-20, 0,  19));

    labyrinth->addWall(QVector3D(38, 6,  1), QVector3D(-19, 0, -20));
    labyrinth->addWall(QVector3D( 1, 6, 38), QVector3D( 19, 0, -19));
    labyrinth->addWall(QVector3D(38, 6,  1), QVector3D(-19, 0,  19));
    labyrinth->addWall(QVector3D( 1, 6, 38), QVector3D(-20, 0, -19));

    labyrinth->addIgnore(0, labyrinth->getWallColor(0));
    labyrinth->addIgnore(1, labyrinth->getWallColor(1));

    labyrinth->addIgnore(2, labyrinth->getWallColor(2));
    labyrinth->addIgnore(3, labyrinth->getWallColor(3));
    labyrinth->addIgnore(4, labyrinth->getWallColor(4));
    labyrinth->addIgnore(5, labyrinth->getWallColor(5));

    labyrinth->addIgnore(6, labyrinth->getWallColor(6));
    labyrinth->addIgnore(6, labyrinth->getWallColor(2));
    labyrinth->addIgnore(6, labyrinth->getWallColor(3));

    labyrinth->addIgnore(7, labyrinth->getWallColor(7));
    labyrinth->addIgnore(7, labyrinth->getWallColor(3));
    labyrinth->addIgnore(7, labyrinth->getWallColor(4));

    labyrinth->addIgnore(8, labyrinth->getWallColor(8));
    labyrinth->addIgnore(8, labyrinth->getWallColor(4));
    labyrinth->addIgnore(8, labyrinth->getWallColor(5));

    labyrinth->addIgnore(9, labyrinth->getWallColor(9));
    labyrinth->addIgnore(9, labyrinth->getWallColor(5));
    labyrinth->addIgnore(9, labyrinth->getWallColor(2));


    labyrinth->addWall(QVector3D(33, 6, 1), QVector3D(-19, 0, -14));
    labyrinth->addWall(QVector3D(1, 6, 27), QVector3D(14, 0, -14));
    labyrinth->addWall(QVector3D(20, 6, 1), QVector3D(-6, 0, 12));
    labyrinth->addWall(QVector3D(1, 6, 19), QVector3D(-6, 0, -7));

    labyrinth->addBall(QVector3D(0, 3, 0));
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
    //program.setUniformValue("rendTexture", rendTexture);
    texture->bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;

    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);
    matrix.scale(0.05);

    //---Search nearest box--
    program.bind();
    program.setUniformValue("rendTexture", 0);
    for(int i = 0; i < 10; i++) {
        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getWallMatrix(i));
        program.setUniformValue("color", labyrinth->getWallColor(i));

        // Draw cube geometry
        boxDraw->draw(&program);
    }

    glReadPixels(geometry().width() / 2, geometry().height() / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, centralPixel);
    ignoreColorId.setRgb(centralPixel[0], centralPixel[1], centralPixel[2], centralPixel[3]);
    program.setUniformValue("rendTexture", rendTexture);
    //--- ---

    //return;
    //---render box---
    lightingProgram.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //lightingProgram.setUniformValue("color", QColor(50, 205, 50));
    for(int i = 0; i < labyrinth->getWallCount(); i++) {
        if(!labyrinth->isIgnore(i, ignoreColorId)) {
            // Set modelview matrix
            lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getWallMatrix(i));
            // Set modelview-projection matrix
            lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getWallMatrix(i));

            lightingProgram.setUniformValue("color", boxs[i].getColor());

            // Draw cube geometry
            boxDraw->draw(&lightingProgram);\
        }
    }

    // Set modelview matrix
    lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getBallMatrix());
    // Set modelview-projection matrix
    lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getBallMatrix());
    lightingProgram.setUniformValue("color", QColor(0, 0, 0));
    // Draw cube geometry
    boxDraw->draw(&lightingProgram);\

    //--- ---
    //program.setUniformValue("rendTexture", 1);
    //QMatrix4x4 matrix2;
    //matrix2.translate(-0.5, 0.5, 0);
    //matrix2.scale(0.5);
    //program.setUniformValue("mvp_matrix", matrix2);
    //plane->draw(&program);
}

void OGLWindow::timerEvent(QTimerEvent *)
{
    // Decrease angular speed (friction)
    //angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;       
    }

    QMatrix4x4 matrix;
    matrix.rotate(rotation);
    labyrinth->setGravity((QVector3D(0, -10, 0) * matrix).normalized() * 10);
    //Step simulation
    labyrinth->step();
    // Request an update
    update();
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
