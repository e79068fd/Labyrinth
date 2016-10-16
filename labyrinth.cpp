#include "labyrinth.h"

Labyrinth::Labyrinth() {
    r = 0; g = 0; b = 0;

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

}

btVector3 toBtVector3(const QVector3D& v) {
    return btVector3(v.x(), v.y(), v.z());
}

void Labyrinth::addWall(const QVector3D& size, const QVector3D& translate) {
    // Render part
    QMatrix4x4 matrix;
    matrix.translate((size - QVector3D(0.1, 0.1, 0.1))/ 2 + translate);
    matrix.scale((size - QVector3D(0.1, 0.1, 0.1)) / 2);

    wallMatrix.push_back(matrix);
    wallColor.push_back(getNewColor());
    ignore.push_back(QVector<QColor>());

    // Physics part
    btCollisionShape* wallShape = new btBoxShape(toBtVector3(size - QVector3D(0.1, 0.1, 0.1)));

    btDefaultMotionState* wallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate - QVector3D(0.05, 0.05, 0.05))));
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
    btScalar mass = 1;
    btVector3 inertia(0, 0, 0);
    ball->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(mass, ballMotionState, ball, inertia);
    ballRigidBody = new btRigidBody(ballRigidBodyCI);
    dynamicsWorld->addRigidBody(ballRigidBody);
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
    QMatrix4x4 result;
    btTransform transform;
    ballRigidBody->getMotionState()->getWorldTransform(transform);
    float oglArray[16];
    transform.getOpenGLMatrix(oglArray);
    for(int i = 0; i < 4; i++) {
        result.setColumn(i, QVector4D(oglArray[i * 4], oglArray[i * 4 + 1], oglArray[i * 4 + 2], oglArray[i * 4 + 3]));
    }
    return result;
}

void Labyrinth::setGravity(const QVector3D& v) {
    dynamicsWorld->setGravity(toBtVector3(v));
}

bool Labyrinth::isIgnore(int index, const QColor& color) {
    return !(index >= ignore.size() || ignore[index].indexOf(color) == -1);
}

void Labyrinth::step() {
    dynamicsWorld->stepSimulation(1 / 60.f, 30);
}
