#include "oglwindow.h"

OGLWindow::OGLWindow() :
    geometries(0),
    texture(0),
    angularSpeed(0),
    labyrinth(0)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    setFormat(format);

    rendTexture = 0;
    centralPixel = new GLubyte[4]; //RGBA

    framesPerSecond = 0;
    lastTimeForFPS = 0;

    colors.push_back(QColor(0, 0, 255));
    colors.push_back(QColor(255, 0, 0));
    colors.push_back(QColor(240, 0, 240));
    colors.push_back(QColor(225, 0, 225));
    colors.push_back(QColor(210, 0, 210));
    colors.push_back(QColor(195, 0, 195));
    colors.push_back(QColor(0, 255, 0));
    colors.push_back(QColor(0, 200, 0));
    colors.push_back(QColor(0, 145, 0));
    colors.push_back(QColor(0, 90, 0));
    colors.push_back(QColor(128, 128, 128));
    colors.push_back(QColor(128, 128, 128));
    colors.push_back(QColor(128, 128, 128));
    colors.push_back(QColor(128, 128, 128));
}

void OGLWindow::startGame() {
    initLabyrinth();

    lastTimeForEnd = 0;
    angularSpeed = 0;
    rotationAxis = QVector3D();
    rotation = QQuaternion();

    // Use QBasicTimer because its faster than QTimer
    timer.start(1000/60, this);
}

OGLWindow::~OGLWindow()
{
    makeCurrent();
    delete texture;
    delete geometries;
    delete []centralPixel;
    doneCurrent();
}

void OGLWindow::beginMoveEvent(const QPointF& first, const QPointF& last, int type) {
    switch(type) {
        case 1:
            if(first.x() < 100 && first.y() < 100) {
                hide();
                emit changeWindow();
            }
            mousePressPosition = QVector2D(first);
            break;
        case 2:
            QPointF diff = first - last;
            oldAtan = qAtan(diff.y() / diff.x());
            break;
    }
}

