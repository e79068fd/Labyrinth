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


LabyrinthObject::LabyrinthObject(const QVector3D& translate) {
    matrix.translate(translate);

    btCollisionShape* shape = new btBoxShape(btVector3(0.5, 0.5, 0.5));

    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate)));
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, shape, btVector3(0, 0, 0));
    body = new btRigidBody(rigidBodyCI);
}

btRigidBody* LabyrinthObject::getBody() const {
    return body;
}

const QMatrix4x4& LabyrinthObject::getMatrix() const {
    return matrix;
}


Wall::Wall(const QVector3D& s, const QVector3D& translate) {
    QVector3D size = s - QVector3D(0.005, 0.005, 0.005);
    matrix.translate(translate);
    matrix.scale(size);

    btCollisionShape* wallShape = new btBoxShape(toBtVector3(size));

    btDefaultMotionState* wallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate)));
    btRigidBody::btRigidBodyConstructionInfo wallRigidBodyCI(0, wallMotionState, wallShape, btVector3(0, 0, 0));
    body = new btRigidBody(wallRigidBodyCI);
}

void Wall::setMask(int m) {
    mask = m;
}

int Wall::getMask() const{
    return mask;
}


Ball::Ball(const QVector3D &translate) {
    btCollisionShape* ball = new btSphereShape(1);

    btDefaultMotionState* ballMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), toBtVector3(translate)));
    btScalar mass = 0.1;
    btVector3 inertia(0, 0, 0);
    ball->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo ballRigidBodyCI(mass, ballMotionState, ball, inertia);
    body = new btRigidBody(ballRigidBodyCI);

    update();
}

void Ball::update() {
    btTransform transform;
    body->getMotionState()->getWorldTransform(transform);

    float oglArray[16];
    transform.getOpenGLMatrix(oglArray);
    matrix = toQMatrix4x4(oglArray);

    btQuaternion q = transform.getRotation();
    rotation = QQuaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}

const QQuaternion& Ball::getRotation() const {
    return rotation;
}


Labyrinth::Labyrinth() {
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

void Labyrinth::addWall(const Wall& w) {
    wall.push_back(w);
    dynamicsWorld->addRigidBody(w.getBody());
}

void Labyrinth::addBall(const Ball& b) {
    ball.push_back(b);
    dynamicsWorld->addRigidBody(b.getBody());
}

void Labyrinth::addFinish(const LabyrinthObject& f) {
    finish.push_back(f);
    dynamicsWorld->addRigidBody(f.getBody());
}

const QVector<Wall>& Labyrinth::getWall() {
    return wall;
}

const QVector<Ball>& Labyrinth::getBall() {
    return ball;
}

const QVector<LabyrinthObject>& Labyrinth::getFinish() {
    return finish;
}

void Labyrinth::setGravity(const QVector3D& g) {
    gravity = g;
}

void Labyrinth::setRotation(const QQuaternion& rotation) {
    QMatrix4x4 matrix;
    matrix.rotate(rotation);
    for(Ball b: ball) {
        b.getBody()->setGravity(toBtVector3(gravity * matrix * 5));
        b.getBody()->activate(true);
    }
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
        for(LabyrinthObject f: finish) {
            if(obA == f.getBody() || obB == f.getBody())
                isFinish = true;
        }
    }


    for(Ball& b: ball) {
        b.update();
    }
}
