#include "labyrinth.h"


btVector3 toBtVector3(const QVector3D& v) {
    return btVector3(v.x(), v.y(), v.z());
}

QVector3D toQVector3D(const btVector3& v) {
    return QVector3D(v.getX(), v.getY(), v.getZ());
}

QMatrix4x4 toQMatrix4x4(btScalar* m) {
    QMatrix4x4 result;
    for(int i = 0; i < 4; i++) {
        result.setColumn(i, QVector4D(m[i * 4], m[i * 4 + 1], m[i * 4 + 2], m[i * 4 + 3]));
    }
    return result;
}

Labyrinth::Labyrinth() {
    r = 0; g = 0; b = 0;
    isFinish = false;

    //Init Physics

    // Build the broadphase
    broadphase = new btDbvtBroadphase();

    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;

    // The world.
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

Labyrinth::~Labyrinth() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

void Labyrinth::addWall(const QVector3D& size, const QVector3D& translate) {
    // Render part
    QMatrix4x4 matrix;
    matrix.translate(translate);
    matrix.scale(size - QVector3D(0.005, 0.005, 0.005));

    wallMatrix.push_back(matrix);
    wallColor.push_back(getNewColor());
    ignore.push_back(QVector<QColor>());

    // Physics part
    btCollisionShape* wallShape = new btBoxShape(toBtVector3(size));

    btDefaultMotionState* wallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate)));
    btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI(0, wallMotionState, wallShape, btVector3(0, 0, 0));
    btRigidBody* wallRigidBody = new btRigidBody(wallRigidBodyCI);
    dynamicsWorld->addRigidBody(wallRigidBody);
}

void Labyrinth::addIgnore(int index, QColor color) {
    ignore[index].push_back(color);
}

void Labyrinth::addBall(const QVector3D& translate) {
    btCollisionShape* ball = new btSphereShape(1);

    btDefaultMotionState* ballMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate)));
    btScalar mass = 0.1;
    btVector3 inertia(0, 0, 0);
    ball->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(mass, ballMotionState, ball, inertia);
    ballRigidBody = new btRigidBody(ballRigidBodyCI);
    dynamicsWorld->addRigidBody(ballRigidBody);
}

void Labyrinth::addFinish(const QVector3D &translate) {
    finishMatrix.translate(translate);

    btCollisionShape* finishShape = new btBoxShape(btVector3(1, 1, 1));

    btDefaultMotionState* finishMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate)));
    btRigidBody::btRigidBodyConstructionInfo finishRigidBodyCI(0, finishMotionState, finishShape, btVector3(0, 0, 0));
    finishRigidBody = new btRigidBody(finishRigidBodyCI);
    dynamicsWorld->addRigidBody(finishRigidBody);
}

const QMatrix4x4& Labyrinth::getWallMatrix(int index) {
    return wallMatrix[index];
}

const QColor& Labyrinth::getWallColor(int index) {
    return wallColor[index];
}

const QVector<QColor>& Labyrinth::getIgnore(int index) {
    return ignore[index];
}

int Labyrinth::getWallCount() {
    return wallMatrix.size();
}

QColor Labyrinth::getNewColor() {
    r++;
    if(r > 255) {
        r = 0;
        g++;
        if(g > 255) {
            g = 0;
            b++;
            //b > 255
        }
    }
    return QColor(r, g, b);
}

QMatrix4x4 Labyrinth::getBallMatrix(int index) {
    btTransform transform;
    ballRigidBody->getMotionState()->getWorldTransform(transform);
    float oglArray[16];
    transform.getOpenGLMatrix(oglArray);
    return toQMatrix4x4(oglArray);
}

const QMatrix4x4& Labyrinth::getFinishMatrix(int index) {
    return finishMatrix;
}

void Labyrinth::setGravity(const QVector3D& v) {
    ballRigidBody->setGravity(toBtVector3(v));
    ballRigidBody->activate(true);
}

bool Labyrinth::isIgnore(int index, const QColor& color) {
    return !(index >= ignore.size() || ignore[index].indexOf(color) == -1);
}

bool Labyrinth::checkFinish() {
    return isFinish;
}

void Labyrinth::step() {
    dynamicsWorld->stepSimulation(1 / 60.f, 10);

    int n = dynamicsWorld->getDispatcher()->getNumManifolds();
    for(int i = 0; i < n; i++) {
        btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btCollisionObject* obA = const_cast<btCollisionObject*>(contactManifold->getBody0());
        btCollisionObject* obB = const_cast<btCollisionObject*>(contactManifold->getBody1());
        if(obA == finishRigidBody || obB == finishRigidBody)
            isFinish = true;
    }
}
