#ifndef LABYRINTH_H
#define LABYRINTH_H

#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QMatrix4x4>
#include <QDebug>

class Labyrinth {

public:
    Labyrinth();
    ~Labyrinth();
    void addWall(const QVector3D& size, const QVector3D& translate);
    void addIgnore(int index, QColor color);
    const QMatrix4x4& getWallMatrix(int index);
    const QColor& getWallColor(int index);
    const QVector<QColor>& getIgnore(int index);
    int getWallCount();
    bool isIgnore(int index, const QColor &color);

private:
    QVector<QMatrix4x4> wallMatrix;
    QVector<QColor> wallColor;
    QVector<QVector<QColor>> ignore;
    int r, g, b;

    QColor getNewColor();
};

#endif // LABYRINTH_H