void OGLWindow::updateMoveEvent(const QPointF& first, const QPointF& last, int type) {
    qreal x, y, z, length;
    QVector2D diff;
    switch(type) {
        case 1:
            diff = QVector2D(first) - mousePressPosition;
            mousePressPosition = QVector2D(first);
            x = diff.y(); y = diff.x(); z = 0;
            length = diff.length() / 5.0;
            break;
        case 2:
            diff = QVector2D(first) - QVector2D(last);
            qreal atan_ = qAtan(diff.y() / diff.x());
            x = 0; y = 0; z = oldAtan - atan_;
            if(qAbs(z) > 2) { // 2 is 114.592 Degrees in Radians
                qreal sign = atan_ < 0 ? -1 : 1;
                oldAtan = sign * (2 * 1.5708 - qAbs(oldAtan)); // 1.5708 is 90 Degrees in Radians
                z = oldAtan - atan_;
            }
            length = qAbs(z) * 100;
            oldAtan = atan_;
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
}

void OGLWindow::touchEvent(QTouchEvent *event) {
    if(event->type() == QTouchEvent::TouchEnd) {
        oldMoveType = 0;
        return;
    }

    QList<QTouchEvent::TouchPoint> listTouch = event->touchPoints();

    int myType = (oldMoveType == listTouch.size()) ? QTouchEvent::TouchUpdate : QTouchEvent::TouchBegin;
    oldMoveType = listTouch.size();

    switch(myType) {
        case QTouchEvent::TouchBegin:
            beginMoveEvent(listTouch.first().pos(), listTouch.last().pos(), oldMoveType);
            break;
        case QTouchEvent::TouchUpdate:
            updateMoveEvent(listTouch.first().pos(), listTouch.last().pos(), oldMoveType);
            break;
        default:
            break;
    }
}

void OGLWindow::mousePressEvent(QMouseEvent *e) {
    //qDebug() << e->localPos() << e->button();
    if(e->buttons() & Qt::LeftButton)
        beginMoveEvent(e->localPos(), QPointF(), 1);
    if(e->buttons() & Qt::RightButton)
        beginMoveEvent(QPointF(geometry().width() / 2., geometry().height() / 2.), e->localPos(), 2);
}

void OGLWindow::mouseMoveEvent(QMouseEvent *e) {
    if(e->buttons() & Qt::LeftButton)
        updateMoveEvent(e->localPos(), QPointF(), 1);
    if(e->buttons() & Qt::RightButton)
        updateMoveEvent(QPointF(geometry().width() / 2., geometry().height() / 2.), e->localPos(), 2);
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
    //glEnable(GL_FRONT_AND_BACK);

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
    if(labyrinth != 0)
        delete labyrinth;
    labyrinth = new Labyrinth();

    labyrinth->addWall(QVector3D(20, 0.5, 20), QVector3D(0, -0.5, 0));
    labyrinth->addWall(QVector3D(20, 0.5, 20), QVector3D(0,  6.5, 0));

    labyrinth->addWall(QVector3D(0.5, 3, 0.5), QVector3D(-19.5, 3, -19.5));
    labyrinth->addWall(QVector3D(0.5, 3, 0.5), QVector3D( 19.5, 3, -19.5));
    labyrinth->addWall(QVector3D(0.5, 3, 0.5), QVector3D( 19.5, 3,  19.5));
    labyrinth->addWall(QVector3D(0.5, 3, 0.5), QVector3D(-19.5, 3,  19.5));

    labyrinth->addWall(QVector3D( 19, 3, 0.5), QVector3D(  0, 3, -19.5));
    labyrinth->addWall(QVector3D(0.5, 3,  19), QVector3D( 19.5, 3, 0));
    labyrinth->addWall(QVector3D( 19, 3, 0.5), QVector3D(  0, 3,  19.5));
    labyrinth->addWall(QVector3D(0.5, 3,  19), QVector3D(-19.5, 3, 0));

    labyrinth->addWallMask(0, BoxDrawObject::Bottom);
    labyrinth->addWallMask(1, BoxDrawObject::Top);
    //labyrinth->addWallMask(2, BoxDrawObject::Front | BoxDrawObject::Right);
    //labyrinth->addWallMask(3, BoxDrawObject::Front | BoxDrawObject::Left);
    //labyrinth->addWallMask(4, BoxDrawObject::Back | BoxDrawObject::Left);
    //labyrinth->addWallMask(5, BoxDrawObject::Back | BoxDrawObject::Right);
    labyrinth->addWallMask(6, BoxDrawObject::Back | BoxDrawObject::Left| BoxDrawObject::Right);
    labyrinth->addWallMask(7, BoxDrawObject::Right | BoxDrawObject::Front | BoxDrawObject::Back);
    labyrinth->addWallMask(8, BoxDrawObject::Front | BoxDrawObject::Left| BoxDrawObject::Right);
    labyrinth->addWallMask(9, BoxDrawObject::Left | BoxDrawObject::Front | BoxDrawObject::Back);

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


    labyrinth->addWall(QVector3D(16.5, 3, 0.5), QVector3D(-2.5, 3, -13.5));
    labyrinth->addWall(QVector3D(0.5, 3, 13.5), QVector3D(13.5, 3, -0.5));
    labyrinth->addWall(QVector3D( 10, 3, 0.5), QVector3D(4, 3, 13.5));
    labyrinth->addWall(QVector3D(0.5, 3, 9.5), QVector3D(-5.5, 3, 4.5));

    labyrinth->addBall(QVector3D(0, 3, 0));
    labyrinth->addFinish(QVector3D(-17, 3, -17));
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
    if(labyrinth == 0)
        return;
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

    /*
    //---Search nearest box--
    program.bind();
    program.setUniformValue("rendTexture", 0);

    for(int i = 0; i < qMin(labyrinth->getWallCount(), 10); i++) {
        // Set modelview-projection matrix
        program.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getWallMatrix(i));
        program.setUniformValue("color", labyrinth->getWallColor(i));

        // Draw cube geometry
        boxDraw->draw(&program);
    }

    glReadPixels(geometry().width() / 2, geometry().height() / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, centralPixel);

    ignoreColorId.setRgb(centralPixel[0], centralPixel[1], centralPixel[2]);
    program.setUniformValue("rendTexture", rendTexture);
    //--- ---
*/
    ignoreColorId.setRgb(0, 0, 0);
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

            lightingProgram.setUniformValue("color", colors[i]);

            // Draw cube geometry
            boxDraw->setMask(labyrinth->getWallMask(i));
            boxDraw->draw(&lightingProgram);\
        }
    }

    // Ball
    // Set modelview matrix
    lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getBallMatrix());
    // Set modelview-projection matrix
    lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getBallMatrix());
    lightingProgram.setUniformValue("color", QColor(0, 0, 0));
    // Draw cube geometry
    boxDraw->draw(&lightingProgram);\

    // Finish
    // Set modelview matrix
    lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getFinishMatrix());
    // Set modelview-projection matrix
    lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getFinishMatrix());
    lightingProgram.setUniformValue("color", QColor(255, 255, 255));
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
    angularSpeed *= 0.99;

    // Stop rotation when speed goes below threshold
    if (angularSpeed < 0.01) {
        angularSpeed = 0.0;
    } else {
        // Update rotation
        rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;       
    }

    if(labyrinth->checkFinish()) {
        int currentTime = QTime::currentTime().msecsSinceStartOfDay();
        if(lastTimeForEnd == 0)
            lastTimeForEnd = currentTime;
        if(currentTime - lastTimeForEnd > 5000) {
            timer.stop();
            emit endGame();
            close();
        }
    } else {
        QMatrix4x4 matrix;
        matrix.rotate(rotation);
        labyrinth->setGravity((QVector3D(0, -10, 0) * matrix).normalized() * 30);
        //Step simulation
        labyrinth->step();
    }
    // Request an update
    update();
}

void OGLWindow::fps() {
    int currentTime = QTime::currentTime().msecsSinceStartOfDay();

    //Увеличиваем счетчик кадров
    framesPerSecond++;

    //Теперь вычтем из текущего времени последнее запомненное время. Если результат больше единицы,
    //это значит, что секунда прошла и нужно вывести новый FPS.
    if(currentTime - lastTimeForFPS > 1000)
    {
        //Устанавливаем lastTime в текущее время. Теперь оно будет использоватся как предидущее время
        //для след. секунды.
        lastTimeForFPS = currentTime;

        // Установим FPS для вывода:
        qDebug() << "FPS: " << framesPerSecond;

        //Сбросим FPS
        framesPerSecond = 0;
    }
}
