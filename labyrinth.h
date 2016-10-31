#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMatrix4x4>
#include <QDebug>
#include <btBulletDynamicsCommon.h>

class LabyrinthObject {
public:
    LabyrinthObject() { }
    LabyrinthObject(const QVector3D& translate);

    btRigidBody* getBody() const;
    const QMatrix4x4& getMatrix() const;
protected:
    btRigidBody* body;
    QMatrix4x4 matrix;
};

class Wall: public LabyrinthObject {
public:
    Wall() { }
    Wall(const QVector3D& s, const QVector3D& translate);

    void setMask(int m);
    int getMask() const;
private:
    int mask;
};

class Ball: public LabyrinthObject {
public:
    Ball() { }
    Ball(const QVector3D& translate);

    void update();
    const QQuaternion& getRotation() const;
private:
    QQuaternion rotation;
};

class Labyrinth {

public:
    Labyrinth();
    ~Labyrinth();

    void addWall(const Wall& w);
    void addBall(const Ball& b);
    void addFinish(const LabyrinthObject& f);

    const QVector<Wall>& getWall();
    const QVector<Ball>& getBall();
    const QVector<LabyrinthObject>& getFinish();

    void setGravity(const QVector3D& g);
    void setRotation(const QQuaternion& rotation);

    bool checkFinish();

    void step();

private:
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    QVector<Wall> wall;
    QVector<Ball> ball;
    QVector<LabyrinthObject> finish;

    bool isFinish;

    QVector3D gravity;
};

#endif // LABYRINTH_H
