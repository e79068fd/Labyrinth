#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMatrix4x4>
#include <QDebug>
#include <btBulletDynamicsCommon.h>

class Labyrinth {

public:
    Labyrinth();
    ~Labyrinth();

    void addWall(const QVector3D& size, const QVector3D& translate);
    void addIgnore(int index, QColor color);
    void addBall(const QVector3D& translate);
    void addFinish(const QVector3D& translate);
    void addWallMask(int index, int mask);

    const QMatrix4x4& getWallMatrix(int index);
    const QColor& getWallColor(int index);
    const QVector<QColor>& getIgnore(int index);
    int getWallCount();
    int getWallMask(int index);

    QMatrix4x4 getBallMatrix(int index = 0);
    const QMatrix4x4& getFinishMatrix(int index = 0);

    void setGravity(const QVector3D& v);

    bool isIgnore(int index, const QColor &color);
    bool checkFinish();

    void step();

private:
    QVector<QMatrix4x4> wallMatrix;
    QVector<QColor> wallColor;
    QVector<QVector<QColor>> ignore;

    int r, g, b;
    QColor getNewColor();

    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    btRigidBody* ballRigidBody;

    btRigidBody* finishRigidBody;
    QMatrix4x4 finishMatrix;
    bool isFinish;

    QMap<int, int> wallMask;
};

#endif // LABYRINTH_H
