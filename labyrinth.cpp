#include "labyrinth.h"

Labyrinth::Labyrinth() {
    r = 0; g = 0; b = 0;
}

Labyrinth::~Labyrinth() {

}

void Labyrinth::addWall(const QVector3D& size, const QVector3D& translate) {
    QMatrix4x4 matrix;
    matrix.translate(size / 2 + translate);
    matrix.scale(size / 2);

    wallMatrix.push_back(matrix);
    wallColor.push_back(getNewColor());
    ignore.push_back(QVector<QColor>());
}

void Labyrinth::addIgnore(int index, QColor color) {
    ignore[index].push_back(color);
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

bool Labyrinth::isIgnore(int index, const QColor& color) {
    return !(index >= ignore.size() || ignore[index].indexOf(color) == -1);
}
