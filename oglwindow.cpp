#include "oglwindow.h"

OGLWindow::OGLWindow() :
    texture(0),
    angularSpeed(0),
    labyrinth(0)
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    setFormat(format);

    rendTexture = 0;

    framesPerSecond = 0;
    lastTimeForFPS = 0;
}

void OGLWindow::startGame() {
    initLabyrinth();

    lastTimeForEnd = 0;
    angularSpeed = 0;
    rotationAxis = QVector3D();
    rotation = QQuaternion();

    // Use QBasicTimer because its faster than QTimer
    timer.start(1000/60, this);

    accelerometer.start();
}

OGLWindow::~OGLWindow()
{
    makeCurrent();
    delete texture;
    delete labyrinth;
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
    if(type > 2) return;
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
    //glEnable(GL_FRONT_AND_BACK);

    QVector2D texcoords[] = {
        QVector2D(0, 0),
        QVector2D(0.33, 0),
        QVector2D(0, 0.5),
        QVector2D(0.33, 0.5)
    };
    plane = new PlaneDrawObject;
    plane->setTexture(texcoords);

    boxDraw = new BoxDrawObject;

    sphere1 = new SphereDrawObject(1);
    sphere3 = new SphereDrawObject(3);
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

    LabyrinthGenerator generator;
    generator.generate();
    generator.matchLabyrinth(labyrinth);
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
    texture->bind();

    lightingProgram.bind();

    // Calculate model view transformation
    QMatrix4x4 matrix;

    matrix.translate(0.0, 0.0, -5.0);
    matrix.rotate(rotation);
    matrix.scale(0.075);

    QMatrix4x4 normalMatrix;
    normalMatrix.rotate(rotation);
    lightingProgram.setUniformValue("normal_matrix", normalMatrix);
    //---render---

    // Ball
    // Set modelview matrix
    QMatrix4x4 normalMatrixForBall;
    normalMatrixForBall.rotate(rotation);
    normalMatrixForBall.rotate(labyrinth->getBallRotation());
    lightingProgram.setUniformValue("normal_matrix", normalMatrixForBall);
    lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getBallMatrix());

    // Set modelview-projection matrix
    lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getBallMatrix());

    lightingProgram.setUniformValue("color", QColor(192, 192, 192));

    // Draw sphere geometry
    sphere3->draw(&lightingProgram);

    lightingProgram.setUniformValue("normal_matrix", normalMatrix);
    // Finish
    // Set modelview matrix
    lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getFinishMatrix());

    // Set modelview-projection matrix
    lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getFinishMatrix());

    lightingProgram.setUniformValue("color", QColor(255, 255, 255));

    // Draw sphere geometry
    sphere1->draw(&lightingProgram);

    lightingProgram.setUniformValue("color", QColor(50, 205, 50, 200));
    for(int i = 0; i < labyrinth->getWallCount(); i++) {
        // Set modelview matrix
        lightingProgram.setUniformValue("mvp_matrix", matrix * labyrinth->getWallMatrix(i));

        // Set modelview-projection matrix
        lightingProgram.setUniformValue("mvp_matrix", projection * matrix * labyrinth->getWallMatrix(i));

        // Draw cube geometry
        boxDraw->setMask(labyrinth->getWallMask(i));
        boxDraw->draw(&lightingProgram);
    }
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
            accelerometer.stop();
            emit endGame();
            close();
        }
    } else {
        QMatrix4x4 matrix;
        matrix.rotate(rotation);
        QAccelerometerReading* read = accelerometer.reading();
        QVector3D gravity;
        if(read)
            gravity = QVector3D(read->x(), read->y(), read->z()) * -1;
        else
            gravity = QVector3D(0, 0, -10);
        labyrinth->setGravity((gravity * matrix).normalized() * 30);
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
